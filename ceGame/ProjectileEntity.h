#ifndef CRIMSON_PROJECTILE_ENTITY
#define CRIMSON_PROJECTILE_ENTITY

#pragma once

/*#define PROJECTILE_TYPE_NO_INIT  0
#define PROJECTILE_TYPE_ROCKET   1
#define PROJECTILE_TYPE_FIREBALL 2*/



//#define PROJECTILE_TYPE_ROCKET_SPEED   750.f
//#define PROJECTILE_TYPE_FIREBALL_SPEED 1000.f

#include "Entity.h"

class CProjectileEntity : public CEntity
{
public:

	virtual bool RelinkMe(void) { return true; };

	virtual bool IsProjectile(void) { return true;} ;

	CProjectileEntity();
	~CProjectileEntity();


	virtual CVec3f GetCurrentBindPosition(int iBindNum) const;

	virtual void Init(DWORD dwType,const char* pcModel, const char* pcMaterial,const CVec3f& vOrigin, const CVec3f& vVelocity, DWORD dwShotByEntityID);

	virtual void UpdateServer(void);

	virtual bool TouchedBy(CEntity* pOther);
protected:
	DWORD m_dwProjectileType;

	DWORD m_dwShotByEntityID;
};






#endif