#include "stdafx.h"
#include "Server_Networking_Master.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Game.h"
extern CGame* g_pGame;

#include <vector>

#include "CollectableEntity.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "NetControllablePhysics.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "EngineOptions.h"
extern IEngineOptions* g_pEngineOptions;

extern bool g_bRenderableRequirePostProcessing;

DWORD AcceptConnections(tPassToConnectThread *pParam)
{

	char temp[400];

	SOCKET* pConnectionSocket = pParam->pConnectionSocket;
	
		




		(*pConnectionSocket) = socket(AF_INET, SOCK_STREAM,0); 

		if ((*pConnectionSocket)==INVALID_SOCKET)
		{
			int iError =  WSAGetLastError();
    		char error[50];
			sprintf(error,"Creating Connection Socket Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}
	 
		sockaddr_in m_SocketAdress;

		m_SocketAdress.sin_family = AF_INET;         // host byte order
		m_SocketAdress.sin_port = htons(SERVER_PORT_NUM_TCP);     // short, network byte order
		m_SocketAdress.sin_addr.s_addr = htonl (INADDR_ANY);
		memset(&(m_SocketAdress.sin_zero), '\0', 8); // zero the rest of the struct 


		// if bind() return zero than it means it's OK!
	    if (bind((*pConnectionSocket), (struct sockaddr *)&m_SocketAdress, sizeof(struct sockaddr)))
		{
    		int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"SERVER: Binding Socket Failed - Error Code:%d",iError);			 
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

		int iPort = SERVER_PORT_NUM_TCP;
		sprintf(temp,"Bound TCP listen socket to port %i",iPort);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



		unsigned long ulYes = 1; 
		if (ioctlsocket((*pConnectionSocket), FIONBIO, &ulYes) == SOCKET_ERROR) 
		{
			char error[50];
			sprintf(error,"non blocking");			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

		// "The TCP_NODELAY option disables the Nagle algorithm"
		char yes=1;
			
		if (setsockopt((*pConnectionSocket),SOL_SOCKET,TCP_NODELAY,&yes,sizeof(char))) 
		{
			int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"CLIENT: setsockopt Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		} 

		
	//////// debug debug debug //////////////
	//char temp[300];

	


	// add self as a "client" (player...)

	listen((*pConnectionSocket), SERVER_MAX_CLIENTS_ACCEPTED);


	sockaddr_in socketAdress;
	SOCKET streamSocket = NULL;
	char cSavedOponentIPAdress[16];
	char message[100];

	static int iDbg=0;

	while (!pParam->bKillMe)
	//while (1)
	{
		Sleep(70);
		int sin_size = sizeof(struct sockaddr_in);

		streamSocket = NULL;

		streamSocket = accept((*pConnectionSocket), (struct sockaddr *)&socketAdress, &sin_size);
		if (streamSocket!=INVALID_SOCKET)
		{
			// debug debug debug
 			int iBlah = sizeof(struct sockaddr);

			sockaddr t;

			int iRes = getpeername(streamSocket,&t,&iBlah);
		
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// init and fill this client game state with correct data

			tPlayerGameState* pGameState = new tPlayerGameState();

			pGameState->m_clientIP[0] = (unsigned char) t.sa_data[2];
			pGameState->m_clientIP[1] = (unsigned char) t.sa_data[2+1];
			pGameState->m_clientIP[2] = (unsigned char) t.sa_data[2+2];
			pGameState->m_clientIP[3] = (unsigned char) t.sa_data[2+3];

			pGameState->iScore = 0;
			pGameState->cControlType = PLAYER_CONTROL_TYPE_NETWORKED;
			pGameState->dTimeDying = 0.0;					
			
			/*
			if ((pGameState->m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
			{
				sprintf(temp,"UDP: socket() failed with client %u.%u.%u.%u",
					pGameState->m_clientIP[0],
					pGameState->m_clientIP[1],
					pGameState->m_clientIP[2],
					pGameState->m_clientIP[3]);
				g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

				delete(pGameState);
				continue;
			}

			unsigned long ulYes = 1; 
			if (ioctlsocket(pGameState->m_UDPSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
			{			
				sprintf(temp,"failed making a udp connection non blocking ioctlsocket(), with client %u.%u.%u.%u",
					pGameState->m_clientIP[0],
					pGameState->m_clientIP[1],
					pGameState->m_clientIP[2],
					pGameState->m_clientIP[3]);
				g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				
				delete(pGameState);
				continue;
			}

			// fill in the adress of the client
			char stringIP[16];
			sprintf(stringIP,"%u.%u.%u.%u",
				pGameState->m_clientIP[0],
				pGameState->m_clientIP[1],
				pGameState->m_clientIP[2],
				pGameState->m_clientIP[3]);


			pGameState->m_UDPAdress.sin_family = AF_INET;         // host byte order
			pGameState->m_UDPAdress.sin_port = htons(SERVER_PORT_NUM_UDP);     // short, network byte order
			pGameState->m_UDPAdress.sin_addr.s_addr =  inet_addr(stringIP);
			memset(&(pGameState->m_UDPAdress.sin_zero), '\0', 8); // zero the rest of the struct
			
			*/

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////

			pParam->pClientSocketsSafeMap->Acquire();
			SOCKETS_GAME_STATES_MAP* pMap = pParam->pClientSocketsSafeMap->GetMap();
			(*pMap)[streamSocket] = pGameState;
			pParam->pClientSocketsSafeMap->Release();

			iDbg++;
		}
		else // INVALID_SOCKET
			continue;
	
	
		strcpy(cSavedOponentIPAdress,inet_ntoa(socketAdress.sin_addr));	

		sprintf(message,"SERVER:Successfull client join! (%s)",cSavedOponentIPAdress);
	
		g_TextMessages.AddMessage(message,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	}

	return true;
}



CNetworking_Master::CNetworking_Master()
{
	m_dwGameType = SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT;
	m_dwConnectStage = CONNECTION_NOT_INIT;

	m_pConnectionSocket_TCP = NULL;
	m_hConnectionThread = NULL;
	m_dwConnectionThreadId = 0;

	m_bConnectionThreadStarted = false;

	m_pParam = NULL;

	m_dTimeTillNextUpdateClientsInPosAndAng = 100.f;

	m_dTimeTillGameStart = 10000.f;

	//m_iPlayersThatSaidHello = 0;

	m_uiLastSentPosAndAngID = 0;
}


CNetworking_Master::~CNetworking_Master()
{
	if (m_pConnectionSocket_TCP)
		closesocket(*m_pConnectionSocket_TCP);

	/*if (m_ServerSocket_UDP)
		closesocket(m_ServerSocket_UDP);*/

}

void CNetworking_Master::Think(void)
{
	
	/*if (m_dTimeTillGameStart>=0.f)
	{
		m_dTimeTillGameStart-=g_pTimer->GetDeltaTimeFAKE();
		return;
	}*/

	int iSize = m_PlayersGameState.size();

	// all have to say hello before we start
	/*if (m_iPlayersThatSaidHello<iSize-1)
		return;*/

	for (int i=0;i<iSize;i++)
	{		
		////////////////////////////////////////////////////////////////////////////////////
		/////// dying related


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

			SendMessageToAll_UDP((char*)&e);

			g_pGame->OnEvent(GAME_EVENT_RESPAWN_PLAYER,&e);

			m_PlayersGameState[i]->dTimeDying = 0.0;
			m_PlayersGameState[i]->bDying = false;
		}

	}


	
	////////////////////////////////////////////////////////////////////////////////////
	////// update everyone we the other clients positions+angles


	m_dTimeTillNextUpdateClientsInPosAndAng+=g_pTimer->GetDeltaTime();


	if (m_dTimeTillNextUpdateClientsInPosAndAng>SERVER_UPDATES_CLIENT_POS_AND_ANG_RATE)
	{
		m_dTimeTillNextUpdateClientsInPosAndAng-=SERVER_UPDATES_CLIENT_POS_AND_ANG_RATE;
		
		if (m_dTimeTillNextUpdateClientsInPosAndAng>SERVER_UPDATES_CLIENT_POS_AND_ANG_RATE)
			m_dTimeTillNextUpdateClientsInPosAndAng = 0;

		char* pNextMessage = NULL;
		CVec3f pos,angles;
		CEntity* pEntity=NULL;
		unsigned int uiEntityID=0;

		int iTotalSize = 1 + (iSize-1)*sizeof(tClientStateUpdate_Combat_FULL);

		for (int i=1;i<iSize;i++) // no need to update the server...
		{	
			m_UpdateClientInOtherClientsState[0] = OTHER_CLIENT_POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP;

			// update will be sent to client #i			
			pNextMessage = &m_UpdateClientInOtherClientsState[1];
			

			for (int j=0;j<iSize;j++)
			{
				if (i==j) // don't update client with his own data
					continue;

				uiEntityID = m_PlayersGameState[j]->uiEntityID;
				pEntity = g_pQ3Map->GetEntity(uiEntityID);
				BREAKPOINT(!pEntity);

				CControllableEntity* pPlayer = (CControllableEntity*) pEntity;

				pos = pEntity->GetPhysicsModel()->GetPosition();
				angles = pEntity->GetAngles();

				tClientStateUpdate_Combat_FULL e;
				e.fAngX = angles.x;
				e.fAngY = angles.y;
				e.fPosX = pos.x;
				e.fPosY = pos.y;
				e.fPosZ = pos.z;
				e.uiPlayerEntityID = uiEntityID;
				e.uiID = m_uiLastSentPosAndAngID;
				e.cMoveAnima = pPlayer->GetMoveState();
			
				memcpy(pNextMessage,&e,sizeof(tClientStateUpdate_Combat_FULL));
				pNextMessage+=sizeof(tClientStateUpdate_Combat_FULL);
			}

			

			// send the message to that client
			m_PlayersGameState[i]->m_CommBuffUDP.SendRawData(m_PlayersGameState[i]->m_UDPSocket,&m_PlayersGameState[i]->m_UDPAdress,
															m_UpdateClientInOtherClientsState,iTotalSize);

		}

		// restore!!
		m_uiLastSentPosAndAngID++;
	}

	
	


	//m_PlayersDataSafeMap.Release();
}

void CNetworking_Master::SendMessageToAll_UDP(char* pMessage)
{
	SendMessageToAll_UDP_Except(0,pMessage);
}

/*void CNetworking_Master::Send_RawData_MessageToAll_UDP_Except(unsigned int uiExceptEntityID, char* pData, int iLength)
{
	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();

	int iError=0;

	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			if (it->second->uiEntityID==uiExceptEntityID)
				continue;
			
			it->second->m_CommBuffUDP.SendRawData(it->second->m_UDPSocket,&it->second->m_UDPAdress,pData,iLength);
			
			if (iError<0)
			{
				if (iError==WSAENOTSOCK || iError==WSAECONNABORTED || iError==WSAECONNRESET)
				{
					
				
				}
			}
		}

	m_PlayersDataSafeMap.Release();
}
*/

void CNetworking_Master::InformPlayersOnGameStart()
{
	SOCKETS_GAME_STATES_MAP* pSocketsMap = NULL;

	tUpdateClientStartGame msg;
	

	// update all clients with this connectID->dwEntityID mapping
	m_PlayersDataSafeMap.Acquire();

	pSocketsMap = m_PlayersDataSafeMap.GetMap();


	for (SOCKETS_GAME_STATES_MAP_IT it=pSocketsMap->begin();it!=pSocketsMap->end();++it)
	{
		if (! it->second->m_bInformedGameStart) // if not already informed
		{
			//inform in game start
			it->second->m_CommBuffTCP.PushSendMessage((char*)&msg,sizeof(tUpdateClientStartGame));
			it->second->m_bInformedGameStart = true;
		}
		
	}

	m_PlayersDataSafeMap.Release();
}

void CNetworking_Master::SendMessageToAll_UDP_Except(unsigned int uiExceptEntityID, char* pMessage)
{
	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();

	int iError=0;

	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			if (it->second->uiEntityID==uiExceptEntityID)
				continue;
			
			it->second->m_CommBuffUDP.PushSendMessage(pMessage,TranslateCombatMessageTypeToSize(pMessage[0]));
			
			if (iError<0)
			{
				if (iError==WSAENOTSOCK || iError==WSAECONNABORTED || iError==WSAECONNRESET)
				{
					
				
				}
			}
		}

	m_PlayersDataSafeMap.Release();

}

void CNetworking_Master::UpdatePlayersList()
{
	SOCKETS_GAME_STATES_MAP* pSocketsMap = NULL;

	// loop through the sockets map and add them to the final vector

	m_PlayersDataSafeMap.Acquire();
	pSocketsMap = m_PlayersDataSafeMap.GetMap();

	// this is used to sort the player game states based on the connection IDs
	std::map<char,tPlayerGameState*> sortMap;

	// here, instead to go in the od
	for (SOCKETS_GAME_STATES_MAP_IT it=pSocketsMap->begin();it!=pSocketsMap->end();++it)
	{
		if (it->first == 0)
			continue;
		//it->second->cControlType = PLAYER_CONTROL_TYPE_NETWORKED;
		//m_PlayersGameState.push_back(it->second);			
		sortMap[it->first] = it->second;
	}


	for (std::map<char,tPlayerGameState*>::iterator it = sortMap.begin();it!=sortMap.end();++it)
	{
		if (it->first == 0)
			continue;
		if (!it->second->m_bAddedToServerList)
		{
			it->second->cControlType = PLAYER_CONTROL_TYPE_NETWORKED;
			m_PlayersGameState.push_back(it->second);
			it->second->m_bAddedToServerList = true;
		}		
	}

	m_PlayersDataSafeMap.Release();

	//yoel
	if (g_pGame)
		g_pGame->AddPlayers(&m_PlayersGameState);
}

void CNetworking_Master::OnEvent(DWORD dwEventID,void* pEventData)
{
	char TempMessage[200];

	int iID=-1;
	char* pString=NULL;
	tClientJoinRequest* pMsgClientJoin=NULL;
	tUpdateClientGivenEntityID* pMsgGivenEntityID=NULL;


	char* pName;
	IDS_STATES_MAP_IT it;
	IDS_STATES_MAP* pMap = NULL;

	SOCKETS_GAME_STATES_MAP* pSocketsMap = NULL;
	tPlayerGameState* pSelfPlayerGameState = NULL;
	CControllableEntity* pControllableEntity = NULL;



	
	

	switch (dwEventID)
	{

	////////////////////////////////////////
	// UDP
	////////////////////////////////////////

	/*case COMBAT_EVENT_CLIENT_STATE_UPDATE:
	{
		tClientStateUpdate_Combat* pMsg = (tClientStateUpdate_Combat*) pEventData;

		CControllableEntity* pPlayer = (CControllableEntity*) g_pQ3Map->GetEntity(pMsg->uiPlayerEntityID);

		BREAKPOINT(!pPlayer);
		
		CVec3f vNewPos(pMsg->fPosX,pMsg->fPosY,pMsg->fPosZ);
		pPlayer->GetPhysicsModel()->SetPosition(vNewPos);

		CVec3f vNewAngles(pMsg->fAngX,pMsg->fAngY,0.f);
		//pPlayer->SetAngles(vNewAngles);
		pPlayer->GetRenderModel()->SetAngles(vNewAngles);

		//sprintf(TempMessage,"Got angles: %f,%f.",pMsg->fAngX,pMsg->fAngY);
		//g_TextMessages.AddMessage(TempMessage,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);

		// now i also need to update all other clients

		
		break;
	}*/


	case COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL:
	{
		tCombatEventClientRequestCollectable_FULL* pMsg = (tCombatEventClientRequestCollectable_FULL*) pEventData;
		
		CCollectableEntity* pEntityColl = (CCollectableEntity*) g_pQ3Map->GetEntity(pMsg->uiItemID);

		if (!pEntityColl->GetCollected())
			{
				// create message for game - update local game state
				tCombatEventCollectablePickupUpdate e;
				e.uiPlayerEntityID = pMsg->uiPlayerEntityID;
				e.uiItemID         = pMsg->uiItemID;
				g_pGame->OnEvent(COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE,&e);

				// now update all clients that this object was taken by that player
				SendMessageToAll_UDP((char*) &e);
			}	

		break;
	}

	case COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT:
	{
		tCombatEvent_ServerUpdate_ProjectileShoot* pMsg = (tCombatEvent_ServerUpdate_ProjectileShoot*) pEventData;
		// update all clients
		tCombatEvent_ServerUpdate_ProjectileShoot e;
		memcpy(e.fStartPosition,pMsg->fStartPosition,sizeof(float)*3);
		memcpy(e.fDirection,pMsg->fDirection,sizeof(float)*3);
		e.ucProjectileType = pMsg->ucProjectileType;
		e.uiPlayerEntityID = pMsg->uiPlayerEntityID;
		
		
		g_TextMessages.AddMessage("Send projectile update message to all clients",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);
		SendMessageToAll_UDP_Except(pMsg->uiPlayerEntityID,(char*)&e);

		break;
	}
	
	// someone (including server) want to shoot a missile
	case COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT:
	{
		tCombatEventClientRequestProjectileShoot* pMsg = (tCombatEventClientRequestProjectileShoot*) pEventData;

		// update all clients
		tCombatEvent_ServerUpdate_ProjectileShoot e;
		memcpy(e.fStartPosition,pMsg->fStartPosition,sizeof(float)*3);
		memcpy(e.fDirection,pMsg->fDirection,sizeof(float)*3);
		e.ucProjectileType = pMsg->ucProjectileType;
		e.uiPlayerEntityID = pMsg->uiPlayerEntityID;
		SendMessageToAll_UDP_Except(pMsg->uiPlayerEntityID,(char*)&e);

		// update the server game state
		g_pGame->OnEvent(COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT,&e);			

		break;
	}

	case SERVER_EVENT_PROJECTILE_EXPLOTION_UDP:
	{
		Event_Projectile_Explosion(pEventData);
		break;
	}

	/*case GAME_EVENT_PLAYER_LEVEL_DAMAGE:
	{
		Event_Player_Level_Damage(pEventData);
		break;
	}*/

	case SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP:
	{
		Event_Player_Level_Damage(pEventData);
		break;
	}

	case SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP:
	{
		Event_Player_Met_Teleporter(pEventData);

	}
		

	/*case COMBAT_EVENT_CLIENT_STATE_UPDATE:
		// first of all update self
		//pClientStateUpdateMsg = (tClientStateUpdate_Combat*) pEventData;
		//pControllableEntity = (CControllableEntity*) g_pQ3Map->GetEntity(pClientStateUpdateMsg->uiPlayerEntityID);
		
		//pControllableEntity->GetPhysicsModel()->SetPosition(CVec3f(pClientStateUpdateMsg->fPosX,
		//	pClientStateUpdateMsg->fPosY,pClientStateUpdateMsg->fPosZ));
								
		// update all clients with this new position

		break;*/



	////////////////////////////////////////
	// TCP
	////////////////////////////////////////

	case TCP_MSG_CHAT:
		pString = (char*) pEventData;		
		iID = pString[1];

		m_IdsStatesSafeMap.Acquire();

		pMap = m_IdsStatesSafeMap.GetMap();
		it = pMap->find(iID);

		if (it!=pMap->end())
		{
			sprintf(TempMessage,"%s: %s",it->second->pName,&pString[3]);
			g_TextMessages.AddMessage(TempMessage,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);

			SendChatMessage(&pString[3],iID);
		}

		m_IdsStatesSafeMap.Release();

		break;
	case TCP_MSG_UPDATE_CLIENT_ENTITY_ID:

		pMsgGivenEntityID = (tUpdateClientGivenEntityID*) pEventData;

		// update all clients with this connectID->dwEntityID mapping
		m_PlayersDataSafeMap.Acquire();

		pSocketsMap = m_PlayersDataSafeMap.GetMap();


		for (SOCKETS_GAME_STATES_MAP_IT it=pSocketsMap->begin();it!=pSocketsMap->end();++it)
		{
			it->second->m_CommBuffTCP.PushSendMessage((char*)pEventData,sizeof(tUpdateClientGivenEntityID));							
		}

		m_PlayersDataSafeMap.Release();



		break;
	case TCP_MSG_JOIN_REQUEST:
		pMsgClientJoin = (tClientJoinRequest*) pEventData;
		
		sprintf(TempMessage,"[%s] joined. character=[%s]",pMsgClientJoin->name,pMsgClientJoin->characterName);
		g_TextMessages.AddMessage(TempMessage,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);


		break;

	case TCP_MSG_START_GAME: // would actually not come from tcp, but would be triggered from menu 

		// notice that i recreate the message data (it contains only type) inside this function
		InformPlayersOnGameStart();	

		break;

	case INFORM_START_GAME:
	{
	
		// here i should close the connection thread
		m_PlayersGameState.clear();

		// first of all add the server itself

		pSelfPlayerGameState = new tPlayerGameState();
		pSelfPlayerGameState->cControlType = PLAYER_CONTROL_TYPE_HUMAN;


		pSelfPlayerGameState->pName = new char[strlen("server")+1];
		strcpy(pSelfPlayerGameState->pName,"server");

		//char* pcPlayerChar = "MD5/mancubus/mancubus.chr";
		//char* pcPlayerChar = "MD5/vagary/vagary.chr";
		char* pcPlayerChar = "MD5/archvile/archvile.chr";
		//char* pcPlayerChar = "MD5/marine/marine.chr";
		//char* pcPlayerChar = "MD5/hellknight/hellknight.chr";


		/*pSelfPlayerGameState->pCharacterType = new char[strlen("MD5/vagary/vagary.chr")+1];
		strcpy(pSelfPlayerGameState->pCharacterType,"MD5/vagary/vagary.chr");*/

		pSelfPlayerGameState->pCharacterType = new char[strlen(pcPlayerChar)+1];
		strcpy(pSelfPlayerGameState->pCharacterType,pcPlayerChar);

		/*pSelfPlayerGameState->pCharacterType = new char[strlen("MD5/archvile/archvile.chr")+1];
		strcpy(pSelfPlayerGameState->pCharacterType,"MD5/archvile/archvile.chr");*/

		/*pSelfPlayerGameState->pCharacterType = new char[strlen("MD5/mancubus/mancubus.chr")+1];
		strcpy(pSelfPlayerGameState->pCharacterType,"MD5/mancubus/mancubus.chr");*/

		pSelfPlayerGameState->m_ConnectID = m_cHumanClientConnectID;


		// needs aquire
		// Add server to the game list as first 
		m_PlayersGameState.push_back(pSelfPlayerGameState);

		////////////////////////////////////////////////////////
		
		UpdatePlayersList();

		// stop the connection thread
		//m_pParam->bKillMe = true;

		// Load Game
		SafeDelete(g_pGame);
		g_pGame = new CGame();
		g_pGame->Load(&m_PlayersGameState);

		
		m_dwConnectStage = CONNECTION_IN_GAME;
		
		///////////////////////////////////////////////////////////////////////////

		break;
}
	default:
		if (dwEventID<100)
			BREAKPOINT(1); // unrecognized event!

	}
	
	//g_TextMessages.AddMessage(message,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

}



// change this to a simple inform all


char CNetworking_Master::AddIdNameElement(char* pcName, char* pcCharacterName)
{
	//TODO: remember to zero this upon game restart
	static char cID = 0;

	m_IdsStatesSafeMap.Acquire();

	char* name = new char[strlen(pcName)+1];
	strcpy(name,pcName);

	char* characterName = new char[strlen(pcCharacterName)+1];
	strcpy(characterName,pcCharacterName);

	IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();
	
	tPlayerGameState* pPlayerState = new tPlayerGameState();

	(*pMap)[cID] = pPlayerState;

	pPlayerState->pName = name;
	pPlayerState->pCharacterType = characterName;

	char cStoredID = cID;

	cID++;

	m_IdsStatesSafeMap.Release();

	return cStoredID;
}


bool CNetworking_Master::Connect(void *pData)
{

		m_dwConnectStage = CONNECTION_NOT_INIT;

		int iTemp = FIRST_CONNECTION_MAX_TIME / 1000;
		char WaitingForClient[200];
		g_TextMessages.AddMessage("Waiting For Clients Started",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);		
		sprintf(WaitingForClient,"(auto timeout %d Seconds)",iTemp);
		g_TextMessages.AddMessage(WaitingForClient,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);

		//m_bAlreadyInited = false;
		//m_iType = COMMUNICATION_SERVER;


		// removed to test all networking code in connection thread
		/*
		WSADATA wsaData;   // if this doesn't work
    
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
		{
		//MessageBox(HWND_DESKTOP,"WSAStartup failed.\n", "Error", MB_OK);
			g_TextMessages.AddMessage("WSAStartup failed.\n",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
		} 




		m_ConnectionSocket = socket(AF_INET, SOCK_STREAM,0); 

		if (m_ConnectionSocket==INVALID_SOCKET)
		{
			int iError =  WSAGetLastError();
    		char error[50];
			sprintf(error,"Creating Connection Socket Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}
	 

		m_SocketAdress.sin_family = AF_INET;         // host byte order
		m_SocketAdress.sin_port = htons(SERVER_PORT_NUM_TCP);     // short, network byte order
		m_SocketAdress.sin_addr.s_addr = htonl (INADDR_ANY);
		memset(&(m_SocketAdress.sin_zero), '\0', 8); // zero the rest of the struct 


		// if bind() return zero than it means it's OK!
	    if (bind(m_ConnectionSocket, (struct sockaddr *)&m_SocketAdress, sizeof(struct sockaddr)))
		{
    		int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"SERVER: Binding Socket Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}



		unsigned long ulYes = 1; 
		if (ioctlsocket(m_ConnectionSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
		{
			char error[50];
			sprintf(error,"non blocking");			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

		// "The TCP_NODELAY option disables the Nagle algorithm"
		char yes=1;
			
		if (setsockopt(m_ConnectionSocket,SOL_SOCKET,TCP_NODELAY,&yes,sizeof(char))) 
		{
			int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"CLIENT: setsockopt Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		} 


	// add self as a "client" (player...)

	char cGivenID = AddIdNameElement("Server");

	m_cHumanClientID = cGivenID;
	
	*/

	char cGivenID = AddIdNameElement("Server","MD5/archvile/archvile.chr");
	//char cGivenID = AddIdNameElement("Server","MD5/vagary/vagary.chr");
	//char cGivenID = AddIdNameElement("Server","MD5/mancubus/mancubus.chr");
	m_cHumanClientConnectID = cGivenID;

	m_dwConnectStage = CONNECTION_HANDSHAKING;

	return true;

	
}

void CNetworking_Master::SendChatMessage(const char* pMessage,char cID)
{
	int iStringLength = strlen(pMessage)+1;
	// a chat message is [chatMsgID][string length][string itself...]
	char* pcMessage = new char[iStringLength+3];
	pcMessage[0] = TCP_MSG_CHAT;
	pcMessage[1] = cID;
	pcMessage[2] = iStringLength;
	strcpy(&pcMessage[3],pMessage);


	tPlayerGameState* pPlayerGameState =  GetPlayerStateFromFinalVector_ByConnectID(cID);

	char tempMessage[256];
	if (pPlayerGameState)
	{
		sprintf_s(tempMessage,"%s: %s",pPlayerGameState->pName,pMessage);

	} else
	{
		sprintf_s(tempMessage,"server: %s",pMessage);
	}

	g_TextMessages.AddMessage(tempMessage,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);


	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();


	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			it->second->m_CommBuffTCP.PushSendMessage(pcMessage,iStringLength+3);					
		}

	m_PlayersDataSafeMap.Release();

	SafeDeleteArray(pcMessage);
	
}

bool CNetworking_Master::CreateUDPSocket(SOCKET&  sock, char connection_id)
{
	char temp[256];

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
	{
		sprintf(temp,"UDP: socket() failed");
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	}

	
	unsigned long ulYes = 1; 
	if (ioctlsocket(sock, FIONBIO, &ulYes) == SOCKET_ERROR) 
		{			
			sprintf(temp,"failed making a udp connection non blocking ioctlsocket()");
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}
	

	sockaddr_in tempAddress;

	// Bind the socket to any address and the specified port.
	tempAddress.sin_family = AF_INET;
	tempAddress.sin_port = htons(SERVER_PORT_NUM_UDP+connection_id);
	tempAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	int iBindResult = bind(sock, (SOCKADDR *) &tempAddress, sizeof(tempAddress));

	if (iBindResult)
	{
		sprintf(temp,"error in binding UDP listen socket");
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;

	}

	int iPort = SERVER_PORT_NUM_UDP+connection_id;
	sprintf(temp,"Bound UDP listen socket to port %i",iPort);
	g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	return true;
}


bool CNetworking_Master::HandShake(void)
{

if (!m_bConnectionThreadStarted)
{

	WSADATA wsaData;   // if this doesn't work
    
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
	{
		//MessageBox(HWND_DESKTOP,"WSAStartup failed.\n", "Error", MB_OK);
		g_TextMessages.AddMessage("WSAStartup failed.\n",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	} 


	///////////////////////////////////////////////////////////////////////////
	// UDP
	///////////////////////////////////////////////////////////////////////////
	char temp[400];
	
	
	//if ((m_ServerSocket_UDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) 
	

	m_bConnectionThreadStarted = true;

	
	m_pParam = new tPassToConnectThread();
	m_pConnectionSocket_TCP = new SOCKET;
	m_pParam->pConnectionSocket = m_pConnectionSocket_TCP;
	m_pParam->pClientSocketsSafeMap = &m_PlayersDataSafeMap;

	

	m_hConnectionThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)AcceptConnections,m_pParam,NULL,&m_dwConnectionThreadId);

	//SetThreadPriority(m_hConnectionThread,THREAD_PRIORITY_BELOW_NORMAL);

	//SetThreadPriority(m_hConnectionThread,THREAD_PRIORITY_LOWEST);
	
}


	

	// DEBUG DEBUG DEBUG
	

	return false;
}

// UDP messages
void  CNetworking_Master::SendPendingMessages_UDP(void)
{
	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();

	int iError=0;

	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			iError = it->second->m_CommBuffUDP.SendPendingMessages(it->second->m_UDPSocket, &it->second->m_UDPAdress);				
			
			if (iError<0)
			{
				if (iError==WSAENOTSOCK || iError==WSAECONNABORTED || iError==WSAECONNRESET)
				{
					
				
				}
			}
		}

	m_PlayersDataSafeMap.Release();


}

void CNetworking_Master::RecievePendingMessages_UDP(void)
{
	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();

	int iError=0;

	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
	{
		//iError = it->second->m_CommBuffUDP.SendPendingMessages(it->second->m_UDPSocket, &it->second->m_UDPAdress);				
		RecievePendingMessages_UDP(10,it->second->m_UDPSocket_Receive);
		
	}

	m_PlayersDataSafeMap.Release();


	//RecievePendingMessages_UDP(10);
}

void  CNetworking_Master::RecievePendingMessages_UDP(int iDepth, SOCKET& socket)
{
	int iAdressLength = sizeof(sockaddr);

	int iBytesRecieved = recvfrom(/*m_ServerSocket_UDP*/socket,m_IncomingBuffer,sizeof(m_IncomingBuffer),0,
							(sockaddr *)&m_TempSenderMessageAddr,&iAdressLength);


		// debug
//	BREAKPOINT(iBytesRecieved>10);


	if (iBytesRecieved<0)
	{
		int iError =  WSAGetLastError();

		if (iError!=10035)
		{
			int iBlah=0;
			iBlah ++;			
		}
		return;
	}

	if (!iBytesRecieved)
		return;

	/////////////////////////////////////////////////////////////////////
	CSenderAddress senderAddress;
	senderAddress.m_IP[0] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b1;
	senderAddress.m_IP[1] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b2;
	senderAddress.m_IP[2] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b3;
	senderAddress.m_IP[3] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b4;
	senderAddress.m_Port = m_TempSenderMessageAddr.sin_port;

	SENDER_ADRESS_VS_CONNECT_ID_MAP_IT it = m_SenderConnectIdMap.find(senderAddress);


	/*if (it==m_SenderConnectIdMap.end())
		return;*/

	if (m_IncomingBuffer[0]==GAME_DELTA_MESSAGE_UDP)
	{
		// if we're here then iBytesRecieved>0
		int* pIntArray =  (int*) &m_IncomingBuffer[1];

		int iFirstMessage = pIntArray[0];
		int iLastMessage = pIntArray[1];

		char* pMessages = &m_IncomingBuffer[1+4+4];
		

		int iMessagesNum = iLastMessage - iFirstMessage + 1;

		// ack the last recieved message
		tCombatEventAckLastKnownMessage msg(0);
		msg.iLastKnownMessage = iLastMessage;
		

		if (it==m_SenderConnectIdMap.end()) // not found
			HarvestIncomingMessages(pMessages,iFirstMessage,iMessagesNum,0);
		else
			HarvestIncomingMessages(pMessages,iFirstMessage,iMessagesNum,it->second);
	}
	else
	if (m_IncomingBuffer[0]==POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP)
	{
		// deal with position+angles update!

		tPlayerGameState* pPlayerGameState = NULL;

		if (it==m_SenderConnectIdMap.end())
			BREAKPOINT(1); // not found!
				
		pPlayerGameState = GetPlayerStateFromFinalVector_ByConnectID(it->second);


		if (!pPlayerGameState)
			return;

		tClientStateUpdate_Combat_PARTIAL* pMsg = (tClientStateUpdate_Combat_PARTIAL*) &m_IncomingBuffer[1];

		if (pPlayerGameState->uiLastRecievedPosAndAngID < pMsg->uiID)
		{
			pPlayerGameState->uiLastRecievedPosAndAngID = pMsg->uiID;

			CControllableEntity* pPlayer = (CControllableEntity*) g_pQ3Map->GetEntity(pPlayerGameState->uiEntityID);

			BREAKPOINT(!pPlayer);
		
			CVec3f vNewPos(pMsg->fPosX,pMsg->fPosY,pMsg->fPosZ);
			//pPlayer->GetPhysicsModel()->SetPosition(vNewPos);
			
			CVec3f vNewAngles(pMsg->fAngX,pMsg->fAngY,0.f);
			//pPlayer->SetAngles(vNewAngles);
			pPlayer->GetRenderModel()->SetAngles(vNewAngles);

			pPlayer->SetMoveState(pMsg->cMoveAnima);

			((CNetControlalblePhysics*) pPlayer->GetPhysicsModel())->SetLastRecievedPositionAngles(vNewPos,vNewAngles);

			//sprintf(TempMessage,"Got angles: %f,%f.",pMsg->fAngX,pMsg->fAngY);
			//g_TextMessages.AddMessage(TempMessage,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);		
		}

	} else
	if (m_IncomingBuffer[0]== OTHER_CLIENT_POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP)
	{

	}
	else
	BREAKPOINT(1); // unrecognized message type

	
	iDepth--;
	// test
	if (iDepth>0)
		RecievePendingMessages_UDP(iDepth,socket);
	
}

void CNetworking_Master::HarvestIncomingMessages(char* pMessages,  int iFirstMessage,int iMessagesNum,char cSenderConnectID)
{
	tPlayerGameState* pPlayerGameState = NULL;

	if (cSenderConnectID!=0)
		pPlayerGameState = GetPlayerStateFromFinalVector_ByConnectID(cSenderConnectID);

		
	if (pPlayerGameState)
	{

		if (pPlayerGameState->m_iLastMessageServerRecievedFrom==-1)
			pPlayerGameState->m_iLastMessageServerRecievedFrom=0;

		int iLastMessage = iFirstMessage+iMessagesNum-1;
		// no new messages here
		if (iLastMessage<=pPlayerGameState->m_iLastMessageServerRecievedFrom)
			return;		


		// if we're here then there are new messages
		// send an acknowledge for it
		if (pPlayerGameState->m_iLastAckedMessage!=iLastMessage)
		{
			tCombatEventAckLastKnownMessage msg(0);
			msg.iLastKnownMessage = iLastMessage;
			pPlayerGameState->m_CommBuffUDP.PushSendMessage((char*)&msg,sizeof(tCombatEventAckLastKnownMessage));

			pPlayerGameState->m_iLastAckedMessage=iLastMessage;
		}

	}

	
	char temp[300];

	char* pTravel = pMessages;

	int iCurrentMsg = 0;

	bool bNewMessageProcessed = false;


	// NOTICE: change also the increment of sizeof correctly!!!

	while (iCurrentMsg<iMessagesNum)
	{
		bNewMessageProcessed = false;


		switch(pTravel[0])
		{
		case COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE:
			{		
				if (pPlayerGameState &&
					pPlayerGameState->m_iLastMessageServerRecievedFrom<iFirstMessage+iCurrentMsg)
					{
						tCombatEventAckLastKnownMessage* pMsg = (tCombatEventAckLastKnownMessage*) pTravel;				
										
						pPlayerGameState->m_CommBuffUDP.UpdateAck(pMsg->iLastKnownMessage);

						// ack debug message
						//sprintf(temp,"Got ACK for message %i",pMsg->iLastKnownMessage);
						//g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
						
						bNewMessageProcessed = true;												
						//g_pLogFile->OutPutPlainText("Got COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE message",LOG_MESSAGE_INFORMATIVE);
					}				

				pTravel+=sizeof(tCombatEventAckLastKnownMessage);
			}
			break;

		case COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT:			
		case SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP:
				{
				if (pPlayerGameState &&
					pPlayerGameState->m_iLastMessageServerRecievedFrom<iFirstMessage+iCurrentMsg)
					{													
						OnEvent(pTravel[0],pTravel);
						bNewMessageProcessed = true;

						///// debug //////////////////////////////////////////////////////////////////////////////////////////
						if (pTravel[0]==COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT)
						{
							g_pLogFile->OutPutPlainText("Got COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT message",LOG_MESSAGE_INFORMATIVE);
							//g_TextMessages.AddMessage("Got COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT message",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
						}
						else if (pTravel[0]==SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP)
							g_pLogFile->OutPutPlainText("Got SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP message",LOG_MESSAGE_INFORMATIVE);
						//////////////////////////////////////////////////////////////////////////////////////////////////////
					}				

				pTravel+=TranslateCombatMessageTypeToSize(pTravel[0]);
			}
			break;


	

		case COMBAT_HELLO_SERVER:
			{
				if (!pPlayerGameState)
				{
					//m_iPlayersThatSaidHello++;

					tCombatHelloServer* pMsg = (tCombatHelloServer*) pTravel;				
				
					char temp[400];
					sprintf(temp,"Got Hello message from: %u,%u,%u,%u:%u with connect ID %u",
						m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b1,
						m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b2,
						m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b3,
						m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b4,
						ntohs(m_TempSenderMessageAddr.sin_port),
						pMsg->cSenderConnectionID);

					g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



					CSenderAddress senderAddress;
					senderAddress.m_IP[0] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b1;
					senderAddress.m_IP[1] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b2;
					senderAddress.m_IP[2] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b3;
					senderAddress.m_IP[3] = m_TempSenderMessageAddr.sin_addr.S_un.S_un_b.s_b4;
					senderAddress.m_Port = m_TempSenderMessageAddr.sin_port;

					m_SenderConnectIdMap[senderAddress] = pMsg->cSenderConnectionID;		

					m_PlayersGameState[pMsg->cSenderConnectionID]->m_bReceivedCombatHello = true;

					bNewMessageProcessed = true;

					/*if (cSenderConnectID!=0)
					{
						tPlayerGameState* pState = NULL;
						pState = GetPlayerStateFromFinalVector_ByConnectID(pMsg->cConnectID);
						BREAKPOINT(!pState);						
					}*/
				}			
				

				pTravel+=sizeof(tCombatHelloServer);

			}
			break;

		case COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT:
			// ignore - it's our own
			break;

		default:
			BREAKPOINT(1); // unrecognized message type !!!
		}


		if (pPlayerGameState && bNewMessageProcessed)
			pPlayerGameState->m_iLastMessageServerRecievedFrom++;
		iCurrentMsg++;
	}


}

void CNetworking_Master::SendPendingMessages_TCP(void)
{
	//TODO: make this non static!
	static double fTimeTillNextSend = 100.0;


	fTimeTillNextSend-=g_pTimer->GetDeltaTime();

	if (fTimeTillNextSend<=0.0)
	{
		fTimeTillNextSend = 100.0;
	}
	else
		return;




	m_PlayersDataSafeMap.Acquire();

	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();


	
	

	int iError=0;

	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			iError = it->second->m_CommBuffTCP.SendPendingMessages(it->first);						

			if (iError==WSAENOTSOCK || iError==WSAECONNABORTED || iError==WSAECONNRESET)
			{
				g_TextMessages.AddMessage("Client Disconnected!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				
				m_ClientsDisconnected.push_back(it->second->m_ConnectID);

				m_PlayersDataSafeMap.Acquire();
				SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();
				it = pMap->erase(it);
				m_PlayersDataSafeMap.Release();
				continue;
			}
		}

	m_PlayersDataSafeMap.Release();

	
	UpdateClientsInClientDisc();
}



void CNetworking_Master::UpdateClientsInClientDisc(void)
{
	SOCKETS_GAME_STATES_MAP* pMap = NULL;

	////////////////////////////////////////////////////////////
	// update all clients in case of client disc
	if (m_ClientsDisconnected.size()>0)
	{


		m_IdsStatesSafeMap.Acquire();
		IDS_STATES_MAP* pIdsStatesMap = m_IdsStatesSafeMap.GetMap();

		for (int iVecI=0;iVecI<m_ClientsDisconnected.size();iVecI++)
		{
			if (pIdsStatesMap->find(m_ClientsDisconnected[iVecI])!=pIdsStatesMap->end())
				pIdsStatesMap->erase(m_ClientsDisconnected[iVecI]);
		}

		m_IdsStatesSafeMap.Release();

		tUpdateClientDisc msgUpdate;

		m_PlayersDataSafeMap.Acquire();
		pMap = m_PlayersDataSafeMap.GetMap();

		for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{								
				for (int iVecI=0;iVecI<m_ClientsDisconnected.size();iVecI++)
				{
					msgUpdate.cID = m_ClientsDisconnected[iVecI];						
					it->second->m_CommBuffTCP.PushSendMessage((char*)&msgUpdate,sizeof(tUpdateClientDisc));
				}
				
		}


		m_PlayersDataSafeMap.Release();
	}

	m_ClientsDisconnected.clear();
}

const char* CNetworking_Master::GetMapName() const
{
	return g_pEngineOptions->GetDesiredLevel();
}

void CNetworking_Master::RecievePendingMessages_TCP(void)
{
	char* pMessage = NULL;
	int iMessageSize = 0;
	int iMessageType = -1;

	char temp[400];

	//int iCharNum=0;
	int iError = 0;

	m_PlayersDataSafeMap.Acquire();
	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();

	bool bClientJoined=false;

	//if (m_dwConnectStage==CONNECTION_HANDSHAKING)
	if (m_dwConnectStage != CONNECTION_NOT_INIT)
	{
		// loop on all clients and recieve messages from them
		for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();)
		{
			int iRecievedBytes = it->second->m_CommBuffTCP.RecieveIncoming(it->first);

			int iError =  WSAGetLastError();


			if (iError==WSAENOTSOCK || iError==WSAECONNABORTED || iError==WSAECONNRESET)
			{
				g_TextMessages.AddMessage("Client Disconnected!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				
				m_ClientsDisconnected.push_back(it->second->m_ConnectID);

				m_PlayersDataSafeMap.Acquire();
				SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();
				it = pMap->erase(it);
				m_PlayersDataSafeMap.Release();
				continue;
			}


			iMessageType = it->second->m_CommBuffTCP.HarvestIncomingMessages((char*)pMessage,iMessageSize);

			if (iMessageType<=0)
			{
				++it;
				continue;
			}

			////////////////////////////////////////////////////////////////////////
			// Connection Related
			////////////////////////////////////////////////////////////////////////

			
			if (iMessageType==TCP_MSG_JOIN_REQUEST)
			{
				tClientJoinRequest* pMsg = (tClientJoinRequest*) pMessage;


				sprintf(temp,"Got join request from %s",pMsg->name);
				g_TextMessages.AddMessage(temp,1.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



				it->second->pName = new char[strlen(pMsg->name)+1];
				strcpy(it->second->pName,pMsg->name);

				it->second->pCharacterType = new char[strlen(pMsg->characterName)+1];
				strcpy(it->second->pCharacterType,pMsg->characterName);

				/*char Message = TCP_MSG_CLIENT_ACCEPTED;
				it->second->m_CommBuffTCP.PushSendMessage(&Message,sizeof(char));*/

				tServerAckData msg;
				//strcpy(msg.mapName,"Maps/q3dm17.bsp");
				strcpy(msg.mapName,g_pEngineOptions->GetDesiredLevel());
				


				char cGivenID = AddIdNameElement(pMsg->name,pMsg->characterName);
				it->second->m_ConnectID = cGivenID;
				
				short sPort = SERVER_PORT_NUM_UDP+cGivenID;
				// each client listens at a different port, to allow clients to sit on the same PC
				it->second->m_UDPAdress.sin_port = htons(sPort);     // short, network byte order
				 
				

				if (!CreateUDPSocket(it->second->m_UDPSocket_Receive,cGivenID))
				{
					sprintf(temp,"Error creating listening UDP socket for connection ID = %d",cGivenID);
					g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				}



				////////////////////////////////////////////////////////////////////////////////////////
				// Should it be the same socket as previous ??

				if ((it->second->m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
				{
					sprintf(temp,"UDP: socket() failed with client %u.%u.%u.%u",
						it->second->m_clientIP[0],
						it->second->m_clientIP[1],
						it->second->m_clientIP[2],
						it->second->m_clientIP[3]);
					g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

				}

				unsigned long ulYes = 1; 
				if (ioctlsocket(it->second->m_UDPSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
				{			
					sprintf(temp,"failed making a udp connection non blocking ioctlsocket(), with client %u.%u.%u.%u",
						it->second->m_clientIP[0],
						it->second->m_clientIP[1],
						it->second->m_clientIP[2],
						it->second->m_clientIP[3]);
					g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

				}

				// fill in the adress of the client
				char stringIP[16];
				sprintf(stringIP,"%u.%u.%u.%u",
					it->second->m_clientIP[0],
					it->second->m_clientIP[1],
					it->second->m_clientIP[2],
					it->second->m_clientIP[3]);


				it->second->m_UDPAdress.sin_family = AF_INET;         // host byte order
				it->second->m_UDPAdress.sin_port = htons(SERVER_PORT_NUM_UDP+cGivenID);     // short, network byte order
				it->second->m_UDPAdress.sin_addr.s_addr =  inet_addr(stringIP);
				memset(&(it->second->m_UDPAdress.sin_zero), '\0', 8); // zero the rest of the struct

				////////////////////////////////////////////////////////////////////////////////////////


				msg.cID = cGivenID;

				it->second->m_CommBuffTCP.PushSendMessage((char*)&msg,sizeof(tServerAckData));

				// update him with himself
				/*tUpdateClientInOtherClientJoin msgUpdate;
				strcpy(msgUpdate.name,pMsg->name);*/
				
				//msgUpdate.cID = AddIdNameElement(msgUpdate.name);

								
				sprintf(temp,"Gave ID=%i to player %s",cGivenID,pMsg->name);
				g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

				sprintf(temp,"Going to send him UDP messages on port: %u",sPort);
				g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

				bClientJoined=true;														
			} else 
			{
				OnEvent(iMessageType,pMessage);

			}

			++it;
		}

	}
	/*else if (m_dwConnectStage == CONNECTION_IN_GAME)
	{
		// loop on all clients and recieve messages from them
		for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{
			it->second->m_CommBuffTCP.RecieveIncoming(it->first);
			iMessageType = it->second->m_CommBuffTCP.HarvestIncomingMessages((char*)pMessage,iMessageSize);

			if (iMessageType==NO_COMPLETE_INCOMING_MESSAGE || iMessageType==0)
				continue;

			OnEvent(iMessageType,pMessage);
		}
	}*/

	m_PlayersDataSafeMap.Release();

	UpdateClientsInClientDisc();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// inform all other clients on the new client (including himself)
	if (bClientJoined)
	{
		tUpdateClientInOtherClientJoin msgUpdate;

		m_PlayersDataSafeMap.Acquire();
		pMap = m_PlayersDataSafeMap.GetMap();

		for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{

				m_IdsStatesSafeMap.Acquire();

				IDS_STATES_MAP* pIdsStatesMap = m_IdsStatesSafeMap.GetMap();
				
				for (IDS_STATES_MAP_IT idsIT=pIdsStatesMap->begin();idsIT!=pIdsStatesMap->end();++idsIT)
				{

					msgUpdate.cID = idsIT->first;
					strcpy(msgUpdate.name,idsIT->second->pName);
					strcpy(msgUpdate.characterName,idsIT->second->pCharacterType);
					

					it->second->m_CommBuffTCP.PushSendMessage((char*)&msgUpdate,sizeof(tUpdateClientInOtherClientJoin));
				}

				m_IdsStatesSafeMap.Release();
		}


		m_PlayersDataSafeMap.Release();

	}

	// if game already started
	if (CONNECTION_IN_GAME == m_dwConnectStage)
	{
		//update our playerslist with the new player
		UpdatePlayersList();
		InformPlayersOnGameStart();
	}

	CVec3f vColor(0.f,1.f,1.f);

	/*m_IdsStatesSafeMap.Acquire();

	IDS_STATES_MAP* pIdsStatesMap = m_IdsStatesSafeMap.GetMap();

	int iCharNum=0;
	for (IDS_STATES_MAP_IT it=pIdsStatesMap->begin();it!=pIdsStatesMap->end();++it)
	{

		BREAKPOINT(!it->second);
		BREAKPOINT(!it->second->pName);
		BREAKPOINT(!it->second->pCharacterType);

		g_pGraphicsAPI->printf(vColor,10,400+30*iCharNum,"Player connectID:%i, entityID%i, ip:%i.%i.%i.%i - name:%s  character:%s",
			it->first,it->second->uiEntityID,
			it->second->m_clientIP[0],it->second->m_clientIP[1],it->second->m_clientIP[2],it->second->m_clientIP[3],
			it->second->pName,it->second->pCharacterType);	
		iCharNum++;
	}

	m_IdsStatesSafeMap.Release();*/

	int iLine = 0;
	int iSize = m_PlayersGameState.size();
	for (int i=0;i<iSize;i++)
	{
		tPlayerGameState* pPlayerState = m_PlayersGameState[i];

		BREAKPOINT(!pPlayerState);
		BREAKPOINT(!pPlayerState->pName);
		BREAKPOINT(!pPlayerState->pCharacterType);

		int iConnectID = pPlayerState->m_ConnectID;

		g_pGraphicsAPI->printf(vColor,10,400+30*iLine,"%s %s ConnectID:%u EntityID:%u",
			pPlayerState->pName,
			pPlayerState->pCharacterType,
			iConnectID,
			pPlayerState->uiEntityID);

		iLine++;
	}


	/*if (g_bRenderableRequirePostProcessing)
		g_pGraphicsAPI->printf(vColor,10,400+30*iLine,"FOUND renderables requiring post processing!\n");
	else
		g_pGraphicsAPI->printf(vColor,10,400+30*iLine,"DIDN'T find renderables requiring post processing.\n");*/

}

/*void CNetworking_Master::PushSendMessageToAllClients_TCP(char* pMessageData, int iSize)
{

	m_PlayersDataSafeMap.Acquire();
	SOCKETS_GAME_STATES_MAP* pMap = m_PlayersDataSafeMap.GetMap();


	// loop on all clients and recieve messages from them
	for (SOCKETS_GAME_STATES_MAP_IT it=pMap->begin();it!=pMap->end();)
	{
		it->second->m_CommBuffTCP.PushSendMessage(pMessageData,iSize);
	}



	m_PlayersDataSafeMap.Release();

}*/