#ifndef CRIMSON_SERVER_NETWORKING_CLIENT_H
#define CRIMSON_SERVER_NETWORKING_CLIENT_H

#pragma once

#include "Server.h"

#include <winsock.h>

#include "CommBuff_TCP.h"
#include "CommBuff_UDP.h"



#define CLIENT_POS_AND_ANG_UPDATE_RATE 30 // in ms

// temp
//#define CLIENT_POS_AND_ANG_UPDATE_RATE 100 // in ms

struct tConnectData
{
	tConnectData():pcPlayerName(NULL){};

	char ipHostAdress[16];
	char* pcPlayerName; 
};

class CNetworking_Client : public IServer
{
public:

	CNetworking_Client();
	~CNetworking_Client();

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
	void RecievePendingMessages_UDP(int iDepth);

	virtual const char* GetMapName() const;
	


	void SendMessageToAll_UDP(char* pMessage) {}; // dummy - pure client only communicates with server

	void RemoveIDNameElement(char cID);
	void AddIdNameElement(char cID,char* pcName,char* pcCharacterName);
	

private:

	void AddNewPlayerToPlayersStateVector(void);

	void HarvestIncomingMessages(char* pMessages, int iFirstMessage,int iMessagesNum);

	char* m_pServerIP;

	// tcp
	SOCKET m_ConnectionSocket;
	SOCKET m_StreamingSocket;
	sockaddr_in m_SocketAdress;
	bool m_bJoinRequestSent;
	CCommBuff_TCP m_CommBuffTCP;

	// udp
	SOCKET m_ServerSocket_UDP;
	sockaddr_in  m_SendToAdress_UDP;

	SOCKET m_RecieveSocket_UDP;
	char m_IncomingBuffer[BIGGEST_INCOMING_MESSAGE_SIZE];
	sockaddr_in m_RecieveAdress_UDP;
	sockaddr_in m_TempSenderAdress_UDP;
	CCommBuff_UDP m_CommBuffUDP;


	int m_iLastRecievedMessage;
	int m_iLastAckedMessage;

	char* m_pcName;	

	unsigned int m_uiLastSentPosAndAngID;

};
















#endif