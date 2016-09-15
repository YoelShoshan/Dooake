#include "stdafx.h"
#include "Frustum.h"

#include <math.h>
#include <windows.h>

/*#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;*/

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

#include "Profiler.h"

#include "MatrixGenerator.h"



enum FrustumSide
{
	RIGHT	= 0,
	LEFT	= 1,
	BOTTOM	= 2,
	TOP		= 3,
	BACK	= 4,
	FRONT	= 5	
}; 



CFrustum::CFrustum()
{
	m_iPlanesNum = 0;
	m_pPlanes = NULL;
}

CFrustum::~CFrustum()
{
	SafeDeleteArray(m_pPlanes);
	m_iPlanesNum = 0;
}



void CFrustum::AllocPlanes(int iNum)
{
	m_iPlanesNum = iNum;
	m_pPlanes = new CPlane[m_iPlanesNum];	
}


//void CFrustum::CalculateCornerPoints(const float* pfView, const float* pfProjection)
void CFrustum::CalculateCornerPoints(const float* pfView, const float* pfProjection)
{
	CMatrix mView;
	mView.SetTransposed(pfView);

	CMatrix mProjection;
	mProjection.SetTransposed(pfProjection);

	float viewProjection[16];
	(mView*mProjection).ReturnTransposedArray(viewProjection);


	CMatrix mInvViewProj;
	
	mInvViewProj.Set(viewProjection);
	mInvViewProj.Inverse();


	


	m_pCorners[FRUSTUM_BACK_UP_RIGHT_CORNER].Set(1.f,1.f,1.f,1.f);
	m_pCorners[FRUSTUM_BACK_UP_LEFT_CORNER].Set(-1.f,1.f,1.f,1.f);
	m_pCorners[FRUSTUM_BACK_DOWN_RIGHT_CORNER].Set(1.f,-1.f,1.f,1.f);
	m_pCorners[FRUSTUM_BACK_DOWN_LEFT_CORNER].Set(-1.f,-1.f,1.f,1.f);

	m_pCorners[FRUSTUM_FRONT_UP_RIGHT_CORNER].Set(1.f,1.f,0.f,1.f);
	m_pCorners[FRUSTUM_FRONT_UP_LEFT_CORNER].Set(-1.f,1.f,0.f,1.f);
	m_pCorners[FRUSTUM_FRONT_DOWN_RIGHT_CORNER].Set(1.f,-1.f,0.f,1.f);
	m_pCorners[FRUSTUM_FRONT_DOWN_LEFT_CORNER].Set(-1.f,-1.f,0.f,1.f);


	for (int i=0;i<FRUSTUM_CORNERS_NUM;i++)
	{
		m_pCorners[i] = mInvViewProj * m_pCorners[i];

		m_pCorners[i].Set(m_pCorners[i].x / m_pCorners[i].w,
						m_pCorners[i].y / m_pCorners[i].w,
						m_pCorners[i].z / m_pCorners[i].w,
						1.f);
	}
}

void CFrustum::ProjectPoints(CVector4f* points,int iNum, CMatrix& mat)
{
	for (int i=0;i<iNum;i++)
		{
			points[i] = mat * points[i];
			points[i]/=points[i].w;
			points[i].Set( (points[i].x*0.5f)+0.5f,
									(points[i].y*0.5f)+0.5f,
									points[i].z,
									points[i].w);
		}


}

void CFrustum::ProjectCornerPoints(const float* pfView, const float* pfProjection,CFrustum* pCamFrustum)
{
	static const int edgeIndices[12][2] =
	{

		{FRUSTUM_BACK_DOWN_LEFT_CORNER	, FRUSTUM_BACK_UP_LEFT_CORNER},
		{FRUSTUM_BACK_UP_LEFT_CORNER	, FRUSTUM_BACK_UP_RIGHT_CORNER },
		{FRUSTUM_BACK_UP_RIGHT_CORNER   , FRUSTUM_BACK_DOWN_RIGHT_CORNER },
		{FRUSTUM_BACK_DOWN_RIGHT_CORNER , FRUSTUM_BACK_DOWN_LEFT_CORNER },

		{FRUSTUM_FRONT_DOWN_LEFT_CORNER , FRUSTUM_FRONT_UP_LEFT_CORNER },
		{FRUSTUM_FRONT_UP_LEFT_CORNER   , FRUSTUM_FRONT_UP_RIGHT_CORNER },
		{FRUSTUM_FRONT_UP_RIGHT_CORNER  , FRUSTUM_FRONT_DOWN_RIGHT_CORNER },
		{FRUSTUM_FRONT_DOWN_RIGHT_CORNER, FRUSTUM_FRONT_DOWN_LEFT_CORNER },
	 
		{FRUSTUM_BACK_DOWN_LEFT_CORNER  , FRUSTUM_FRONT_DOWN_LEFT_CORNER },
		{FRUSTUM_BACK_UP_LEFT_CORNER    , FRUSTUM_FRONT_UP_LEFT_CORNER },
		{FRUSTUM_BACK_UP_RIGHT_CORNER   , FRUSTUM_FRONT_UP_RIGHT_CORNER },
		{FRUSTUM_BACK_DOWN_RIGHT_CORNER , FRUSTUM_FRONT_DOWN_RIGHT_CORNER }
	};

	BREAKPOINT(!pCamFrustum);

	CPlane* pPlanes = pCamFrustum->GetPlanes();
	
	CPlane front = pPlanes[FRONT];
	front.d = -front.d;

	bool boundingBools[8];
	ZeroMemory(boundingBools,sizeof(bool)*8);
		
	//bool boundingBools[8]={false};
	
	static CVec3f vProjected[20];
	int iProjected=0;

	for (int i=0;i<8;i++)
	{
		CVec3f corner(m_pCorners[i].x,m_pCorners[i].y,m_pCorners[i].z);

		float fRes = Dot(front.n,corner)-front.d;

		if ( fRes > 0.f)
		{
			boundingBools[i]=true;
			vProjected[iProjected++]=corner;
		}
	}



	// project any points along edges into near clipping plane
	for (int i = 0; i < 12; i++)
	{
		int index1 = edgeIndices[i][0];
		int index2 = edgeIndices[i][1];

		// if one or the other but not both points of an edge on
		// the cube is behind the near clip plane, then we want
		// to intersect the edge line segment with the near clip plane
		if (boundingBools[index1]^boundingBools[index2])
		{
			CVec3f corner1(m_pCorners[index1].x,m_pCorners[index1].y,m_pCorners[index1].z);
			CVec3f corner2(m_pCorners[index2].x,m_pCorners[index2].y,m_pCorners[index2].z);
			

			
			//t=(d-N*A)/(N*(B-A))

			CVec3f edge = corner2 -corner1;
			float tLine = (front.d - Dot(front.n,corner1)  ) / Dot(front.n,edge);
			vProjected[iProjected++] = corner1 + edge * tLine;
		}
	}

	if (iProjected>0)
	{
		CMatrix view;
		view.Set(pfView);
	
		CMatrix proj;
		proj.Set(pfProjection);

		CMatrix viewProjection=proj*view;

		for (int i=0;i<iProjected;i++)
			m_ActiveProjectedCorners[i].Set(vProjected[i].x,vProjected[i].y,vProjected[i].z,1.f);

		ProjectPoints(m_ActiveProjectedCorners,iProjected,viewProjection);
						
	}
	else
	{
		m_fXmin=m_fXmax=m_fYmin=m_fYmax=0;
		return;
	}


		

	m_fXmin = 999999.f;
	m_fYmin = 999999.f;
	m_fXmax = -999999.f;
	m_fYmax = -999999.f;

	for (int i=0;i<iProjected;i++)
	{
		if (m_ActiveProjectedCorners[i].x < m_fXmin)
			m_fXmin = m_ActiveProjectedCorners[i].x;

		if (m_ActiveProjectedCorners[i].y < m_fYmin)
			m_fYmin = m_ActiveProjectedCorners[i].y;


		if (m_ActiveProjectedCorners[i].x > m_fXmax)
			m_fXmax = m_ActiveProjectedCorners[i].x;

		if (m_ActiveProjectedCorners[i].y > m_fYmax)
			m_fYmax = m_ActiveProjectedCorners[i].y;
	}

	// value -> [0,1]

	if (m_fXmin < 0.f)
		m_fXmin = 0.f;
	if (m_fXmin > 1.f)
		m_fXmin = 1.f;

	if (m_fXmax < 0.f)
		m_fXmax = 0.f;
	if (m_fXmax > 1.f)
		m_fXmax = 1.f;

	if (m_fYmin < 0.f)
		m_fYmin = 0.f;
	if (m_fYmin > 1.f)
		m_fYmin = 1.f;

	if (m_fYmax < 0.f)
		m_fYmax = 0.f;
	if (m_fYmax > 1.f)
		m_fYmax = 1.f;

	

}



///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This extracts the frustum from the OpenGL projection and modelview matrix.
/////
///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFrustum::CalculateFrustum(const float* pfView, const float* pfProjection)
{   



	if (m_iPlanesNum<6)
	{
		SafeDeleteArray(m_pPlanes);
		AllocPlanes(6);
	}
        

	CMatrix mView;
	mView.SetTransposed(pfView);

	CMatrix mProjection;
	mProjection.SetTransposed(pfProjection);

	float clip[16];
	(mView*mProjection).ReturnTransposedArray(clip);


	


	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.

	// This will extract the RIGHT side of the frustum
	m_pPlanes[RIGHT].n.v[0] = clip[ 3] - clip[ 0];
	m_pPlanes[RIGHT].n.v[1] = clip[ 7] - clip[ 4];
	m_pPlanes[RIGHT].n.v[2] = clip[11] - clip[ 8];
	m_pPlanes[RIGHT].d = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	m_pPlanes[RIGHT].Normalize();

	// This will extract the LEFT side of the frustum
	m_pPlanes[LEFT].n.v[0] = clip[ 3] + clip[ 0];
	m_pPlanes[LEFT].n.v[1] = clip[ 7] + clip[ 4];
	m_pPlanes[LEFT].n.v[2] = clip[11] + clip[ 8];
	m_pPlanes[LEFT].d = clip[15] + clip[12];

	// Normalize the LEFT side
	m_pPlanes[LEFT].Normalize();


	// This will extract the BOTTOM side of the frustum
	m_pPlanes[BOTTOM].n.v[0] = clip[ 3] + clip[ 1];
	m_pPlanes[BOTTOM].n.v[1] = clip[ 7] + clip[ 5];
	m_pPlanes[BOTTOM].n.v[2] = clip[11] + clip[ 9];
	m_pPlanes[BOTTOM].d = clip[15] + clip[13];

	// Normalize the BOTTOM side
	m_pPlanes[BOTTOM].Normalize();

	// This will extract the TOP side of the frustum
	m_pPlanes[TOP].n.v[0] = clip[ 3] - clip[ 1];
	m_pPlanes[TOP].n.v[1] = clip[ 7] - clip[ 5];
	m_pPlanes[TOP].n.v[2] = clip[11] - clip[ 9];
	m_pPlanes[TOP].d = clip[15] - clip[13];

	// Normalize the TOP side
	m_pPlanes[TOP].Normalize();


	// This will extract the BACK side of the frustum
	m_pPlanes[BACK].n.v[0] = clip[ 3] - clip[ 2];
	m_pPlanes[BACK].n.v[1] = clip[ 7] - clip[ 6];
	m_pPlanes[BACK].n.v[2] = clip[11] - clip[10];
	m_pPlanes[BACK].d = clip[15] - clip[14];

	// Normalize the BACK side
	m_pPlanes[BACK].Normalize();


	// This will extract the FRONT side of the frustum
	m_pPlanes[FRONT].n.v[0] = clip[ 3] + clip[ 2];
	m_pPlanes[FRONT].n.v[1] = clip[ 7] + clip[ 6];
	m_pPlanes[FRONT].n.v[2] = clip[11] + clip[10];
	m_pPlanes[FRONT].d = clip[15] + clip[14];

	// Normalize the FRONT side
	m_pPlanes[FRONT].Normalize();
	

}


/*
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a point is inside of the frustum
/////
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::PointInFrustum( float x, float y, float z )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(m_pPlanes[i].n.v[0] * x + m_pPlanes[i].n.v[1] * y + m_pPlanes[i].n.v[2] * z + m_pPlanes[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a sphere is inside of our frustum by it's center and radius.
/////
///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::SphereInFrustum( float x, float y, float z, float radius )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < m_iPlanesNum; i++ )	
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( m_pPlanes[i].n.v[0] * x + m_pPlanes[i].n.v[1] * y + m_pPlanes[i].n.v[2] * z + m_pPlanes[i].d <= -radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}
	
	// The sphere was inside of the frustum!
	return true;
}


///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a cube is in or around our frustum by it's center and 1/2 it's length
/////
///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::CubeInFrustum( float x, float y, float z, float size )
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	for(int i = 0; i < 6; i++ )
	{
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

*/
///////////////////////////////// BOX IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a BOX is in or around our frustum by it's min and max points
/////
///////////////////////////////// BOX IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

#define V_IN 1
#define V_OUT 2
#define V_INTERSECT 3



int CFrustum::BoxInFrustum( float x, float y, float z, float x2, float y2, float z2) const
{
	static CProfileStep step("BoxFrustum");	
	step.Begin();

	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)    mode |= V_IN;  // set IN bit to 1
																									else mode |= V_OUT; // set OUT bit to 1
		if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and 
											// a vertex OUT of ANY plane continue ( we have enough information to say: 
											// INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
		
		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till know, if 
		// there is a vertice in, continue (we are not over!) 
		if (mode == V_IN) continue;

		step.End();
		// there is not vertex IN front of this plane, so the box is COMPLETE_OUT
		return COMPLETE_OUT;
	}

	step.End();

	// All planes has a vertex IN FRONT so or the box is intersecting or complete IN
	if (mode == V_INTERSECT) return INTERSECT;
			            else return COMPLETE_IN;
									

/*

	static CVec3f vCenter;

	vCenter.v[0] = (x+x2)/2.f;
	vCenter.v[1] = (y+y2)/2.f;
	vCenter.v[2] = (z+z2)/2.f;
    
     float R=x2-x; 
     float S=y2-y; 
     float T=z2-z; 


	 for (int i=0;i<m_iPlanesNum;i++)
	 {
	     float rEff=0.5*(fabs(R*m_pPlanes[i].n.v[0])+fabs(S*m_pPlanes[i].n.v[1])+fabs(T*m_pPlanes[i].n.v[2])); 
          
		 // Box is completely behind 1 plane -> not in Frustum 
          if (m_pPlanes[i].d-m_pPlanes[i].n*vCenter>=rEff) 
               COMPLETE_OUT; 

	 }   
      
     return COMPLETE_IN;
	 
	 // what about intersect?
		 */
	 
}


int CFrustum::BoxInFrustum(CVec3f& vMins, CVec3f& vMaxs) const
{

	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMins.v[1]  + m_pPlanes[i].d >= 0)    mode |= V_IN;  // set IN bit to 1
																									else mode |= V_OUT; // set OUT bit to 1
		if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and 
											// a vertex OUT of ANY plane continue ( we have enough information to say: 
											// INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
		
		if(m_pPlanes[i].n.v[0] * vMaxs.v[0] + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMaxs.v[1] + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMaxs.v[0] + m_pPlanes[i].n.v[1] * vMaxs.v[1] + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMaxs.v[2] + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMaxs.v[0]+ m_pPlanes[i].n.v[1] *vMins.v[1] + m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMins.v[0] + m_pPlanes[i].n.v[1] *vMaxs.v[1]+ m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMaxs.v[0]+ m_pPlanes[i].n.v[1] *vMaxs.v[1]+ m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till know, if 
		// there is a vertice in, continue (we are not over!) 
		if (mode == V_IN) continue;

		// there is not vertex IN front of this plane, so the box is COMPLETE_OUT
		return COMPLETE_OUT;
	}

	// All planes has a vertex IN FRONT so or the box is intersecting or complete IN
	if (mode == V_INTERSECT) return INTERSECT;
			            else return COMPLETE_IN;

	/*	CVec3f vMins(x,y,z);
	CVec3f vMaxs(x2,y2,z2);


     CVec3f vCenter=(vMins+vMaxs)/2.0f; 
     float R=vMaxs.v[0]-vMins.v[0]; 
     float S=vMaxs.v[1]-vMins.v[1]; 
     float T=vMaxs.v[2]-vMins.v[2]; 
      
 
     for (CONSTFRUSTUMPLANEITERATOR it=m_FrustumPlanes.begin();it!=m_FrustumPlanes.end();it++) 
     { 
          float rEff=0.5*(fabs(R*(*it).vNormal.x)+fabs(S*(*it).vNormal.y)+fabs(T*(*it).vNormal.z)); 
 
          // Box is completely behind 1 plane -> not in Frustum 
          if ((*it).d-(*it).vNormal*vCenter>=rEff) 
               return false; 
     } 
 
     return true;*/
}

int CFrustum::PolygonInFrustum(CVec3f* pVerts, int iNum)
{
	BOOL bTemp1=false;

	//for (int i=0;i<m_iPlanesNum;i++)
	for (int i=0;i<m_iPlanesNum;i++)
	{
		bTemp1=false;
		for (int j=0;j<iNum;j++) 
		{
			if (m_pPlanes[i].n*pVerts[j]>m_pPlanes[i].d) // in 
			{				
				bTemp1=true;
				break;
			}
		}

		if (!bTemp1) // all vertices behind this plane
			return  FRUSTUM_CHECK_OUT;		
    }

	return FRUSTUM_CHECK_AT_LEAST_PARTLY_IN;
	
}