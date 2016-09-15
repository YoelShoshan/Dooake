#ifndef CRIMSON_PROJECTILE_PHYSICS_H
#define CRIMSON_PROJECTILE_PHYSICS_H

#pragma once

#include "BasicPhysics.h"


class CProjectilePhysics : public CBasicPhysics
{
public:
	CProjectilePhysics() {};
	~CProjectilePhysics() {};


	virtual void ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep);

	

};


#endif