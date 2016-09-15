#ifndef CRIMSON_GAME_H
#define CRIMSON_GAME_H

#pragma once

#include "StaticModel.h"
//CStaticModel* g_pBox=NULL;

#include "StaticModelManager.h"
#include "MD5Manager.h"
#include "Character.h"

#include "Server.h"
#include "Camera.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;

#include "HumanPlayerEntity.h"


#include <vector>

class CGame
{
public:
	CGame();
	~CGame();

	virtual bool Load(std::vector<tPlayerGameState*>* pPlayersGameState);
	virtual void AddPlayers(std::vector<tPlayerGameState*>* pPlayersGameState);

	virtual void UpdateLogics(void);
	virtual void Render(void);


	virtual void OnEvent(DWORD dwGameEvent, void* pEventData);

	//void RenderCrossHair(void);

private:

	

	void AddPlayer(tPlayerGameState* pPlayerState);

	void RestartMatch();

	unsigned int FindLeaderID(int& iLeaderScore);

	void ShowScore();

	int FindWinner();

	void HandleCameraSwitch();

	CStaticModel* m_pBox;
	CStaticModel* m_pBox2;
	
	CSmartPointer<IMaterial_Export> m_spCrossHair;
	

	// debug
	CSmartPointer<IMaterial_Export> m_DebugMaterialGreen;

	CHumanPlayerEntity* m_pHumanPlayerEntity;


	FSOUND_STREAM* m_pBackgroundMusic;

	//will not work well with dynamically added and removed players
	std::vector<tPlayerGameState*>* m_pPlayersGameState;

	//int          m_iLeaderScore;
	unsigned int m_uiLeaderID;
	
	
	unsigned int m_uiCameraMode;

	static const unsigned int CAMERA_MODE_FIRST_PERSON	= 0;
	static const unsigned int CAMERA_MODE_FREE_FLY		= 1;

	CCamera* m_pActiveCamera;
	CCamera* m_pPreviousCamera;
	
	// Game Modes

	unsigned int m_uiGameMode;

	static const unsigned int GAME_MODE_TIME_LIMIT	= 0;
	static const unsigned int GAME_MODE_FRAG_LIMIT	= 1;

	// Frag Limit
	
	unsigned int m_uiFragLimit_FragsLimit;
	unsigned int m_uiFragLimit_RemainingFrags;

	// Time Limit

	float	m_fTimeLimit_MatchTime;
	float	m_fTimeLimit_TimeStarted;
	
	bool	m_bAnnounced_5_minutes_left;
	bool	m_bAnnounced_1_minute_left;

	bool	m_bMatchPoint;
	
	unsigned int m_uiClientsAdded;


};


#endif