#ifndef CRIMSON_BASIC_PHYSICS_H
#define CRIMSON_BASIC_PHYSICS_H

#pragma once

#include "PhysicsModel.h"

class CBasicPhysics : public IPhysicsModel
{
public:
	CBasicPhysics();
	~CBasicPhysics();

	
	

	virtual bool IsSlider(void) { return false; };

	virtual void FindPhysicsStepDesiredPosition(float fStep);

	virtual void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep);
	

	virtual void BlendPhysicsState(float fAlpha);
		
	
	virtual bool CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal);

	virtual void ApplyForce(const CVec3f& vForce);
protected:

	bool m_bInAir;

};







#endif