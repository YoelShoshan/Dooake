#ifndef CRIMSON_MD5_INSTANCE_H
#define CRIMSON_MD5_INSTANCE_H

#pragma once

#include "MD5Mesh.h"
#include "MD5Anim.h"
#include "Renderable.h"

#include "Math3d.h"
#include "StaticModel.h"

#include "AnimationController.h"

#include "SmartPointer.h"
#include "Entity.h"


#include "MD5_Shared.h"


#include <vector>

class CAnimController;
class CMD5Inst 
{
public:
	CMD5Inst();
	~CMD5Inst();

	void Render(int iTechNum);


	void SetShared(CSmartPointer<CMD5_Shared> spSet,CEntity* pParentEntity);

	bool GenerateRenderables(DWORD dwGenerate, int iFrame);

	CVec3f GetJointPosition(int iJointIndex);
		
	void  UpdateAni(DWORD dwGenerate, float fDeltaTime,bool bVisible);

	void RequestMoveState(int iMoveState);

	void PlayAction(int iAnimNum);


	/*bool OnlyBase(void); */
	

	int   GetAnimationsNum(void);
	int   GetAnimationFramesNum(int iAnimationNum);
	float GetAnimationFPS(int iAnimationNum);


	void UpdateRenderables(DWORD dwGenerate,const CVec3f &vScale);

	
	void SetMaterialForAll(IMaterial_Export* pMat);
	void SetTransformForAll(float* pfTransf);
	
	void AddLightToAllRends(ILight_Export* pLight);
	void ClearLightsLinkage(void);

	
	void SetSkeletonRender(bool bSet);
	bool GetSkeletonRender(void) const { return m_bRenderSkeleton;};


	int GetJointsNum(void) const {return m_spShared->GetMesh()->GetJointsNum();};


	CVec3f* m_pDebugJointsFKRot;
	CVec3f* m_pCurrentJointsPositions;
	tMD5BindJoint* m_pCurrentBindJoints;


	void SetScale(const CVec3f& vScale) { m_vScale=vScale;};
	

	void SetLookUpAngle(float fLookUpAngle) { m_fLookUpDownAngle = fLookUpAngle; };

	float GetLookUpAngle(void) const { return m_fLookUpDownAngle;};

	bool GetSharedFlipUpDown(void) const { return m_spShared->GetFlipUpDown();};


	bool GetShareIsLoopable_Attack1(void) const { return m_spShared->GetIsAutoFire_Attack1();};
	bool GetShareIsLoopable_Attack2(void) const { return m_spShared->GetIsAutoFire_Attack2();};

	int GetSharedAnimationsNum(void) const{ return m_spShared->GetAnimsNum();};

	virtual void Die();

private:

	void RenderJoints(void);
	
	CRenderable* m_AnimatedRends;
	IVertexBuffer_Export** m_ppAnimatedVB;
	
	CSmartPointer<CMD5_Shared> m_spShared;

	CAnimController* m_pAnimController;

	CVec3f** m_ppVerts;

	CVec3f* m_pTempTangentsArray;
	CVec3f* m_pTempBinormalsArray;
	CVec3f* m_pTempNormalsArray;

	// debug info

	bool m_bFKDebug;
	bool m_bRenderSkeleton;
	bool m_bSkeletonMeshesInitied;
	
	CStaticModel** m_ppJointModel;
	CStaticModel* m_pSELECTEDJointModel;

	CVec3f m_vScale;

	float m_fLookUpDownAngle; // used for aiming up/ down


	 
};


#endif