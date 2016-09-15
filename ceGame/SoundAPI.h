#ifndef CRIMSON_SOUND_API_H
#define CRIMSON_SOUND_API_H

#pragma once

#include <windows.h>
#include "fmod.h"
#include "Math3d.h"

#define SM_MAX_CHANNELS 32


class CSoundAPI
{
public:
	CSoundAPI();
	~CSoundAPI();

	// init fmod
	bool Init(void);

	// play a sound sample
	bool LoadPlaySound(char* pcFileName, FSOUND_SAMPLE*& pSound);

	// play looped music
	bool PlayLoopedMusic(char* pcFileName, FSOUND_STREAM*& pStream, int* channel);



	bool LoadStream(char* pcFileName, FSOUND_STREAM*& pStream);
	bool FreeStream(FSOUND_STREAM*& pStream);
	bool StopStream(FSOUND_STREAM*& pStream);
	bool PlayStream(FSOUND_STREAM*& pStream);
	bool PlayStream(FSOUND_STREAM*& pStream,int iVolume);



	bool LoadSample(const char* pcFileName,FSOUND_SAMPLE*& pSound);	
	bool FreeSound(FSOUND_SAMPLE*& pSound);
	bool PlaySample(FSOUND_SAMPLE*& pSound);
	bool PlaySample(FSOUND_SAMPLE*& pSound,int iVolume);
	bool PlaySample3D(FSOUND_SAMPLE*& pSound,const CVec3f& vPos,int iVolume);


	void UpdateListenerPosition(const CVec3f & vPos,const CVec3f& vLookDir);


	void Update(void);

	void Disable(void) { m_bInitedCorrectly=false;};

	/*////// Background music //////
	void StopBackgroundMusic(void);

	void PlayIntroSong(void);
	void PlayBoardGameSong(void);	
	void PlayPreBattleSong(void);
	void PlayBattleSong(void);	
	void PlayWinSong(void);*/

	//int* m_ppChannels[SM_MAX_CHANNELS];

	//FSOUND_SAMPLE** m_ppEffectsSamples;

	//FSOUND_STREAM* m_pBackgroundMusic;

private:
	bool m_bInitedCorrectly;

};









#endif //YOEL_SOUND_MANAGER_H