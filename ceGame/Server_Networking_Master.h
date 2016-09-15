#ifndef CRIMSON_SERVER_NETWORKING_MASTER_H
#define CRIMSON_SERVER_NETWORKING_MASTER_H

#pragma once

#include "Server.h"

#include <winsock.h>
#include "SenderAddress.h"

#include "SafeMap.h"

// for a server + client

typedef std::map<SOCKET,tPlayerGameState*> SOCKETS_GAME_STATES_MAP;
typedef std::map<SOCKET,tPlayerGameState*>::iterator SOCKETS_GAME_STATES_MAP_IT;


typedef std::map<CSenderAddress,char> SENDER_ADRESS_VS_CONNECT_ID_MAP;
typedef std::map<CSenderAddress,char>::iterator SENDER_ADRESS_VS_CONNECT_ID_MAP_IT;

#define SERVER_UPDATES_CLIENT_POS_AND_ANG_RATE 30 // in ms

struct tPassToConnectThread
{
	tPassToConnectThread():bKillMe(false) {};
	SOCKET* pConnectionSocket;
	CSafeMap<SOCKET,tPlayerGameState*>* pClientSocketsSafeMap;
	bool bKillMe;
};


DWORD AcceptConnections(tPassToConnectThread *pParam);



class CNetworking_Master : public IServer
{
public:

	CNetworking_Master();
	~CNetworking_Master();

	void Think(void);
	void OnEvent(DWORD dwEventID,void* pEventData);
	bool Connect(void* pData);
	bool HandShake(void);

	
	// TCP messages
	void SendChatMessage(const char* pMessage,char cID);
	void SendPendingMessages_TCP(void);
	void RecievePendingMessages_TCP(void);

	// UDP messages
	void SendPendingMessages_UDP(void);
	void RecievePendingMessages_UDP(void);
	void RecievePendingMessages_UDP(int iDepth, SOCKET& socket);

	virtual const char* GetMapName() const;
	

	char AddIdNameElement(char* pcName, char* pcCharacterName);
	//void PushSendMessageToAllClients_TCP(char* pMessageData, int iSize);

private:

	void UpdatePlayersList();

	void InformPlayersOnGameStart();

	void HarvestIncomingMessages(char* pMessages, int iFirstMessage,int iMessagesNum,char cSenderConnectID);

	void SendMessageToAll_UDP_Except(unsigned int uiExceptEntityID, char* pMessage);

	/*// send a general message in UDP, not active
	void Send_RawData_MessageToAll_UDP_Except(unsigned int uiExceptEntityID, char* pData, int iLength);*/

	void SendMessageToAll_UDP(char* pMessage);

	

	void UpdateClientsInClientDisc(void);

	bool CreateUDPSocket(SOCKET&  sock, char connection_id);

	std::vector<char> m_ClientsDisconnected;
	
	// tcp
	SOCKET* m_pConnectionSocket_TCP;
	CSafeMap<SOCKET,tPlayerGameState*> m_PlayersDataSafeMap;
	sockaddr_in m_SocketAdress;
	HANDLE m_hConnectionThread;
	DWORD  m_dwConnectionThreadId;
	bool m_bConnectionThreadStarted;

	// udp
	//SOCKET m_ServerSocket_UDP;
	//sockaddr_in  m_UDPRecieveAdress;
	sockaddr_in  m_TempSenderMessageAddr;

	char m_IncomingBuffer[BIGGEST_INCOMING_MESSAGE_SIZE];


	

	SENDER_ADRESS_VS_CONNECT_ID_MAP m_SenderConnectIdMap;

	tPassToConnectThread* m_pParam;


	////////// update all client in pos+ang
	double m_dTimeTillNextUpdateClientsInPosAndAng;
	char  m_UpdateClientInOtherClientsState[1000];
	double m_dTimeTillGameStart;


	//int m_iPlayersThatSaidHello;

	unsigned int m_uiLastSentPosAndAngID;
};
















#endif