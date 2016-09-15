#ifndef CRIMSON_PARTICLE_SYSTEM_MANAGER_H
#define CRIMSON_PARTICLE_SYSTEM_MANAGER_H

#pragma once

#define st_ParticleSystemManager CParticleSystemManager::Inst()

#include "ParticleSystem_Shared.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"


class CParticleSystemManager : public CManager<CParticleSystem_Shared>,public CSingleton<CParticleSystemManager>	
{
public:

	CParticleSystemManager() {m_pDbg1=NULL;m_pDbg2=NULL;};
	~CParticleSystemManager() {};
	
	CSmartPointer<CParticleSystem_Shared> Load(const char *FileName);

private:
	// debug
	CParticleSystem_Shared* m_pDbg1;
	CParticleSystem_Shared* m_pDbg2;
};





#endif