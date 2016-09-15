#ifndef CRIMSON_BSP_PHYSICS_MODEL
#define CRIMSON_BSP_PHYSICS_MODEL

#pragma once

#include "PhysicsModel.h"

#include "Q3MapDefenitions.h"


struct tTrace
{
	// inputs
	CVec3f vRayStart;
	CVec3f vRayEnd;
	CVec3f vBBoxRadius;

	// outputs
	float fFraction;
	CPlane CollisionPlane;
	bool bAllSolid;
	//bool bStartsOut
};


class CBSP_PhysicsModel : public IPhysicsModel
{


public:
	CBSP_PhysicsModel();
	~CBSP_PhysicsModel();

	virtual bool IsLevelModel(void) const { return true; };

	
	void Update(float fDeltaTime)
	{
		// dummy - base level geometry is static
	}

	void ApplyForce(const CVec3f& vForce)
	{
		// dummy - applying force on the level shouldn't have any effect
	}

	void FindPhysicsStepDesiredPosition(float fStep)
	{
		// dummy...
	}

	void BlendPhysicsState(float fAlpha)
	{
		// dummy...
	}

	void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep)
	{
		 // dummy
	}

	


	bool TraceMove_Brush(tBSPBrush *brush,struct tTrace* pTrace);
	
	void TraceMove_Node(int iNodeIndex, float fStartFraction, float fEndFraction, CVec3f vStart, CVec3f vEnd,struct tTrace* pTrace,int iRecurseDepth);
	
	void TraceMove(struct tTrace* pTrace);
	

	bool CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal);

	void SetPlanes(int iPlanesNum, CPlane* pPlanes);


	void SetBrushes(int iBrushesNum, tBSPBrush* pBrushes);


	
	void SetBrushesSides(int iBrushesSidesNum, tBSPBrushORIGINALSide* pBrushSides);


	void SetLeafBrushes(int iLeafBrushesNum, int* pLeafBrushes);


		
	void SetNodes(int iNodesNum, tBSPNode*  pNodes);


	void SetLeafs(int iLeafsNum, tBSPLeaf* pLeafs);


	void SetTextures(int iTexturesNum, tBSPTexture* pTextures);


private:

	int          m_iTexturesNum;
	tBSPTexture* m_pTextures;

	int      m_iPlanesNum;          // number of planes
	CPlane   *m_pPlanes;            // planes array
	
	int       m_iLeafBrushesNum;     // number of leaf brushes
	int*      m_pLeafBrushes;        // leaf brushes indices array
    
	int       m_iBrushesNum;         // number of brushes
	tBSPBrush *m_pBrushes;           // brushes array

    int        m_BrushSidesNum;            // number of brush sides
	tBSPBrushORIGINALSide *m_pBrushSides;  // brush sides array

	int        m_iNodesNum;      // number of nodes in the BSP tree
	tBSPNode*  m_pNodes;         // nodes array  


	int         m_iLeafsNum;      // number of leafs
	tBSPLeaf*   m_pLeafs;         // leafs array

};


#endif