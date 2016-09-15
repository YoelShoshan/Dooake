#ifndef CRIMSON_SOUND_H
#define CRIMSON_SOUND_H

//#include "SoundAPI.h"

#include "ReferenceCount.h"

#include "SoundManager.h"

class CSoundManager;
class CSound : public CReferenceCount
{

	friend class CSoundManager;

public:
	
	~CSound();
	
	const FSOUND_SAMPLE* GetSample(void) const { return m_pSample; };

	bool Play(const CVec3f& vPos,int iVolume);

	bool Play(int iVolume);

protected:
	CSound(const char* pcName)
	{
		m_pcName = new char[strlen(pcName)+1];
		strcpy(m_pcName,pcName);
		m_pSample=NULL;
	};

	FSOUND_SAMPLE* m_pSample;

private:
	char* m_pcName;
	
};




#endif