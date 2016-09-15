#ifndef CRIMSON_COMMUNICATION_BUFFER_UDP_H
#define CRIMSON_COMMUNICATION_BUFFER_UDP_H

#pragma once

#include <map>
/*struct tUdpMapElem
{
	unsigned short uiMessagePos;
	unsigned short uiMessageSize;
};*/

//typedef std::map<unsigned int uiMessageID

#define BIGGEST_INCOMING_MESSAGE_SIZE 2048


// after the 1 byte header that describes the message as game delta
// [first message num][last message num][...messages...]
#define GAME_DELTA_MESSAGE_UDP                     10

// // after the 1 byte header that describes the message as a pos and ang client-updates-server message
// [simply a tClientStateUpdate_Combat_PARTIAL]
#define POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP    20 // client updates the server with this

// // after the 1 byte header that describes the message as a pos and ang server-updates-clients
// [...a sequence of tClientStateUpdate_Combat_FULL's...] - number of messages will be computed from the incoming bytes num...
#define OTHER_CLIENT_POS_AND_ANG_CONSTANT_UPDATE_MESSAGE_UDP    30 // server updates clients with this

#include "RingBuffer.h"
#include <winsock.h>

#define NO_ACKS_YET -1

class CCommBuff_UDP
{
public:
	CCommBuff_UDP();
	~CCommBuff_UDP();

	// outgoing
	void PushSendMessage(char* pMessageData, int iSize);
	int SendPendingMessages(const SOCKET streamSocket, const sockaddr_in* address);

	void UpdateAck(int iMessageID);



	// simply sends data, not related to the ring buffer
	void SendRawData(const SOCKET streamSocket,const sockaddr_in* address,char* pData, int iLength);


private:

	void FormatOutgoingBuff(void);

	// outgoing messages

	
	CRingBuffer m_Outgoing;
	std::map<int,int> m_IdsSizesMap;

	int m_uiLastAckedMsgID;
	int m_iCurrentMsgID;

	//int  m_iActiveTempOutgoingBuff_Size;
	char m_TempOutgoingBuff[BIGGEST_INCOMING_MESSAGE_SIZE];

	char m_TempSingleMessageBuff[BIGGEST_INCOMING_MESSAGE_SIZE];

	/*int  m_iActiveTempOutgoingBuff_Formmated_Size;
	char m_TempOutgoingBuff_Formatted[BIGGEST_INCOMING_MESSAGE_SIZE];*/

	// recieved messages
};


#endif