#include "stdafx.h"
#include "Sound.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;
#include <crtdbg.h>
#include "fmod.h"

class CSoundManager;
CSound::~CSound()
{
	st_SoundManager->Unregister(m_pcName);

}

bool CSound::Play(const CVec3f& vPos,int iVolume)
{
	//debug
	//return true;

	assert(_CrtCheckMemory());
	//CSmartPointer<CSound> /*CSound**/ sp = Load(FileName);

	//FSOUND_SAMPLE* pSample = ((FSOUND_SAMPLE*)sp->GetSample());

	if (m_pSample)
	{
		g_SoundAPI.PlaySample3D( m_pSample,vPos,iVolume);
	} else
		return false;
	assert(_CrtCheckMemory());


	return true;
}

bool CSound::Play(int iVolume)
{
	assert(_CrtCheckMemory());
	//CSmartPointer<CSound> /*CSound**/ sp = Load(FileName);

	//FSOUND_SAMPLE* pSample = ((FSOUND_SAMPLE*)sp->GetSample());

	if (m_pSample)
	{
		g_SoundAPI.PlaySample( m_pSample,iVolume);
	} else
		return false;
	assert(_CrtCheckMemory());


	return true;
}