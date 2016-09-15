#ifndef CRIMSON_SOUND_MANAGER_H
#define CRIMSON_SOUND_MANAGER_H

#pragma once

#define st_SoundManager CSoundManager::Inst()

#include "SoundAPI.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"
#include "Sound.h"

#include "Math3d.h"

//////////////////////////////////////////////////////////////////////////
//just for reference:
//sound/feedback/
//takenlead.wav
//lostlead.wav
//tiedlead.wav
//accuracy.wav
//////////////////////////////////////////////////////////////////////////


class CSound;
class CSoundManager : public CManager<CSound>,public CSingleton<CSoundManager>	
{
public:

	CSoundManager() {};
	~CSoundManager() {};
	
	CSmartPointer<CSound> Load(const char *FileName);

	//void PlaySound(const char* FileName,const CVec3f& vPos,int iVolume);
private:
	CSmartPointer<CSound> m_spErrorSound;
};





#endif