#include "stdafx.h"
#include "ParticleSystemManager.h"

#include "ce.h"

class CParticleSystem_Shared;
CSmartPointer<CParticleSystem_Shared> CParticleSystemManager::Load(const char *FileName)
{
	//	assert(_CrtCheckMemory());
	CParticleSystem_Shared* pAlreadyInMan= AlreadyInManager(FileName);


	

	//debug
	/*if (!m_pDbg1 && pAlreadyInMan && strcmp(FileName,"ParticleSystems/explotion.ps")==0)
	{
		m_pDbg1 = pAlreadyInMan;
	}

	if (!m_pDbg2 && pAlreadyInMan && strcmp(FileName,"ParticleSystems/monsters/mancubus_smoke.ps")==0)
	{
		m_pDbg2 = pAlreadyInMan;
	}*/

	if (!pAlreadyInMan)
	{
		CParticleSystem_Shared* pSharedPS;
		pSharedPS = new CParticleSystem_Shared();
		
		if (pSharedPS->Load(FileName))  // load was successfull
		{
			// hacked "touching"
			// add another one in the first time
			pSharedPS->Inc();

			m_ResourcesMap[std::string(FileName)] = pSharedPS; // add to map

			m_iResourcesNum++;

			// debug
			BREAKPOINT(!pSharedPS->GetParticlesMax());
			BREAKPOINT(pSharedPS->GetParticlesMax()>10000);

			return pSharedPS;
		}
		else  // load FAILED
		{
			CParticleSystem_Shared* pErrorImageInMan = AlreadyInManager("ceData/default.ps");
			if (!pErrorImageInMan)	// wasn't found		
			{
				pSharedPS->Load("ceData/default.ps");
				m_ResourcesMap[std::string("ceData/default.ps")] = pSharedPS; // add to map
				m_iResourcesNum++;

				// debug
				BREAKPOINT(!pSharedPS->GetParticlesMax());
				BREAKPOINT(pSharedPS->GetParticlesMax()>10000);

				return pSharedPS;
			}
			else
			{
				SafeDelete(pSharedPS);

				// debug
				BREAKPOINT(!pErrorImageInMan->GetParticlesMax());
				BREAKPOINT(pErrorImageInMan->GetParticlesMax()>10000);

				return pErrorImageInMan;
			}
		}
	}
	else
	{
		// debug
		BREAKPOINT(!pAlreadyInMan->GetParticlesMax());
		BREAKPOINT(pAlreadyInMan->GetParticlesMax()>10000);
		return pAlreadyInMan;
	}

}