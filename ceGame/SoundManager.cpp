#include "stdafx.h"
#include "SoundManager.h"

#include "ce.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;
#include <crtdbg.h>
#include "fmod.h"


/*struct FSOUND_SAMPLE;
void CSoundManager::PlaySound(const char* FileName,const CVec3f& vPos,int iVolume)
{
	assert(_CrtCheckMemory());
	CSmartPointer<CSound>  sp = Load(FileName);

	FSOUND_SAMPLE* pSample = ((FSOUND_SAMPLE*)sp->GetSample());

	g_SoundAPI.PlaySample3D( pSample,vPos,iVolume);
	assert(_CrtCheckMemory());
}*/

class CSound;
CSmartPointer<CSound> CSoundManager::Load(const char *FileName)
{
	if (m_spErrorSound == NULL)
	{
		CSound* pErrorSound = new CSound("ceData/default.ogg");
		g_SoundAPI.LoadSample("ceData/default.ogg",pErrorSound->m_pSample);

		if (!pErrorSound)
		{
			g_pLogFile->OutPutPlainText("CSoundManager::Load Can't find ceData/default.ogg",LOG_MESSAGE_ERROR);			
		}

		m_spErrorSound = pErrorSound;	
	}

	assert(_CrtCheckMemory());
	CSound* pAlreadyInMan= AlreadyInManager(FileName);

	if (!pAlreadyInMan)
	{
		assert(_CrtCheckMemory());
		CSound* pSharedSound;
		pSharedSound = new CSound(FileName);
		
		// hacked TOUCHING
		pSharedSound->Inc();
		
		if (g_SoundAPI.LoadSample(FileName,pSharedSound->m_pSample))  // load was successfull
		{
			m_ResourcesMap[std::string(FileName)] = pSharedSound; // add to map

			m_iResourcesNum++;
			

			assert(_CrtCheckMemory());
			return pSharedSound;
		}
		else  // load FAILED
		{
			assert(_CrtCheckMemory());
			pSharedSound->Dec();

			if (m_spErrorSound == NULL)
			{
				g_pLogFile->OutPutPlainText("CSoundManager::Load Can't find ceData/default.ogg",LOG_MESSAGE_ERROR);		
			}

			return m_spErrorSound;

		}
	}
	else
	{
		assert(_CrtCheckMemory());
		return pAlreadyInMan;
	}

}