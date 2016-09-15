#include "stdafx.h"
#include "Server.h"

#include "ce.h"
#include "stdio.h"

#include "ProjectileEntity.h"
#include "ControllableEntity.h"
#include "HurtEntity.h"
#include "TeleporterEntity.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Game.h"
extern CGame* g_pGame;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;


void IServer::ShowPlayersStates()
{
	char temp[100];
	for (int i=0;i<m_PlayersGameState.size();i++)
	{		
		/*m_PlayersGameState[i].
			sprintf_s(temp,100,"%s: Life:%d Ammo:%d",
				m_PlayersGameState[i].pName;
				m_PlayersGameState[i].*/

	}

}

tPlayerGameState* IServer::GetPlayerStateUsingConnectionID(char cID)
{
	/*int iSize = m_PlayersGameState.size();
	for (int i=0;i<iSize;i++)
	{
		if (m_PlayersGameState[i]->m_ConnectID == cID)
			return m_PlayersGameState[i];
	}*/

	m_IdsStatesSafeMap.Acquire();

	IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();

	IDS_STATES_MAP_IT it = pMap->find(cID);

	tPlayerGameState* pAns = NULL;
	
	if (it!=pMap->end())
		pAns = it->second;
	
	m_IdsStatesSafeMap.Release();

	BREAKPOINT(!pAns);

	return pAns;
}

tPlayerGameState* IServer::GetPlayerStateFromFinalVector(unsigned int uiEntityID)
{
	int iSize = m_PlayersGameState.size();
	for (int i=0;i<iSize;i++)
	{
		if (m_PlayersGameState[i]->uiEntityID == uiEntityID)
			return m_PlayersGameState[i];
	}
	

	// wasn't found!
	//BREAKPOINT(1);
	return NULL;

}

tPlayerGameState* IServer::GetPlayerStateFromFinalVector_ByConnectID(char cConnectID)
{
	int iSize = m_PlayersGameState.size();
	for (int i=0;i<iSize;i++)
	{
		if (m_PlayersGameState[i]->m_ConnectID == cConnectID)
			return m_PlayersGameState[i];
	}
	

	// wasn't found!
	//BREAKPOINT(1);
	return NULL;


}

int IServer::TranslateCombatMessageTypeToSize(char cMessageType)
{
	switch(cMessageType)
	{
	case COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL:
		return sizeof(tCombatEventClientRequestCollectable_FULL);
		break;

	case GAME_EVENT_PLAYER_DIED_BY_LEVEL:
		return sizeof(tPlayerDiedByLevel_Game);
		break;

	case SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP:
		return sizeof(tLevelDamageEvent_Server);
		break;

/*	case COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_PARTIAL:
		return sizeof(tCombatEventClientRequestCollectable_PARTIAL);
		break;*/

	case COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE:
		return sizeof(tCombatEventCollectablePickupUpdate);
		break;

	case COMBAT_EVENT_CLIENT_STATE_UPDATE_PARTIAL:
		return sizeof(tClientStateUpdate_Combat_PARTIAL);
		break;

	case COMBAT_EVENT_CLIENT_STATE_UPDATE_FULL:
		return sizeof(tClientStateUpdate_Combat_FULL);
		break;

	case COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT:
		return sizeof(tCombatEventClientRequestProjectileShoot);
		break;

	case COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT:
		return sizeof(tCombatEvent_ServerUpdate_ProjectileShoot);
		break;

	case GAME_EVENT_PLAYER_DAMAGE:
		return sizeof(tPlayerDamageEvent_Game);
		break;

	case GAME_EVENT_PLAYER_DIED_BY_WEAPON:
		return sizeof(tPlayerDiedByWeapon_Game);
		break;

	case COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE:
		return sizeof(tCombatEventAckLastKnownMessage);
		break;

	case GAME_EVENT_PLAYER_LEVEL_DAMAGE:
		return sizeof(tLevelDamageEvent_Game);
		break;

	case SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP:
		return sizeof(tMetTeleporterEvent_Server);
		break;

	case GAME_EVENT_PLAYER_TELEPORTED:
		return sizeof(tTeleportedEvent_Game);
		break;

	case GAME_EVENT_RESPAWN_PLAYER:
		return sizeof(tRespawnPlayer_Game);
		break;

	default:
		BREAKPOINT(1);//
		break;
	};

}

void IServer::Event_Player_Met_Teleporter(void* pEventData)
{
	// interpert incoming data
		tMetTeleporterEvent_Server* pData = (tMetTeleporterEvent_Server*) pEventData;

		CTeleporterEntity* pTeleportEnt = (CTeleporterEntity*) g_pQ3Map->GetEntity(pData->uiTeleporterID);

		// create message for game
		tTeleportedEvent_Game e;
		e.uiPlayerEntityID = pData->uiPlayerEntityID;
		e.uiTeleporterID   = pData->uiTeleporterID;
		e.vTeleportTarget  = pTeleportEnt->GetDestination();

		SendMessageToAll_UDP((char*)&e);

		g_pGame->OnEvent(GAME_EVENT_PLAYER_TELEPORTED,&e);


}

void IServer::Event_Player_Level_Damage(void* pEventData)
{
	// interpert incoming data
		tLevelDamageEvent_Server* pData = (tLevelDamageEvent_Server*) pEventData;


		// FIXME: perhaps this should be done inside the entities.		
		// ignore dying		
		if (GetPlayerStateFromFinalVector(pData->uiPlayerEntityID)->bDying)
			return;


		CHurtEntity* pHirtEnt = (CHurtEntity*) g_pQ3Map->GetEntity(pData->uiHurtEntity);

		// create message for game
		tLevelDamageEvent_Game e;
		e.uiPlayerEntityID = pData->uiPlayerEntityID;
		e.iDamage = pHirtEnt->GetDamage();

		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		if (pContEnt->GetLife()-e.iDamage <= 0 ) // player has no more life
			{
				tPlayerDiedByLevel_Game e;
				e.uiPlayerEntityID = pData->uiPlayerEntityID;		
				g_pGame->OnEvent(GAME_EVENT_PLAYER_DIED_BY_LEVEL,&e);	

				
				
				GetPlayerStateFromFinalVector(pData->uiPlayerEntityID)->bDying = true;
				GetPlayerStateFromFinalVector(pData->uiPlayerEntityID)->dTimeDying = 0.0;

				SendMessageToAll_UDP((char*)&e);
			} 
		else
		{
			SendMessageToAll_UDP((char*)&e);
			g_pGame->OnEvent(GAME_EVENT_PLAYER_LEVEL_DAMAGE,&e);
		}

}

void IServer::Event_Projectile_Explosion(void* pEventData)
{
	char temp[100]; 
	
	// create message for game

		tProjectileExpoltionEvent_Server* pData = (tProjectileExpoltionEvent_Server*) pEventData;

				

		//st_SoundManager->PlaySound("Sound/rocklx1a.wav",pData->vPos,255);

		float fDamageFactor = 1.f;

		switch(pData->cProjectileExplotionType)
		{
		case WEAPON_TYPE_ROCKET:
			fDamageFactor = WEAPON_ROCKET_DAMAGE;
			break;
		case WEAPON_TYPE_FIREBALL:
			fDamageFactor = WEAPON_FIREBALL_DAMAGE;
			break;
		case WEAPON_TYPE_BLUE:
			fDamageFactor = WEAPON_BLUE_DAMAGE;
			break;
		case WEAPON_TYPE_WHITE:
			fDamageFactor = WEAPON_WHITE_DAMAGE;
			break;
			
		default:
			BREAKPOINT(1); // unrecognized type!			
		break;
		};

		CVec3f vPlayerPos;
		float fDistance = 99999.f;

		int iDamage=0;
		int iSize = m_PlayersGameState.size();
		for (int i=0;i<iSize;i++)
		{
			// ignore dying
			if (m_PlayersGameState[i]->bDying)
				continue;

			CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(m_PlayersGameState[i]->uiEntityID);

			vPlayerPos = pContEnt->GetPhysicsModel()->GetPosition();
			fDistance = (pData->vPos-vPlayerPos).Length();

			if (fDistance > 200.f)
				continue;

			if (pData->uiShotByPlayerID == pContEnt->GetID())
				fDamageFactor*=0.1f;

			iDamage = (int) floorf(      (1.f - (fDistance/200.f)) * fDamageFactor);
						
			if (pData->bDirectHit)
			{
				// no distance+
				iDamage = (int) floorf(fDamageFactor*1.5);
			}

			tPlayerDamageEvent_Game e;
			e.iDamage = iDamage;
			e.uiPlayerEntityID = m_PlayersGameState[i]->uiEntityID;		
			
			/*sprintf_s(temp,100,"%s - %d damage.",m_PlayersGameState[i]->pName,iDamage);
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

			if (vPlayerPos==pData->vPos)
				return;
			else
				e.vApplyForce = ((vPlayerPos-pData->vPos).Normalize())*((float) iDamage)*0.3f;
			
			if (pContEnt->GetLife()-e.iDamage <= 0 ) // player has no more life
			{
				tPlayerDiedByWeapon_Game e;
				e.uiPlayerEntityID_victom = m_PlayersGameState[i]->uiEntityID;
				e.uiPlayerEntityID_killer = pData->uiShotByPlayerID;				

				// update all clients if there's need
				SendMessageToAll_UDP((char*)&e);

				// update game state
				g_pGame->OnEvent(GAME_EVENT_PLAYER_DIED_BY_WEAPON,&e);
				
				m_PlayersGameState[i]->bDying = true;
				m_PlayersGameState[i]->dTimeDying = 0.0;

				continue; // no need to inform about damage if player died...
			} 			
			

			// update all clients if there's need
			SendMessageToAll_UDP((char*)&e);			

			// change server game state
			g_pGame->OnEvent(GAME_EVENT_PLAYER_DAMAGE,&e );

		}

}