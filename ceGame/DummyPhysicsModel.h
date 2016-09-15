#ifndef CRIMSON_DUMMY_PHYSICS_MODEL_H
#define CRIMSON_DUMMY_PHYSICS_MODEL_H

#pragma once

#include "PhysicsModel.h"

class CDummyPhysicsModel : public IPhysicsModel
{
public:
	CDummyPhysicsModel();
	~CDummyPhysicsModel();

	void FindPhysicsStepDesiredPosition(float fStep) {};
	void BlendPhysicsState(float fAlpha) {};
	void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep) {};
		
	bool CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal ) {return false;};

	void ApplyForce(const CVec3f& vForce) {};

};



#endif
