#include "stdafx.h"
#include "BSP_PhysicsModel.h"
#include "Input.h"


#include "SurfaceFlags.h"

CBSP_PhysicsModel::CBSP_PhysicsModel()
{
   m_iPlanesNum = 0;
   m_pPlanes = NULL;
	
	m_iLeafBrushesNum = 0;
	m_pLeafBrushes = NULL;
    
	m_iBrushesNum = 0;
	m_pBrushes = NULL;

	m_BrushSidesNum = 0;
	m_pBrushSides = NULL;

	m_iNodesNum = 0;
	m_pNodes = NULL;

	m_iLeafsNum = 0;
	m_pLeafs = NULL;

}

CBSP_PhysicsModel::~CBSP_PhysicsModel()
{

}


bool CBSP_PhysicsModel::TraceMove_Brush(tBSPBrush *brush,tTrace* pTrace)
{


	bool bDidCollide=false;

	float fStartFraction = -1.f;
	float fEndFraction   = 1.f;
	bool bStartsOut = false;
	bool bEndsOut = false;



	int iSavedBrushSideNum = 0;

	CVec3f vCandidateToHitNormal;

	for (int i=0;i<brush->numOfBrushSides;i++)
	{
		CPlane     *plane;

		
/*		if (brush->m_pPlanes) // a dynamic entity brush
		{
			plane     = &(brush->m_pPlanes[i]);
		}
		else // a map brush*/

		BREAKPOINT(brush->m_pPlanes);
		
		tBSPBrushORIGINALSide *brushSide = &m_pBrushSides[brush->brushSide + i];						

		/*if (!(m_pTextures[brushSide->textureID].contents & (CONTENTS_SOLID | CONTENTS_PLAYERCLIP)))
			continue;*/

		plane     = &m_pPlanes[brushSide->plane];
		

		float fOffset = fabs(pTrace->vBBoxRadius.x*plane->n.x)
						+ fabs(pTrace->vBBoxRadius.y*plane->n.y)
						+ fabs(pTrace->vBBoxRadius.z*plane->n.z);

		
		float fStartDistance,fEndDistance;

		CVec3f vPlaneNormal = plane->n;


		// ORIGINAL
		fStartDistance = (vPlaneNormal*pTrace->vRayStart) - (plane->d + fOffset);
		fEndDistance   = (vPlaneNormal*pTrace->vRayEnd)   - (plane->d + fOffset);
				
        

		if (fStartDistance>0.f)
			bStartsOut=true;
		if (fEndDistance>0.f)
			bEndsOut = true;


		// both are INFRONT of the plane, so it outside
		if (fStartDistance > 0 && fEndDistance > 0)
			return false;

		// both are inside, giving a chance to a different plane 
		if (fStartDistance<=0 && fEndDistance<=0)
			continue;

		if (fStartDistance>fEndDistance) // line is entering the brush
		{
			float fraction = (fStartDistance - EPSILON) / (fStartDistance - fEndDistance);

			// entering the brush
			if (fraction > fStartFraction)
			{
				fStartFraction = fraction;
				iSavedBrushSideNum = i;
				pTrace->CollisionPlane = (*plane);
			}

	
		}
		else // line is leaving the brush
		{
			float fraction = (fStartDistance + EPSILON) / (fStartDistance - fEndDistance);
			if (fraction < fEndFraction)
			{
				fEndFraction = fraction;
			}

		}

	}



	if (bStartsOut == false)
	{
		//pTrace->bStartOut = false;
		
		if (bEndsOut == false)
			pTrace->bAllSolid = true;
						
		return false; //fishy
	}
	
	

	if (fStartFraction < fEndFraction)
	{
		if (fStartFraction > -1 && fStartFraction < pTrace->fFraction)
		{
			

			if (fStartFraction < 0)
				fStartFraction = 0;

			pTrace->fFraction = fStartFraction;

			CPlane *POTplane;
			tBSPBrushORIGINALSide *POTbrushSide = &m_pBrushSides[brush->brushSide + iSavedBrushSideNum];
			POTplane = &m_pPlanes[POTbrushSide->plane];
	

			pTrace->CollisionPlane = (*POTplane);			

			bDidCollide = true;
		}
	}

	return bDidCollide;

}

void CBSP_PhysicsModel::TraceMove(struct tTrace* pTrace)
{
	TraceMove_Node(0,0.f,1.f,pTrace->vRayStart,pTrace->vRayEnd,pTrace,0);


}

void CBSP_PhysicsModel::TraceMove_Node(int iNodeIndex, float fStartFraction, float fEndFraction, CVec3f vStart, CVec3f vEnd,tTrace* pTrace,int iRecurseDepth)
{
	
	//TODO: think about this one !
	/*if (pTrace->fFraction < fStartFraction)
		return;*/

	

	/*if (iRecurseDepth>20)
		return;*/

	iRecurseDepth++;

	if (iNodeIndex < 0) // a leaf
	{

		int iLeafNum = -(iNodeIndex+1);
		tBSPLeaf* leaf = &m_pLeafs[iLeafNum];

		tBSPBrush* brush = NULL;

		for (int i=0; i < leaf->numOfLeafBrushes ; i++)
			{
					brush = &m_pBrushes[m_pLeafBrushes[leaf->leafBrush + i]];
					
					if (!(m_pTextures[brush->textureID].contents & (CONTENTS_SOLID | CONTENTS_PLAYERCLIP)))
						continue;

					BREAKPOINT(brush->numOfBrushSides==0);

					TraceMove_Brush(brush,pTrace);

					// if we couldn't move at all, no need to search anymore
					if (pTrace->fFraction == 0.f)
						return;
			}

		// and now the generate bezier brushes (now a place holder - bbox of the bezier patch used as the brush)
		/*if(st_Input->KeyPressed(DIK_P))
		{
			if (vEnd.y > vStart.y)
			{
				int a=0;
				a++;
			}
			
		}*/

		for (int i=0; i < leaf->numOfBezierLeafBrushes ; i++)
		{
			brush = &m_pBrushes[m_pLeafBrushes[leaf->leafBezierBrush + i]];

			if (!(m_pTextures[brush->textureID].contents & (CONTENTS_SOLID | CONTENTS_PLAYERCLIP)))
				continue;

			BREAKPOINT(brush->numOfBrushSides==0);

			TraceMove_Brush(brush,pTrace);

			// if we couldn't move at all, no need to search anymore
			if (pTrace->fFraction == 0.f)
				return;
		}
		

		return;
	}


	// if we're here, then it's a node.

	tBSPNode  *node  = &m_pNodes[iNodeIndex];


	


	CPlane *plane = &m_pPlanes[node->plane];

	// distance from the start and end point to the plane
	CVec3f vPlaneNormal = plane->n;


	float fStartDistance = (vPlaneNormal * vStart) - plane->d;
	float fEndDistance   = (vPlaneNormal * vEnd)   - plane->d;

	float fOffset = fabs(pTrace->vBBoxRadius.x*plane->n.x)
		+ fabs(pTrace->vBBoxRadius.y*plane->n.y)
		+ fabs(pTrace->vBBoxRadius.z*plane->n.z);

	//float fOffset = 70.f;
    
	
	if (fStartDistance >= fOffset && fEndDistance >=fOffset) // both are infront
		TraceMove_Node(node->front, fStartFraction, fEndFraction, vStart, vEnd,pTrace,iRecurseDepth);
	else
	if (fStartDistance < -fOffset && fEndDistance < -fOffset)  // both are behind
		TraceMove_Node(node->back,fStartFraction,fEndFraction,vStart,vEnd,pTrace,iRecurseDepth);
	else // moving from one side to another
	{
		int side;
		float fraction1,fraction2,middleFraction;
		CVec3f vMiddle;

		if (fStartDistance < fEndDistance)
		{
			side = 1; // back
			float inverseDistance = 1.f / (fStartDistance - fEndDistance);
			fraction1 = (fStartDistance - fOffset - EPSILON) * inverseDistance;
			fraction2 = (fStartDistance + fOffset + EPSILON) * inverseDistance;
		}
		else
		if (fEndDistance < fStartDistance)
		{
			side = 0; // front
			float inverseDistance = 1.f/(fStartDistance - fEndDistance);
			fraction1 = (fStartDistance + fOffset + EPSILON) * inverseDistance;
			fraction2 = (fStartDistance - fOffset - EPSILON) * inverseDistance;
		}
		else
		{
			side = 0; //front
			fraction1 = 1.f;
			fraction2 = 0.f;
		}


		// step 2
		if (fraction1 < 0.f) 
			fraction1 = 0.f;
		else if (fraction1 > 1.f) 
			fraction1 = 1.f;

		if (fraction2 < 0.f) 
			fraction2 = 0.f;
		else if (fraction2 > 1.f) 
			fraction2 = 1.f;



		// step 3
		middleFraction = fStartDistance + (fEndDistance - fStartFraction) * fraction1; // original
		//middleFraction = fStartDistance + (fEndDistance - fStartDistance) * fraction1;
		vMiddle = vStart + ((vEnd-vStart)*fraction1);

		// step 4
		if (side==1) // back
			TraceMove_Node(node->back,fStartFraction,middleFraction,vStart,vMiddle,pTrace,iRecurseDepth);
		else  // front
			TraceMove_Node(node->front,fStartFraction,middleFraction,vStart,vMiddle,pTrace,iRecurseDepth);

		// step 5

		middleFraction = fStartFraction + (fEndFraction - fStartFraction)*fraction2;
		vMiddle = vStart + ((vEnd-vStart)*fraction2);

		// step 6

		if (side==0) // back-->front
			TraceMove_Node(node->back,middleFraction,fEndFraction,vMiddle,vEnd,pTrace,iRecurseDepth);
		else  // front-->back
			TraceMove_Node(node->front,middleFraction,fEndFraction,vMiddle,vEnd,pTrace,iRecurseDepth);

	}

	/*int Q3_FRONT = 0;
	int Q3_BACK = 1;

	int iPlane=m_pNodes[iNodeIndex].plane;
	float t1 = Dot(m_pPlanes[iPlane].n,vStart) - m_pPlanes[iPlane].d;
	float t2 = Dot(m_pPlanes[iPlane].n,vEnd) - m_pPlanes[iPlane].d;


	//float offset = fabs(pTrace->vBBoxRadius.x*m_pPlanes[iPlane].n.x)
	//	+ fabs(pTrace->vBBoxRadius.y*m_pPlanes[iPlane].n.y)
	//	+ fabs(pTrace->vBBoxRadius.z*m_pPlanes[iPlane].n.z);

	float offset = 15.f;
		
	// The whole volume is in front of the plane
	if ((t1 >= offset) && (t2 >= offset))
	{
		TraceMove_Node(m_pNodes[iNodeIndex].front,fStartFraction, fEndFraction, vStart, vEnd, pTrace,iRecurseDepth);
		return;
	}
	
	// The whole volume is in the back of the plane
	else if ((t1 < -offset) && (t2 < -offset))
	{
		TraceMove_Node(m_pNodes[iNodeIndex].back,fStartFraction, fEndFraction, vStart, vEnd, pTrace,iRecurseDepth);
		return;
	}

	


	// Else, the box is split. Check the two halves.
	int side;
	float idist, frac, frac2, midf;
	CVec3f midp;
	float DIST_EPSILON=0.03125;


	if (t1 < t2)
	{
		side  = Q3_BACK;
		idist = 1.0f / (t1 - t2);
		frac  = (t1 - offset - DIST_EPSILON) * idist;
		frac2 = (t1 + offset + DIST_EPSILON) * idist;
	}
	else if (t1 > t2)
	{
		side  = Q3_FRONT;
		idist = 1.0f / (t1 - t2);
		frac  = (t1 + offset + DIST_EPSILON) * idist;
		frac2 = (t1 - offset - DIST_EPSILON) * idist;
	}
	else
	{
		side  = Q3_FRONT;
		frac  = 1.0f;
		frac2 = 0.0f;
	}
	// Clamp
	if (frac  < 0.0f) frac  = 0.0f;
	if (frac  > 1.0f) frac  = 1.0f;
	if (frac2 < 0.0f) frac2 = 0.0f;
	if (frac2 > 1.0f) frac2 = 1.0f;


	// Move from start to the plane
	midf = fStartFraction + (fEndFraction - fStartFraction)*frac;
	midp = vStart + (vEnd - vStart)*frac;

	if (side==Q3_FRONT)
		//checkMoveNode(fSf, midf, vSp, midp, m_pNodes[iNode].iFront,moveMove);
		TraceMove_Node(m_pNodes[iNodeIndex].front,fStartFraction, midf, vStart, vEnd, pTrace,iRecurseDepth);
	else
		//checkMoveNode(fSf, midf, vSp, midp, m_pNodes[iNode].iBack,moveMove);
		TraceMove_Node(m_pNodes[iNodeIndex].back,fStartFraction, midf, vStart, midp, pTrace,iRecurseDepth);


	// Move from the plane to the end
	midf = fStartFraction + (fEndFraction - fStartFraction)*frac2;
	midp = vStart + (vEnd - vStart)*frac2;

	if (side==Q3_FRONT)
		//checkMoveNode(midf,fEf, midp, vEp, m_pNodes[iNode].iBack,moveMove);
		TraceMove_Node(m_pNodes[iNodeIndex].back,midf, fEndFraction, midp, vEnd, pTrace,iRecurseDepth);
	else
		//checkMoveNode(midf,fEf, midp, vEp, m_pNodes[iNode].iFront,moveMove);
		TraceMove_Node(m_pNodes[iNodeIndex].front,midf, fEndFraction, midp, vEnd, pTrace,iRecurseDepth);

		*/

}


bool CBSP_PhysicsModel::CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal)
{
	
	/*CVec3f vPosition = pOtherPM->GetPosition();
	CVec3f vMove = pOtherPM->GetDesiredPos() - pOtherPM->GetPosition();*/
	/*CVec3f vPos  = vStartPos;
	CVec3f vMove = vEndPos - vStartPos;*/



	////////////////////////
	// DEBUG DEBUG
	/*fMoveTillHit = 1.f;
	return;*/
	////////////////////////


	tTrace trace;

	// debug debug debug
	/*CVec3f vStart = pOtherPM->GetPreviousTimeStepPos();
    CVec3f vEnd   = pOtherPM->GetDesiredPos();*/

	CVec3f vStart = vStartPos;
    CVec3f vEnd   = vEndPos;

	trace.vRayStart = vStart;
	trace.vRayEnd   = vEnd;

	trace.vBBoxRadius.x = max(abs(pOtherPM->m_BBox.min.x),abs(pOtherPM->m_BBox.max.x));
	trace.vBBoxRadius.y = max(abs(pOtherPM->m_BBox.min.y),abs(pOtherPM->m_BBox.max.y));
	trace.vBBoxRadius.z = max(abs(pOtherPM->m_BBox.min.z),abs(pOtherPM->m_BBox.max.z));


	//TODO: check if these are correct init values
	//trace.bStartOut = true;
	trace.bAllSolid = false;
	trace.fFraction = 1.f;

	TraceMove_Node(0,0.f,1.f,vStart,vEnd,&trace,0);

	
	fMoveTillHit = trace.fFraction;
	vHitNormal   = trace.CollisionPlane.n;

	

	if (fMoveTillHit<1.f)
	{
		int k=123;
		k++;
	}

	
	return false;
	
}

void CBSP_PhysicsModel::SetPlanes(int iPlanesNum, CPlane* pPlanes)
	{
		m_iPlanesNum = iPlanesNum;
		m_pPlanes    = pPlanes;
	};

	void CBSP_PhysicsModel::SetBrushes(int iBrushesNum, tBSPBrush* pBrushes)
	{
		m_iBrushesNum = iBrushesNum;
		m_pBrushes    = pBrushes;
	};

	
	void CBSP_PhysicsModel::SetBrushesSides(int iBrushesSidesNum, tBSPBrushORIGINALSide* pBrushSides)
	{
		m_BrushSidesNum = iBrushesSidesNum;
		m_pBrushSides   = pBrushSides;
	};

	void CBSP_PhysicsModel::SetLeafBrushes(int iLeafBrushesNum, int* pLeafBrushes)
	{
		m_iLeafBrushesNum = iLeafBrushesNum;
		m_pLeafBrushes     = pLeafBrushes;
	};

		
	void CBSP_PhysicsModel::SetNodes(int iNodesNum, tBSPNode*  pNodes)
	{
		m_iNodesNum = iNodesNum;
		m_pNodes    = pNodes;
	}

	void CBSP_PhysicsModel::SetLeafs(int iLeafsNum, tBSPLeaf* pLeafs)
	{
		m_iLeafsNum = iLeafsNum;
		m_pLeafs    = pLeafs;
	}

	void CBSP_PhysicsModel::SetTextures(int iTexturesNum, tBSPTexture* pTextures)
	{
		m_iTexturesNum = iTexturesNum;
		m_pTextures    = pTextures;
	}

