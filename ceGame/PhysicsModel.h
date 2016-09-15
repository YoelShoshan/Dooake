#ifndef CRIMSON_PHYSICS_MODEL_H
#define CRIMSON_PHYSICS_MODEL_H

#pragma once

#include "Math3D.h"

#include "Entity.h"
//#define VELOCITY_MINIMUM_LENGTH 0.000001f
#define VELOCITY_MINIMUM_LENGTH 0.01f

class CEntity;
class IPhysicsModel
{
public:
	IPhysicsModel():m_vPos(0.f,0.f,0.f),
		m_vPreviousTimeStepPos(0.f,0.f,0.f),
		m_vCurrentTimeStepPos(0.f,0.f,0.f),
		m_vActiveForces(0.f,0.f,0.f),
		m_vVelocity(0.f,0.f,0.f),
		/*m_vAcceleration(0.f,0.f,0.f),*/
		m_bActive(true),
		m_fDistanceToFloor(0.f),
		m_pParentEntity(NULL){ m_BBox.min.Set(-10.f,-10.f,-10.f);m_BBox.max.Set(10.f,10.f,10.f);};
	virtual ~IPhysicsModel() {};


	virtual bool IsSlider(void) { return false; };

	virtual bool IsTriggerOnly(void) { return false; };
	

	virtual bool IsActive(void) { return m_bActive; };
	virtual void SetActive(bool bActive) { m_bActive = bActive;};
	

	virtual bool IsLevelModel(void) const { return false; };

	virtual void FindPhysicsStepDesiredPosition(float fStep) = 0;
	virtual void BlendPhysicsState(float fAlpha) = 0;
	virtual void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep) = 0;
		
	virtual bool CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal ) = 0;

	virtual void ApplyForce(const CVec3f& vForce) = 0;

	//void AdvanceTimeStep(float fStepSize);

	CVec3f GetCurrentTimeStepPos(void) const { return m_vCurrentTimeStepPos;};

	void SetCurrentTimeStepPos(const CVec3f& vCurrentTimeStepPos);
	

	

	CVec3f GetPreviousTimeStepPos(void) const {return m_vPreviousTimeStepPos;};

	void SetPreviousTimeStepPos(const CVec3f& vPreviousTimeStepPos)
	{
		m_vPreviousTimeStepPos = vPreviousTimeStepPos;
	}


	void SetPosition(const CVec3f& vPos)
	{
		/*m_vPos = vPos;
		m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;
		m_vCurrentTimeStepPos = vPos;
		//m_vDesiredPos = vPos;*/

		m_vPos = vPos;
		m_vPreviousTimeStepPos = vPos;
		m_vCurrentTimeStepPos = vPos;
		m_vDesiredPos = vPos;
	}
	
	CVec3f GetPosition(void) { return m_vPos;};
 
	CVec3f GetDesiredPos(void) { return m_vDesiredPos;};
	void   SetDesiredPos(const CVec3f& vDesiredPos) { m_vDesiredPos = vDesiredPos;};

	CVec3f m_vVelocity;
	//CVec3f m_vTempVelocity;
	//CVec3f m_vAcceleration;

	BBox m_BBox;

	
	//void     SetParent(CEntity* pParent) { m_pParent = pParent; };
	
	float GetDistanceToFloor(void) { return m_fDistanceToFloor; };
	

	void SetParentEntity(CEntity* pEntity) { m_pParentEntity = pEntity; };
	CEntity* GetParentEntity(void);
	
	

protected:

	CVec3f m_vActiveForces;

	CVec3f m_vPreviousTimeStepPos;
	CVec3f m_vCurrentTimeStepPos;
	CVec3f m_vPos;
	CVec3f m_vDesiredPos;

	bool m_bActive;

	float m_fDistanceToFloor;

	CEntity* m_pParentEntity;
	
};







#endif