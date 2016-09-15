#ifndef CRIMSON_PARTICLE_SYSTEM_ENTITY_H
#define CRIMSON_PARTICLE_SYSTEM_ENTITY_H

#pragma once

#include "StaticEntity.h"


class CParticleSystemEntity : public CEntity
{
public:
	CParticleSystemEntity();

	//CParticleSystemEntity(CEntity* pParentEntity);

	~CParticleSystemEntity();

	virtual bool IgnoreMeInPhysics(void) { return true; };

	virtual void UpdateRenderModel(float fDeltaTime,bool bVisible);

	void SetParentEntity(CEntity* pParentEntity) { m_pParentEntity = pParentEntity;};


private:

	CEntity* m_pParentEntity;

};


#endif