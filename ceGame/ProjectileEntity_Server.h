#ifndef CRIMSON_PROJECTILE_ENTITY_SERVER
#define CRIMSON_PROJECTILE_ENTITY_SERVER

#pragma once

#include "ProjectileEntity.h"

class CProjectileEntity_Server : public CProjectileEntity
{
public:
	CProjectileEntity_Server();
	~CProjectileEntity_Server();

	virtual void UpdateServer(void);

	

	virtual bool TouchedBy(CEntity* pOther);
};

#endif