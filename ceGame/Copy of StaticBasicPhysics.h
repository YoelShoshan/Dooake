#ifndef CRIMSON_STATIC_BASIC_PHYSICS
#define CRIMSON_STATIC_BASIC_PHYSICS

#pragma once


#include "PhysicsModel.h"

class CBasicPhysics_Static : public IPhysicsModel
{
public:
	CBasicPhysics_Static();
	~CBasicPhysics_Static();

	virtual bool RelinkMe(void) { return false; };

	bool IsTriggerOnly(void) { return true; };

	void CBasicPhysics_Static::FindPhysicsStepDesiredPosition(float fStep) {};
	void BlendPhysicsState(float fAlpha) {};
	void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep) {};

	bool CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal );

	void ApplyForce(const CVec3f& vForce){};


};



#endif