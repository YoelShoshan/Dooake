#include "stdafx.h"

#include "Bezier.h"
#include <crtdbg.h>

//#include "VertexBuffer.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

CVec3f EvaluateQuadricBezierCurve(float fT, const CVec3f &vP0, const CVec3f &vP1, const CVec3f &vP2)
{
	// the basis functions
	float b0 = (1-fT)*(1-fT);
	float b1 = 2*(1-fT)*fT;
	float b2 = fT*fT;

	return CVec3f(b0*vP0.v[0]+b1*vP1.v[0]+b2*vP2.v[0],
		      b0*vP0.v[1]+b1*vP1.v[1]+b2*vP2.v[1],
		      b0*vP0.v[2]+b1*vP1.v[2]+b2*vP2.v[2]);
}

CVec2f EvaluateQuadricBezierCurve(float fT,CVec2f &vP0,CVec2f &vP1,CVec2f &vP2)
{

	const float qt=fT*fT;
	const float dt=2.0f*fT;
	return (vP0*(1.0f-dt+qt))+(vP1*(dt-2.0f*qt))+(vP2*qt);

	/*// the basis functions
	float b0 = (1-fT)*(1-fT);
	float b1 = 2*(1-fT)*fT;
	float b2 = fT*fT;

	return CVec3f(b0*vP0.v[0]+b1*vP1.v[0]+b2*vP2.v[0],
		b0*vP0.v[1]+b1*vP1.v[1]+b2*vP2.v[1],
		b0*vP0.v[2]+b1*vP1.v[2]+b2*vP2.v[2]);*/
}

CVec3f evalQuadricBezierDerivative(float t,CVec3f &p0,CVec3f &p1,CVec3f&p2)
{
	return p0*(t-1.0f)+p1*(1.0f-2.0f*t)+p2*t;
}


/*1. I have to make templates here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
2. I have to add normals and colors */



int EvaluateQuadricBezierPatch(CVec3f* pOriginalVerts, CVec3f* pOriginalNormals, CVec2f* pOriginalTC,CVec2f* pOriginalLMC,
							   int iStepsX, int iStepsY,
							   CVec3f* pTargetVerts, CVec3f* pTargetNormals, CVec2f* pTargetTC,CVec2f* pTargetLMC,
							   int iOriginalPatchDimX, int iOriginalPatchDimY,UINT* puiNewIndices,
							   int iStartVertexNum)
{

	assert(_CrtCheckMemory());

	int aMax = iOriginalPatchDimX / 2;
	int bMax = iOriginalPatchDimY / 2;

	int iSub3x3PatchesNum = (aMax)*(bMax);

	int SubPatchSideVertsX = iStepsX+1;
	int SubPatchSideVertsY = iStepsY+1;
	

	int a=0;
	int b=0;

	//////////////////////
 
	float fStepSSize = 1/(float)iStepsX;
	float fStepTSize = 1/(float)iStepsY;
	
	float t_s=0.f,t_t=0.f;

	CVec3f P1,P2,P3; 
	CVec3f nP1,nP2,nP3;
	CVec2f lmP1,lmP2,lmP3;
	CVec2f tcP1,tcP2,tcP3;

	int s=0,t=0;
	int iPos=0;
	int n=0;

	int t_Pos = 0;

	while (b<bMax)
	{
		while(a<aMax)
		{
			t_t=0.f;
			for (t=0;t<iStepsY+1;t++)
			{				
				n = b*2*iOriginalPatchDimX + 2*a;
				
				if (pOriginalVerts && pTargetVerts)
				{
					P1 = EvaluateQuadricBezierCurve(t_t,pOriginalVerts[n],pOriginalVerts[n+iOriginalPatchDimX],pOriginalVerts[n+2*iOriginalPatchDimX]);
					P2 = EvaluateQuadricBezierCurve(t_t,pOriginalVerts[n+1],pOriginalVerts[n+iOriginalPatchDimX+1],pOriginalVerts[n+2*iOriginalPatchDimX+1]);
					P3 = EvaluateQuadricBezierCurve(t_t,pOriginalVerts[n+2],pOriginalVerts[n+iOriginalPatchDimX+2],pOriginalVerts[n+2*iOriginalPatchDimX+2]);
				}
				

				if (pOriginalNormals && pTargetNormals)
				{
					nP1 = EvaluateQuadricBezierCurve(t_t,pOriginalNormals[n],pOriginalNormals[n+iOriginalPatchDimX],pOriginalNormals[n+2*iOriginalPatchDimX]);
					nP2 = EvaluateQuadricBezierCurve(t_t,pOriginalNormals[n+1],pOriginalNormals[n+iOriginalPatchDimX+1],pOriginalNormals[n+2*iOriginalPatchDimX+1]);
					nP3 = EvaluateQuadricBezierCurve(t_t,pOriginalNormals[n+2],pOriginalNormals[n+iOriginalPatchDimX+2],pOriginalNormals[n+2*iOriginalPatchDimX+2]);
				}

				if (pOriginalLMC && pTargetLMC)
				{
					lmP1 = EvaluateQuadricBezierCurve(t_t,pOriginalLMC[n],pOriginalLMC[n+iOriginalPatchDimX],pOriginalLMC[n+2*iOriginalPatchDimX]);
					lmP2 = EvaluateQuadricBezierCurve(t_t,pOriginalLMC[n+1],pOriginalLMC[n+iOriginalPatchDimX+1],pOriginalLMC[n+2*iOriginalPatchDimX+1]);
					lmP3 = EvaluateQuadricBezierCurve(t_t,pOriginalLMC[n+2],pOriginalLMC[n+iOriginalPatchDimX+2],pOriginalLMC[n+2*iOriginalPatchDimX+2]);
				}

				if (pOriginalTC && pTargetTC)
				{
					tcP1 = EvaluateQuadricBezierCurve(t_t,pOriginalTC[n],pOriginalTC[n+iOriginalPatchDimX],pOriginalTC[n+2*iOriginalPatchDimX]);
					tcP2 = EvaluateQuadricBezierCurve(t_t,pOriginalTC[n+1],pOriginalTC[n+iOriginalPatchDimX+1],pOriginalTC[n+2*iOriginalPatchDimX+1]);
					tcP3 = EvaluateQuadricBezierCurve(t_t,pOriginalTC[n+2],pOriginalTC[n+iOriginalPatchDimX+2],pOriginalTC[n+2*iOriginalPatchDimX+2]);
				}

				t_s = 0.f;
				//iPos = 0;

				t_Pos = b*aMax*SubPatchSideVertsX*SubPatchSideVertsY+t*aMax*SubPatchSideVertsX+a*SubPatchSideVertsX;

				
				for (s=0;s<iStepsX+1;s++)
				{
					//t_Pos = a*SubPatchSideVertsX + b*SubPatchSideVertsY + iPos;
					//t_Pos = a*SubPatchSideVertsX + b*(SubPatchSideVertsX*aMax*SubPatchSideVertsY) + t;
					

					//t_Pos = iPos;

					if (pOriginalVerts && pTargetVerts)
						pTargetVerts[t_Pos] = EvaluateQuadricBezierCurve(t_s,P1,P2,P3);

					if (pOriginalNormals && pTargetNormals)
						pTargetNormals[t_Pos] = EvaluateQuadricBezierCurve(t_s,nP1,nP2,nP3);

					if (pOriginalLMC && pTargetLMC)
						pTargetLMC[t_Pos] = EvaluateQuadricBezierCurve(t_s,lmP1,lmP2,lmP3);


					if (pOriginalTC && pTargetTC)
						pTargetTC[t_Pos] = EvaluateQuadricBezierCurve(t_s,tcP1,tcP2,tcP3);

					t_Pos++;

					//iPos++;            
					
					t_s+=fStepSSize;
				}

				t_t+=fStepTSize;
			}
			a++;
		}
		a=0;
		b++;
	}
	
	/// creating indices, each sub sqaure is handled seperately
	
		/*for (int i=0;i<iSub3x3PatchesNum;i++)
			CreateGridIndices(puiNewIndices,SubPatchSideVertsX,SubPatchSideVertsY,
			(iStepsX)*(iStepsY)*6*i,((SubPatchSideVertsX)*(SubPatchSideVertsY))*i);*/

	//((iTess+1)*(m_iBezierPatchXSize/2)-1)*((iTess+1)*(m_iBezierPatchYSize/2)-1)*6;

	CreateGridIndices(puiNewIndices,SubPatchSideVertsX*aMax,SubPatchSideVertsY*bMax,
					  0,iStartVertexNum );


	return (iStepsX)*(iStepsY)*6*iSub3x3PatchesNum;	
}

void CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert)
{

	int t_DEBUG_BiggestVertNum = 0;

	UINT index=0;
	UINT vertex=xdim;	

	for (int y=1;y<ydim;y++)
	{
		for (int x=0;x<xdim-1;x++)
		{
			IndexArray[startindex+index++]=vertex + startvert;
			if ((vertex+startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+startvert;				
			IndexArray[startindex+index++]=vertex+1 + startvert;
			if ((vertex+1 + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1 + startvert;				
			IndexArray[startindex+index++]=vertex-xdim + startvert;
			if ((vertex-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex-xdim + startvert;				



			IndexArray[startindex+index++]=vertex-xdim + startvert;
			if ((vertex-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex-xdim + startvert;				
			IndexArray[startindex+index++]=vertex+1 + startvert;
			if ((vertex+1 + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1 + startvert;				
			IndexArray[startindex+index++]=vertex+1-xdim + startvert;
			if ((vertex+1-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1-xdim + startvert;				

			vertex++;
		}
		vertex++;
	}
}

CBezierRoute::CBezierRoute()
{
	m_pVertsArray=NULL;
};

CBezierRoute::~CBezierRoute()
{
	SafeDeleteArray(m_pVertsArray);
};



void CBezierRoute::AllocRoute(int iNum)
{
	m_pVertsArray = new CVec3f[iNum];
	m_iVertsNum = iNum;
}

CVec3f CBezierRoute::FindPosInRoute(float fT)
{
//	BREAKPOINT(fT>1.f);

	int iT = (m_iVertsNum-1) /2;
	if (iT*2>m_iVertsNum-1)
		return m_pVertsArray[(int) (m_iVertsNum-1) /2];
	

	iT = floor(fT)*2;
	
	float fSubT = fT - (floor(fT));
	return EvaluateQuadricBezierCurve(fSubT,m_pVertsArray[iT],m_pVertsArray[iT+1],m_pVertsArray[iT+2]);        
}

