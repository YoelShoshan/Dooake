#ifndef CRIMSON_CHARACTER_H
#define CRIMSON_CHARACTER_H

#pragma once

#include "MD5Instance.h"

#include "MD5_Shared.h"

#include "Math3D.h"

#include "Light_IE.h"

#include "RenderModel.h"

#include "ParticleSystem.h"

#include <list>

// use this to get children of a joint

/*
for( i = joint->num + 1; i < num; i++, child++ ) {
// all children of the joint should follow it in the list.
// once we reach a joint without a parent or with a parent
// who is earlier in the list than the specified joint, then
// we've gone through all it's children.
*/

#define MAX_LOOK_UP_DOWN_ANG 88.f

struct tAction
{
	tAction();
	~tAction();

	int  m_iActionAni;
	int  m_iEndLoopAni;
	int* m_piButtons;
};

class CCharacter : public IRenderModel
{
public:
	CCharacter();
	~CCharacter();

	//////////////////////////////////////////////
	// implementations of the IEntity demands
	void Render(int iTech);
	void  Update(float fDeltaTime,bool bVisible);
	void SetPosition(const CVec3f& vPos);
	
	void SetAngles(const CVec3f& vAngles);
	CVec3f GetAngles(void) const { return m_Angles; };

	CVec3f GetCurrentAngles(void) const { return m_vCurrentAngles; };
	CVec3f GetPreviousAngles(void) const { return m_vPreviousAngles; };

	void ClearEffectingLights(void);
	void AddEffectingLight(ILight_Export* pLight);
	//////////////////////////////////////////////

	void BlendAnglesState(float fAlpha) ;
	

	CMD5Inst* GetMD5Inst(void) { return m_MD5Inst;};

	void SetShared(CSmartPointer<CMD5_Shared> spSet,CEntity* pParentEntity);

	

	void PlayAction(int iAnimNum);
	void RequestMoveState(int iMoveState);

	virtual void Die(void);

	/*bool OnlyBase(void);*/

	/*void TriggerAction(int iAction);*/
	


	
		

	
	CVec3f GetPosition(void) const { return m_vPosition; };

		
	

	

	CVec3f GetLookAt(void) const;
	CVec3f GetLeft(void) const;


	// action list, related to the job with arie himan
	bool LoadALFile(const char* pchFileName);

	void SetScale(const CVec3f& vScale) { m_MD5Inst->SetScale(vScale); };

	CVec3f GetCurrentFinalJointPosition(int iJointIndex);

	const char* GetName(void) const { return m_pcName;};
		
	//const unsigned int* GetCurrentTriggeredAnimationsActions(void) const { return m_pCurrentTriggeredActions;};

	int GetRegularWeaponType(void) const { return m_iRegularWeaponType; };
	float GetRegularWeaponSpeed(void) const { return m_fRegularWeaponSpeed; };

private:

	void  ComputeAnimationState(float fDeltaTime,bool bVisible);
	void UpdateRenderablesMatrices(void);


	CMD5Inst* m_MD5Inst;  


	//CVec3f m_Pos;	
	CVec3f m_Angles;
	CVec3f m_vCurrentAngles;
	CVec3f m_vPreviousAngles;
	CVec3f m_LookAt;

	tAction* m_pActions;
	int m_iLastAction;

	

	// weapons

	int m_iRegularWeaponType;
	float m_fRegularWeaponSpeed;


	char* m_pcName;
	

};






#endif