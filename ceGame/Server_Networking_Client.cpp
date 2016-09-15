#include "stdafx.h"
#include "Server_Networking_Client.h"
#include <assert.h>

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "ProjectileEntity.h"

#include "Game.h"
extern CGame* g_pGame;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;

CNetworking_Client::CNetworking_Client()
{
	m_dwGameType = SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT;

	m_dwConnectStage = CONNECTION_NOT_INIT;

	m_ConnectionSocket=NULL;
	m_StreamingSocket=NULL;

	m_bJoinRequestSent = false;

	m_pcName = NULL;
	m_pServerIP = NULL;

	m_iLastRecievedMessage = -1;
	m_iLastAckedMessage = -1;
	m_uiLastSentPosAndAngID=0;
}


CNetworking_Client::~CNetworking_Client()
{
	if (m_ConnectionSocket)
		closesocket(m_ConnectionSocket);

	if (m_StreamingSocket)
		closesocket(m_StreamingSocket);

}

void CNetworking_Client::Think(void)
{
	if (m_uiHumanControlledID==0xFFFFFFFF)
		return;

	
	//change this into a member variable
	static double dTimeSinceLastPositionAndAngUpdate=0;

	dTimeSinceLastPositionAndAngUpdate+=g_pTimer->GetDeltaTime();


	if (dTimeSinceLastPositionAndAngUpdate>CLIENT_POS_AND_ANG_UPDATE_RATE)
	{
		dTimeSinceLastPositionAndAngUpdate-=CLIENT_POS_AND_ANG_UPDATE_RATE;
		
		if (dTimeSinceLastPositionAndAngUpdate>CLIENT_POS_AND_ANG_UPDATE_RATE)
			dTimeSinceLastPositionAndAngUpdate = 0;

		CControllableEntity* pPlayer = (CControllableEntity*) g_pQ3Map->GetEntity(m_uiHumanControlledID);

		if (!pPlayer)
			return;
		tClientStateUpdate_Combat_PARTIAL e;
		CVec3f vPos = pPlayer->GetPhysicsModel()->GetPosition();;
		e.fPosX = vPos.x;
		e.fPosY = vPos.y;
		e.fPosZ = vPos.z;
		CVec3f vAngles = pPlayer->GetAngles();
		e.fAngX = vAngles.x;
		e.fAngY = vAngles.y;
		e.uiID = m_uiLastSentPosAndAngID;

		e.cMoveAnima = pPlayer->GetMoveState();

		//restore!
		m_uiLastSentPosAndAngID++; // advance it
	
		//TODO: make this a member variable
		char send[200];

		send[0] = POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP;

		memcpy(&send[1],&e,TranslateCombatMessageTypeToSize(COMBAT_EVENT_CLIENT_STATE_UPDATE_PARTIAL));

		// restore!
		m_CommBuffUDP.SendRawData(m_ServerSocket_UDP,&m_SendToAdress_UDP,send,TranslateCombatMessageTypeToSize(COMBAT_EVENT_CLIENT_STATE_UPDATE_PARTIAL)+1);


	}

	
	
}

void CNetworking_Client::OnEvent(DWORD dwEventID,void* pEventData)
{
	//tPlayerGameState* pSelfPlayerGameState = NULL;
	//CControllableEntity* pControllableEntity = NULL;


	// udp
	//tClientStateUpdate_Combat* pClientStateUpdateMsg = NULL;
	
	switch (dwEventID)
	{

	////////////////////////////////////////
	// UDP
	////////////////////////////////////////

	// this means that server has updated us with a projectile shoot
	// (might be our own!)
	case COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT:
	{
		tCombatEvent_ServerUpdate_ProjectileShoot* pMsg = (tCombatEvent_ServerUpdate_ProjectileShoot*) pEventData;

		//TODO: ignore if it's our own
		if (m_dwGameType == SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT &&
		    m_uiHumanControlledID == pMsg->uiPlayerEntityID)
			return;

		{		
			float fSpeed = 1.f;

			if (pMsg->ucProjectileType == WEAPON_TYPE_ROCKET)
				fSpeed = WEAPON_ROCKET_SPEED;
			else if (pMsg->ucProjectileType == WEAPON_TYPE_FIREBALL)
				fSpeed = WEAPON_FIREBALL_SPEED;
			else if (pMsg->ucProjectileType == WEAPON_TYPE_BLUE)
				fSpeed = WEAPON_BLUE_SPEED;
			else if (pMsg->ucProjectileType == WEAPON_WHITE_SPEED)
				fSpeed = WEAPON_WHITE_SPEED;							
			else
				BREAKPOINT(1);

			g_pQ3Map->SpawnProjectile(pMsg->ucProjectileType,CVec3f(pMsg->fStartPosition),CVec3f(pMsg->fDirection)*fSpeed,pMsg->uiPlayerEntityID);
		}
		
	}
	break;


	// this means WE requested this - so just redirect it to the server
	case COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT:
	{
		// for now, do not request anything
		m_CommBuffUDP.PushSendMessage((char*)pEventData,TranslateCombatMessageTypeToSize(dwEventID));
	}
	break;

	// this means WE requested this - so just redirect it to the server
	case SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP:
	{
		m_CommBuffUDP.PushSendMessage((char*)pEventData,TranslateCombatMessageTypeToSize(dwEventID));
	}
	break;

	case COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL:
	{

		/*tCombatEventClientRequestCollectable_FULL* pRequestCollectableMsg = (tCombatEventClientRequestCollectable_FULL*) pEventData;
		
		// translate from the full mode to the compact mode to be transfered over the net
		// (server will know who sent it anyway)
		tCombatEventClientRequestCollectable_PARTIAL msg;
		msg.uiItemID = pRequestCollectableMsg->uiItemID;

		m_CommBuffUDP.PushSendMessage((char*) &msg,TranslateCombatMessageTypeToSize(msg.cMsgType));*/

		//do nothing

	}
	break;

	/*case COMBAT_EVENT_CLIENT_STATE_UPDATE:
		pClientStateUpdateMsg = (tClientStateUpdate_Combat*) pEventData;
		pControllableEntity = (CControllableEntity*) g_pQ3Map->GetEntity(pClientStateUpdateMsg->uiPlayerEntityID);
		
		pControllableEntity->GetPhysicsModel()->SetPosition(CVec3f(pClientStateUpdateMsg->fPosX,
			pClientStateUpdateMsg->fPosY,pClientStateUpdateMsg->fPosZ));
															
		break;*/

	default:
		//BREAKPOINT(1); //
		break;
	};

}

void CNetworking_Client::RemoveIDNameElement(char cID)
{
	m_IdsStatesSafeMap.Acquire();

	IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();

	if (pMap->find(cID) != pMap->end())
		pMap->erase(cID);
	
	m_IdsStatesSafeMap.Release();



}
void CNetworking_Client::AddIdNameElement(char cID,char* pcName,char* pcCharacterName)
{
	m_IdsStatesSafeMap.Acquire();

	IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();
	

	if (pMap->find(cID) == pMap->end())
	{
		char* pchNewName = new char[strlen(pcName)+1];
		strcpy(pchNewName,pcName);


		char* pchNewCharacterName = new char[strlen(pcCharacterName)+1];
		strcpy(pchNewCharacterName,pcCharacterName);

		tPlayerGameState* pPlayerState = new tPlayerGameState();

		pPlayerState->pName = pchNewName;
		pPlayerState->pCharacterType = pchNewCharacterName;
		pPlayerState->m_ConnectID = cID;

		(*pMap)[cID] = pPlayerState;
	}
	

	m_IdsStatesSafeMap.Release();

}

bool CNetworking_Client::Connect(void* pData)
{	
	m_dwConnectStage = CONNECTION_NOT_INIT;


	///////////////////////////////////////////////////////////////////////////
	// TCP
	///////////////////////////////////////////////////////////////////////////

	tConnectData* pConData = (tConnectData*) pData;

	// send a request to connect to pConData->pcHostIP

	if (m_ConnectionSocket)
	{
		closesocket(m_ConnectionSocket);
		m_ConnectionSocket=NULL;
	}

	SafeDeleteArray(m_pcName);
	m_pcName = new char[strlen(pConData->pcPlayerName)+1];
	strcpy(m_pcName,pConData->pcPlayerName);


	assert(_CrtCheckMemory());

	if (pConData->ipHostAdress[0]==NULL || pConData->ipHostAdress[1]==NULL)
	{
		g_TextMessages.AddMessage("IP doesn't fit standarts!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		m_dwConnectStage = CONNECTION_NOT_INIT;
		g_pMainMenu->Action(MENU_ACTION_CONNECTION_ABORTED);
		return false;
	}

	assert(_CrtCheckMemory());

	int iDotsCount=0;
	for (int i=0;i<strlen(pConData->ipHostAdress);i++)
	{
		if (pConData->ipHostAdress[i]=='.')
			iDotsCount++;
	}

	assert(_CrtCheckMemory());

	if (iDotsCount!=3)
	{
		g_TextMessages.AddMessage("IP doesn't fit standarts!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		m_dwConnectStage = CONNECTION_NOT_INIT;
		g_pMainMenu->Action(MENU_ACTION_CONNECTION_ABORTED);
		return false;
	}

	assert(_CrtCheckMemory());

	WSADATA wsaData;   // if this doesn't work
    
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
	{
	   //MessageBox(HWND_DESKTOP,"WSAStartup failed.\n", "Error", MB_OK);
	   g_TextMessages.AddMessage("WSAStartup failed.\n",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
       return false;
    } 



	//char tempIP[30];
	//sprintf(tempIP,"84.174.81.187");
	//sprintf(tempIP,"www.google.com");

	hostent *hostInfo;
	hostInfo = gethostbyname(pConData->ipHostAdress);
	//hostInfo = gethostbyname(tempIP);
	
	if (hostInfo == NULL) 
	 {
	  //BREAKPOINT(1);
	  g_TextMessages.AddMessage("problem interpreting host!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	  //cout << "problem interpreting host: " << buf << "\n";		
	 }

	 //int socketDescriptor;
	  // Create a socket.  "AF_INET" means it will use the IPv4 protocol.
	// "SOCK_STREAM" means it will be a reliable connection (i.e., TCP;
	// for UDP use SOCK_DGRAM), and I'm not sure what the 0 for the last
	// parameter means, but it seems to work.
	m_ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ConnectionSocket < 0) 
	{
		 g_TextMessages.AddMessage("Cannot create socket!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 //BREAKPOINT(1);
	    //cerr << "cannot create socket\n";
	   // exit(1);
	}

	// Connect to server.  First we have to set some fields in the
	// serverAddress structure.  The system will assign me an arbitrary
	// local port that is not in use.

	 sockaddr_in serverAddress;

	serverAddress.sin_family = hostInfo->h_addrtype;
	memcpy((char *) &serverAddress.sin_addr.s_addr,
			hostInfo->h_addr_list[0], hostInfo->h_length);
	serverAddress.sin_port = htons(SERVER_PORT_NUM_TCP);




	// try connecting to the server
	//if (connect(m_ConnectionSocket, (struct sockaddr *)&i, sizeof(struct sockaddr)))
	//if (connect(m_ConnectionSocket, (struct sockaddr *)&m_SocketAdress, sizeof(struct sockaddr)))
	if (connect(m_ConnectionSocket,
              (struct sockaddr *) &serverAddress,
              sizeof(serverAddress)) < 0)
	{
		int iError =  WSAGetLastError();
		char error[200];
		sprintf(error,"CLIENT: Connecting To Server ""%s"" Failed - Error Code:%d",pConData->ipHostAdress,iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 closesocket(m_ConnectionSocket);
		 m_dwConnectStage = CONNECTION_NOT_INIT;
		 g_pMainMenu->Action(MENU_ACTION_CONNECTION_ABORTED);
	    return false;
		// here i should handle that "error"
		// look in the .net help for return value failure... 
	}

	
	unsigned long ulYes = 1; 
	if (ioctlsocket(m_ConnectionSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
	{
		char error[50];
		 sprintf(error,"non blocking");			 
	    //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 m_dwConnectStage = CONNECTION_NOT_INIT;
		 g_pMainMenu->Action(MENU_ACTION_CONNECTION_ABORTED);
		return false;
	}
	

	m_StreamingSocket=m_ConnectionSocket;


	//m_bAlreadyInited = true;

	//iDebugSentMessages=0;
	//iDebugRecieveMessages=0;

	assert(_CrtCheckMemory());

	char temp[400];

	sprintf(temp,"CLIENT:Network Connection To IP:%s Succesfully Created.",pConData->ipHostAdress);
	g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	assert(_CrtCheckMemory());

	///////////////////////////////////////////////////////////////////////////
	// UDP
	///////////////////////////////////////////////////////////////////////////
	
	
	if ((m_ServerSocket_UDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) 
		{
			sprintf(temp,"UDP: socket() failed with client %s",pConData->ipHostAdress);
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

	ulYes = 1; 
	if (ioctlsocket(m_ServerSocket_UDP, FIONBIO, &ulYes) == SOCKET_ERROR) 
		{			
			sprintf(temp,"failed making a udp connection non blocking ioctlsocket(), with client %s",
				pConData->ipHostAdress);
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

	
	m_pServerIP = new char [strlen(pConData->ipHostAdress)+1];
	strcpy(m_pServerIP,pConData->ipHostAdress);
		

	m_dwConnectStage = CONNECTION_HANDSHAKING;

	return true;

}

void CNetworking_Client::SendChatMessage(const char* pMessage,char cID)
{
	int iStringLength = strlen(pMessage)+1;
	// a chat message is [chatMsgID][senderConnectID][string length][string itself...]
	char* pcMessage = new char[iStringLength+3];
	pcMessage[0] = TCP_MSG_CHAT;
	pcMessage[1] = cID;
	pcMessage[2] = iStringLength;
	strcpy(&pcMessage[3],pMessage);

	m_CommBuffTCP.PushSendMessage(pcMessage,iStringLength+3);

	SafeDeleteArray(pcMessage);
}

bool CNetworking_Client::HandShake(void)
{
	if (!m_bJoinRequestSent)
	{
		m_bJoinRequestSent = true;


		tClientJoinRequest msg;

		//strcpy(msg.name,"Yoel.");

		strcpy(msg.name,m_pcName);
		
		//strcpy(msg.characterName,"MD5/vagary/vagary.chr");

		//if (rand()%2==0)
			strcpy(msg.characterName,"MD5/marine/marine.chr");


		if (strcmp(m_pcName,"mancubus")==0)
			strcpy(msg.characterName,"MD5/mancubus/mancubus.chr");
		else if (strcmp(m_pcName,"marine")==0)
			strcpy(msg.characterName,"MD5/marine/marine.chr");
		else if (strcmp(m_pcName,"hellknight")==0)
			strcpy(msg.characterName,"MD5/hellknight/hellknight.chr");
		else if (strcmp(m_pcName,"archvile")==0)
			strcpy(msg.characterName,"MD5/archvile/archvile.chr");
		else if (strcmp(m_pcName,"vagary")==0)
			strcpy(msg.characterName,"MD5/vagary/vagary.chr");
		else // default
			strcpy(msg.characterName,"MD5/archvile/archvile.chr");

		m_CommBuffTCP.PushSendMessage((char*)&msg,sizeof(tClientJoinRequest));
	}

	return true;
}

// UDP messages
void CNetworking_Client::SendPendingMessages_UDP(void)
{
	//TODO: make this non static!
	static double fTimeTillNextSend = 100.0;


	fTimeTillNextSend-=g_pTimer->GetDeltaTime();

	if (fTimeTillNextSend<=0.0)
	{
		m_CommBuffUDP.SendPendingMessages(m_ServerSocket_UDP,&m_SendToAdress_UDP);
		fTimeTillNextSend = 100.f;

	}
}

void CNetworking_Client::RecievePendingMessages_UDP(void)
{
	RecievePendingMessages_UDP(10);
}

void CNetworking_Client::RecievePendingMessages_UDP(int iDepth)
{
	int iAdressLength = sizeof(sockaddr);

	// m_TempSenderAdress_UDP is ignored for now, since only server will speak to us anyway
	int iBytesRecieved = recvfrom(m_RecieveSocket_UDP,m_IncomingBuffer,sizeof(m_IncomingBuffer),0,
							(sockaddr *)&m_TempSenderAdress_UDP,&iAdressLength);




	if (iBytesRecieved<0)
	{
		int iError =  WSAGetLastError();

		if (iError!=10035)
		{
			char temp[200];
			sprintf(temp,"recvfrom Recieve Error: %i",iError);
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		}
		return;
	}

	if (!iBytesRecieved)
		return;


	// if we're here then iBytesRecieved>0



	if (m_IncomingBuffer[0]==GAME_DELTA_MESSAGE_UDP)
	{

		int* pIntArray =  (int*) &m_IncomingBuffer[1];

		int iFirstMessage = pIntArray[0];
		int iLastMessage = pIntArray[1];

		char* pMessages = &m_IncomingBuffer[1+4+4];
		

		int iMessagesNum = iLastMessage - iFirstMessage + 1;
		

		// if it's new
		if (m_iLastRecievedMessage<iLastMessage)
		{

			if (m_iLastAckedMessage!=iLastMessage)
			{
				tCombatEventAckLastKnownMessage msg(m_cHumanClientConnectID);
				msg.iLastKnownMessage = iLastMessage;
				m_CommBuffUDP.PushSendMessage((char*)&msg,sizeof(tCombatEventAckLastKnownMessage));

				m_iLastAckedMessage=iLastMessage;
			}

			HarvestIncomingMessages(pMessages,iFirstMessage,iMessagesNum);
		}
	} else 	
	if (m_IncomingBuffer[0]==OTHER_CLIENT_POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP)
	{
		int iMessagesNum = (iBytesRecieved-1)/sizeof(tClientStateUpdate_Combat_FULL);

		if (iMessagesNum*sizeof(tClientStateUpdate_Combat_FULL)!=iBytesRecieved-1)
			BREAKPOINT(1);// we have a problem!!

		char* pTravel = &m_IncomingBuffer[1];
		tClientStateUpdate_Combat_FULL* pMsg = NULL;

		for (int i=0;i<iMessagesNum;i++)
		{
			pMsg = (tClientStateUpdate_Combat_FULL*) pTravel;


			if (m_uiHumanControlledID==pMsg->uiPlayerEntityID)
			{
				g_TextMessages.AddMessage("Got a position update on myself!!!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				pTravel+=sizeof(tClientStateUpdate_Combat_FULL);
				continue;
			}
			
			tPlayerGameState* pPlayerGameState = GetPlayerStateFromFinalVector(pMsg->uiPlayerEntityID);
			

			if (!pPlayerGameState)
			{
				// we aren't proparly informed yet of this client
				continue;
			}

			if (pPlayerGameState->uiLastRecievedPosAndAngID < pMsg->uiID)
				pPlayerGameState->uiLastRecievedPosAndAngID = pMsg->uiID;
			else
			{
				pTravel+=sizeof(tClientStateUpdate_Combat_FULL);
				continue;
			}			

			CControllableEntity* pPlayer = (CControllableEntity*) g_pQ3Map->GetEntity(pMsg->uiPlayerEntityID);

			BREAKPOINT(!pPlayer);
			
			CVec3f vNewPos(pMsg->fPosX,pMsg->fPosY,pMsg->fPosZ);
			pPlayer->GetPhysicsModel()->SetPosition(vNewPos);

			CVec3f vNewAngles(pMsg->fAngX,pMsg->fAngY,0.f);
			pPlayer->GetRenderModel()->SetAngles(vNewAngles);			

			pPlayer->SetMoveState(pMsg->cMoveAnima);
			

			pTravel+=sizeof(tClientStateUpdate_Combat_FULL);
		}

	} else
		BREAKPOINT(1); // unrecognized message type

	iDepth--;
	// test
	if (iDepth>0)
		RecievePendingMessages_UDP(iDepth);
}

void CNetworking_Client::HarvestIncomingMessages(char* pMessages, int iFirstMessage,int iMessagesNum)
{
	char* pTravel = pMessages;

	int iCurrentMsg = 0;
	bool bNewMessageProcessed=false;

	char temp[300];

	while (iCurrentMsg<iMessagesNum)
	{
		bNewMessageProcessed=false;

		switch(pTravel[0])
		{

		// in all of this cases, simply redirect the message into g_pGame
		case GAME_EVENT_PLAYER_DIED_BY_WEAPON:
		case GAME_EVENT_PLAYER_DAMAGE:
		case COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT:
		case GAME_EVENT_PLAYER_LEVEL_DAMAGE: 
		//case SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP:
		case GAME_EVENT_PLAYER_TELEPORTED:
		case COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE:
		case GAME_EVENT_RESPAWN_PLAYER:
		case GAME_EVENT_PLAYER_DIED_BY_LEVEL:
			{
				if (m_iLastRecievedMessage<iFirstMessage+iCurrentMsg) // if this is a message we didn't get yet
				{
					//tPlayerDiedByWeapon_Game* pMsg = (tPlayerDiedByWeapon_Game*) pTravel;				
					//g_pGame->OnEvent(pTravel[0],pMsg);
					g_pGame->OnEvent(pTravel[0],pTravel);
					bNewMessageProcessed = true;
				}
				//pTravel+=sizeof(tPlayerDiedByWeapon_Game);			
				pTravel+=TranslateCombatMessageTypeToSize(pTravel[0]);
			}
			break;
		

		case COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE:
			{
				if (m_iLastRecievedMessage<iFirstMessage+iCurrentMsg) // if this is a message we didn't get yet
				{
					tCombatEventAckLastKnownMessage* pMsg = (tCombatEventAckLastKnownMessage*) pTravel;				
					m_CommBuffUDP.UpdateAck(pMsg->iLastKnownMessage);

					// ack debug message
					//sprintf(temp,"Got ACK for message %i",pMsg->iLastKnownMessage);					
					//g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
					
					bNewMessageProcessed = true;
				}
				//pTravel+=sizeof(tCombatEventAckLastKnownMessage);				
				pTravel+=TranslateCombatMessageTypeToSize(pTravel[0]);
			}
			break;



		default:
			BREAKPOINT(1); // unrecognized message type !!!
		}


		if (bNewMessageProcessed)
			m_iLastRecievedMessage++;
		iCurrentMsg++;
	}


}

void CNetworking_Client::SendPendingMessages_TCP(void)
{
	m_CommBuffTCP.SendPendingMessages(m_StreamingSocket);


}

const char* CNetworking_Client::GetMapName() const
{
	BREAKPOINT(m_MapName[0]==NULL);
	return m_MapName;
}

void CNetworking_Client::AddNewPlayerToPlayersStateVector(void)
{
	char temp[200];
	m_IdsStatesSafeMap.Acquire();

		IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();

		tPlayerGameState* pPlayerGameState = NULL;

		for (IDS_STATES_MAP_IT it=pMap->begin();it!=pMap->end();++it)
		{	
			if (it->second->m_bAddedToClientList)
				continue;
			
			int iConnectID1 = it->first;
			int iConnectID2 = it->second->m_ConnectID;
			sprintf(temp,"%i %i %u",iConnectID1,iConnectID2,it->second->uiEntityID);
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		

			
			
			if (it->second->m_ConnectID == m_cHumanClientConnectID)
				it->second->cControlType = PLAYER_CONTROL_TYPE_HUMAN;
			else
				it->second->cControlType = PLAYER_CONTROL_TYPE_NETWORKED;
				
			// temp - only vagary

			/*it->second->pCharacterType = new char[strlen("MD5/vagary/vagary.chr")+1];
			strcpy(it->second->pCharacterType,"MD5/vagary/vagary.chr");*/
			 
				
			m_PlayersGameState.push_back(it->second);

			it->second->m_bAddedToClientList = true;
			
		}
	
		m_IdsStatesSafeMap.Release();
}

void CNetworking_Client::RecievePendingMessages_TCP(void)
{

	CVec3f vColor(0.f,1.f,1.f);

	m_IdsStatesSafeMap.Acquire();


	IDS_STATES_MAP* pIdsStatesMap = m_IdsStatesSafeMap.GetMap();

	int iCharNum=0;
	for (IDS_STATES_MAP_IT it=pIdsStatesMap->begin();it!=pIdsStatesMap->end();++it)
	{
		int iConnectID = it->first;
		g_pGraphicsAPI->printf(vColor,10,400+30*iCharNum,"name:%s  character:%s   ConnectID:%d    EntityID:%u",
			it->second->pName,it->second->pCharacterType,iConnectID,it->second->uiEntityID);		
		iCharNum++;
	}

	m_IdsStatesSafeMap.Release();



	char* pMessage = NULL;
	int iMessageSize = 0;

	m_CommBuffTCP.RecieveIncoming(m_StreamingSocket);

	int iMessageType = m_CommBuffTCP.HarvestIncomingMessages((char*)pMessage,iMessageSize);

	if (iMessageType<=0)
		return;


	char temp[400];

	
	if (iMessageType==TCP_MSG_CHAT)
	{
		char* pString = (char*) pMessage;		
		int iID = pString[1];

		m_IdsStatesSafeMap.Acquire();

		IDS_STATES_MAP* pMap = m_IdsStatesSafeMap.GetMap();
		IDS_STATES_MAP_IT it = pMap->find(iID);

		if (it!=pMap->end())
		{
			if (iID!=m_cHumanClientConnectID)
				sprintf(temp,"%s: %s",it->second->pName,&pString[3]);
			else
				sprintf(temp,"You: %s",&pString[3]);
		
			if (iID==m_cHumanClientConnectID)
				g_TextMessages.AddMessage(temp,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_YOU);
			else
				g_TextMessages.AddMessage(temp,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);
		}

		m_IdsStatesSafeMap.Release();

	} 	if (iMessageType==TCP_MSG_SERVER_ACK_DATA)
	{
		 tServerAckData* pMsg = (tServerAckData*) pMessage;
		 strcpy(m_MapName,pMsg->mapName);

		sprintf(temp,"Server sets map to: %s",m_MapName);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

		m_cHumanClientConnectID = pMsg->cID;

		sprintf(temp,"Recieved ID=%i from server.",pMsg->cID);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



		//ZeroMemory(&m_RecieveAdress_UDP,sizeof(m_RecieveAdress_UDP));
		// this is my address
		m_RecieveAdress_UDP.sin_family = AF_INET;         // host byte order
		m_RecieveAdress_UDP.sin_port = htons(SERVER_PORT_NUM_UDP+m_cHumanClientConnectID);     // short, network byte order
		m_RecieveAdress_UDP.sin_addr.s_addr =  htonl(INADDR_ANY);
		
		short sPort = SERVER_PORT_NUM_UDP+m_cHumanClientConnectID;
		sprintf(temp,"UDP Listen on port:%u",sPort);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

		
		// make a recieve socket for UDP

		if ((m_RecieveSocket_UDP = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
		{
			sprintf(temp,"UDP: socket() failed!");
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return;
		}

		unsigned long ulYes = 1; 
		if (ioctlsocket(m_RecieveSocket_UDP, FIONBIO, &ulYes) == SOCKET_ERROR) 
		{			
			sprintf(temp,"failed making a udp connection non blocking!");
				
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return;
		}

		//TODO:this bind fails on client
		int iBindResult = bind(m_RecieveSocket_UDP, (SOCKADDR *) &m_RecieveAdress_UDP, sizeof(m_RecieveAdress_UDP));

		if (iBindResult)
		{
			int iError =  WSAGetLastError();

			sprintf(temp,"error in binding UDP listen socket");
				g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				return;
		}


	
		
		
		// this is the server address
		m_SendToAdress_UDP.sin_family = AF_INET;         // host byte order
		m_SendToAdress_UDP.sin_port = htons(SERVER_PORT_NUM_UDP+m_cHumanClientConnectID);     // short, network byte order
		m_SendToAdress_UDP.sin_addr.s_addr =  inet_addr(m_pServerIP);
		memset(&(m_SendToAdress_UDP.sin_zero), '\0', 8); // zero the rest of the struct

		int iPort = SERVER_PORT_NUM_UDP+m_cHumanClientConnectID;
		sprintf(temp,"UDP messages will be sent to IP:%s Port:%i",m_pServerIP,iPort);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

		


		// send hello message to server
		// (that way server will know to associate sender address to Player Entity ID)
		tCombatHelloServer msg(m_cHumanClientConnectID);
		//msg.cSenderConnectionID = m_cHumanClientConnectID;

		sprintf(temp,"Sent hello message to server, containing connectID %u",msg.cSenderConnectionID);
		m_CommBuffUDP.PushSendMessage((char*) &msg,sizeof(tCombatHelloServer));

		

	}
	else if (iMessageType==TCP_MSG_UPDATE_CLIENT_IN_OTHER_CLIENT_JOIN)
	{
		tUpdateClientInOtherClientJoin* pMsg = (tUpdateClientInOtherClientJoin*) pMessage;
		AddIdNameElement(pMsg->cID,pMsg->name,pMsg->characterName);

		sprintf(temp,"Recieved data on [%s]",pMsg->name);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		

		//yoel
		if (g_pGame)
		{
			//AddIdNameElement(
			AddNewPlayerToPlayersStateVector();
			g_pGame->AddPlayers(&m_PlayersGameState);
		}
	}
	else if (iMessageType==TCP_MSG_UPDATE_CLIENT_DISC)
	{
		tUpdateClientDisc* pMsg = (tUpdateClientDisc*) pMessage;
		//AddIdNameElement(pMsg->cID,pMsg->name);
		RemoveIDNameElement(pMsg->cID);

		sprintf(temp,"Client %i Disconnected!",pMsg->cID);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		
	} else if (iMessageType == TCP_MSG_UPDATE_CLIENT_ENTITY_ID)
	{
		tUpdateClientGivenEntityID* pMsg = (tUpdateClientGivenEntityID*) pMessage;
					

		int iConnectID = pMsg->cID;
		sprintf(temp,"Got a entity ID update that links connectID:%d with entityID:%u",iConnectID,pMsg->dwEntity);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		

		tPlayerGameState* pPlayer = GetPlayerStateUsingConnectionID(pMsg->cID);

		int iAssignedConnectID = pPlayer->m_ConnectID;
		sprintf(temp,"assigning it to player with connect ID:%i",iAssignedConnectID);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		

		BREAKPOINT(!pPlayer);		
		pPlayer->uiEntityID = pMsg->dwEntity;
		


	}	
	else if (iMessageType==TCP_MSG_START_GAME)
	{		
		//m_PlayersGameState.clear();

		//split into another function, and call it in the updateclientinotherclient message

				
		sprintf(temp,"Starting Game:");
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		


		int iHumanConnectID = m_cHumanClientConnectID;
		sprintf(temp,"Human Client ConnectID: %d",iHumanConnectID);
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		

		AddNewPlayerToPlayersStateVector();


		g_pMainMenu->Action(MENU_ACTION_PURE_CLIENT_GAME_STARTED);


		// Load Game
		SafeDelete(g_pGame);
		g_pGame = new CGame();
		g_pGame->Load(&m_PlayersGameState);

		m_dwConnectStage = CONNECTION_IN_GAME;

		g_pMainMenu->SetActive(false);

		///////////////////////////////////////////////////////////////////////////

	}

}

