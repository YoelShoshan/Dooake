#ifndef CRIMSON_SERVER_INTERFACE_H
#define CRIMSON_SERVER_INTERFACE_H

#pragma once

#include "ce.h"

#include "Math3d.h"

// for a pure client
#include <map>

#include "SafeMap.h"


#include "ReliableMessages.h"
#include "CombatMessages.h"


#define PLAYER_CONTROL_TYPE_NOT_INIT    0
#define PLAYER_CONTROL_TYPE_HUMAN       1
#define PLAYER_CONTROL_TYPE_BOT         2
#define PLAYER_CONTROL_TYPE_NETWORKED   3

#include "CommBuff_TCP.h"
#include "CommBuff_UDP.h"

struct tPlayerGameState
{
	tPlayerGameState():m_bAddedToClientList(false),m_bEntityCreated(false),m_bReceivedCombatHello(false),m_bAddedToServerList(false),m_bInformedGameStart(false),uiLastRecievedPosAndAngID(0),m_UDPSocket(NULL),m_iLastAckedMessage(-1),m_iLastMessageServerRecievedFrom(-1),dTimeDying(0.0),bDying(false),iScore(0),pName(NULL),pCharacterType(NULL),cControlType(PLAYER_CONTROL_TYPE_NOT_INIT){};
	~tPlayerGameState() 
	{ 
		SafeDeleteArray(pName); 
		SafeDeleteArray(pCharacterType);
		if (m_UDPSocket) 
			closesocket(m_UDPSocket);
	};
	int iScore;
	char* pName;
	char* pCharacterType;
	unsigned int uiEntityID;
	char cControlType;
	bool bDying;
	double dTimeDying;

	char m_ConnectID;

	unsigned char m_clientIP[4];

	CCommBuff_TCP m_CommBuffTCP;
	CCommBuff_UDP m_CommBuffUDP;

	int m_iLastMessageServerRecievedFrom;
	int m_iLastAckedMessage;

	unsigned int uiLastRecievedPosAndAngID;
	// udp
	SOCKET      m_UDPSocket; //send
	sockaddr_in m_UDPAdress;

	SOCKET		m_UDPSocket_Receive;

	bool m_bAddedToServerList;
	bool m_bAddedToClientList;
	bool m_bInformedGameStart;
	bool m_bReceivedCombatHello; // server received combat hello from this client
	bool m_bEntityCreated;
};

typedef std::map<char,tPlayerGameState*> IDS_STATES_MAP;
typedef std::map<char,tPlayerGameState*>::iterator IDS_STATES_MAP_IT;

#include <vector>

#define SERVER_GAME_TYPE_LOCAL                       0
#define SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT       1
#define SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT 2


#define CONNECTION_NOT_INIT                0
#define CONNECTION_HANDSHAKING             1
#define CONNECTION_WAITING_FOR_GAME_START  2
#define CONNECTION_IN_GAME                 3

class IServer
{
public:
	IServer() {m_MapName[0] = NULL;m_cHumanClientConnectID=0xFF;m_uiHumanControlledID=0xFFFFFFFF;};
	virtual ~IServer() {};

	// right now it handles the respawning of players after the death "animation"
	virtual void Think(void) = 0;

	virtual void OnEvent(DWORD dwEventID,void* pEventData) = 0;
	
	virtual void SendMessageToAll_UDP(char* pMessage) = 0;

	virtual void ShowPlayersStates();

	virtual const char* GetMapName() const = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////

	void Event_Projectile_Explosion(void* pEventData);
	void Event_Player_Level_Damage(void* pEventData);
	void Event_Player_Met_Teleporter(void* pEventData);
	

	////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool Connect(void* pData) = 0;

	virtual bool HandShake(void) = 0;

	/////// tcp
	virtual void SendChatMessage(const char* pMessage,char cID) = 0;
	virtual void SendPendingMessages_TCP(void) = 0;
	virtual void RecievePendingMessages_TCP(void) = 0;

	////// udp
	virtual void SendPendingMessages_UDP(void) = 0;
	virtual void RecievePendingMessages_UDP(void) = 0;

	int TranslateCombatMessageTypeToSize(char cMessageType);


	tPlayerGameState* GetPlayerStateFromFinalVector(unsigned int uiEntityID);
	tPlayerGameState* GetPlayerStateFromFinalVector_ByConnectID(char cConnectID);


	tPlayerGameState* GetPlayerStateUsingConnectionID(char cID);

	std::vector<tPlayerGameState*> m_PlayersGameState;

	DWORD m_dwGameType;


	DWORD m_dwConnectStage;

	char m_MapName[MAX_MAP_NAME_LENGTH];

	//IDS_NAMES_MAP m_IdsNames;
	CSafeMap<char,tPlayerGameState*> m_IdsStatesSafeMap;
	
	char m_cHumanClientConnectID;

	unsigned int m_uiHumanControlledID;
};



#endif