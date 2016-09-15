#include "stdafx.h"
#include "Server_Local.h"

#include "ce.h"

#include "Game.h"
extern CGame* g_pGame;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "TeleporterEntity.h"
#include "HurtEntity.h"
#include "ProjectileEntity.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "EngineOptions.h"
extern IEngineOptions* g_pEngineOptions;

CServer_Local::CServer_Local()
{
	m_dwGameType     = SERVER_GAME_TYPE_LOCAL;
	m_dwConnectStage = CONNECTION_NOT_INIT;
}

CServer_Local::~CServer_Local()
{

}

/*void CServer_Local::HandlePlayerDeath(unsigned int uiPlayerID)
{

}*/

const char* CServer_Local::GetMapName() const
{
	return g_pEngineOptions->GetDesiredLevel();
}

void CServer_Local::SendChatMessage(const char* pMessage,char cID)
{
	g_TextMessages.AddMessage("You must be connected in order to chat.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
}

bool CServer_Local::HandShake(void)
{
	m_PlayersGameState.clear();

	/*m_iPlayersNum = iPlayersNum;
	m_pPlayersGameState = new tPlayerGameState[m_iPlayersNum];*/

	char temp[400];

	//int iPlayersNum = 1;
	//int iPlayersNum = 5;
	//int iPlayersNum = 2;
	//int iPlayersNum = 4;
	int iPlayersNum = 3;

	for (int i=0;i<iPlayersNum;i++)
	{
		tPlayerGameState* pGameState = new tPlayerGameState();
		m_PlayersGameState.push_back(pGameState);

		pGameState->iScore = 0;

		if (i==0)
			sprintf(temp,"ZeDuS");
		else if (i==1)
			sprintf(temp,"DTag");
		else if (i==2)
			sprintf(temp,"K.G.");
		else if (i==3)
			sprintf(temp,"Lior");
		else
			sprintf(temp,"Player #%d",i);
		pGameState->pName = new char[strlen(temp)+1];
		strcpy(pGameState->pName,temp);

		char* characterNames[] = 
		{ 
			"MD5/mancubus/mancubus.chr",			
			"MD5/vagary/vagary.chr",
			"MD5/archvile/archvile.chr",
			"MD5/hellknight/hellknight.chr",			
			"MD5/marine/marine.chr"
		};

		//switch(j)
		switch(i)
		{
		case 0:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_HUMAN;
			//sprintf(temp,characterNames[3]);						
			//sprintf(temp,characterNames[2]);						
			sprintf(temp,characterNames[0]);						
			break;
		case 1:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_BOT;
			sprintf(temp,characterNames[1]);								
			break;
		case 2:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_BOT;
			sprintf(temp,characterNames[0]);
			break;
		case 3:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_BOT;
			sprintf(temp,characterNames[2]);
			break;
		case 4:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_BOT;
			sprintf(temp,characterNames[3]);						
			break;
		default:
			pGameState->cControlType = PLAYER_CONTROL_TYPE_BOT;
			sprintf(temp,characterNames[2]);						
			break;
		};
		
		pGameState->pCharacterType = new char[strlen(temp)+1];
		strcpy(pGameState->pCharacterType,temp);

		pGameState->bDying = false;
		pGameState->dTimeDying = 0.0;

	}

	//m_dwConnectStage = CONNECTION_WAITING_FOR_GAME_START; // no need, we can simply start
	m_dwConnectStage = CONNECTION_IN_GAME;
	

	//////////////////////////////////////////////////////////////////////////
	// Load Game
	SafeDelete(g_pGame);
	g_pGame = new CGame();
	g_pGame->Load(&m_PlayersGameState);
	
	///////////////////////////////////////////////////////////////////////////

	return true;
}

void CServer_Local::Think(void)
{
	int iSize = m_PlayersGameState.size();
	for (int i=0;i<iSize;i++)
	{
		if (m_PlayersGameState[i]->bDying)
			m_PlayersGameState[i]->dTimeDying+=g_pTimer->GetDeltaTimeFAKE()*0.001;

		// if dying more then 5 seconds
		if (m_PlayersGameState[i]->dTimeDying > 3.0)
		{
			//BREAKPOINT(1);
			// choose a start position to the entity
			// FIXME: make sure nothing is there!
			int iStartPos = rand() % g_pQ3Map->GetStartPositionsNum();


			tRespawnPlayer_Game e(0);
			e.uiPlayerEntityID = m_PlayersGameState[i]->uiEntityID;
			e.cStartPosition = iStartPos;

			g_pGame->OnEvent(GAME_EVENT_RESPAWN_PLAYER,&e);

			m_PlayersGameState[i]->dTimeDying = 0.0;
			m_PlayersGameState[i]->bDying = false;

		}

	}

	
}

// pData ignored
bool CServer_Local::Connect(void* pData)
{
	m_dwConnectStage = CONNECTION_HANDSHAKING;

	return true;
}


void CServer_Local::OnEvent(DWORD dwEventID,void* pEventData)
{

	if (dwEventID==COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT)
	{

	}else
	if (dwEventID==COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT)
	{
		// do nothing, there's no one to update

	}
	else
	// damage to player from explotion
	if (dwEventID==SERVER_EVENT_PROJECTILE_EXPLOTION_UDP)
	{
		// interpert incoming data
		Event_Projectile_Explosion(pEventData);
			
		// figure out the damage to all players, and apply it to them
		
	} else
	if (dwEventID==COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL)
	{
		// interpert incoming data
		tCombatEventClientRequestCollectable_FULL* pData = (tCombatEventClientRequestCollectable_FULL*) pEventData;
	
		// create message for game
		tCombatEventCollectablePickupUpdate e;
		e.uiPlayerEntityID = pData->uiPlayerEntityID;
		e.uiItemID         = pData->uiItemID;

		g_pGame->OnEvent(COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE,&e);
	} else if (dwEventID==SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP)
	{

		Event_Player_Met_Teleporter(pEventData);

	} else if (dwEventID==SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP)
	{

		Event_Player_Level_Damage(pEventData);

	}
	else
	{
		BREAKPOINT(1); // ERROR! unrecognized event !!!!
	}

}