#ifndef CRIMSON_COMMUNICATION_BUFFER_TCP_H
#define CRIMSON_COMMUNICATION_BUFFER_TCP_H

#pragma once

#include "RingBuffer.h"

#include <winsock.h>

#define INCOMING_DATA_BUFFER_SIZE 0xFFFF
#define BIGGEST_INCOMING_MESSAGE_SIZE 2048


#define NO_COMPLETE_INCOMING_MESSAGE -1

class CCommBuff_TCP
{
public:
	CCommBuff_TCP();
	~CCommBuff_TCP();

	// incoming
	int RecieveIncoming(SOCKET streamSocket);
	int  HarvestIncomingMessages(char*& pIncomingMessage, int& iSize );

	// outgoing
	void PushSendMessage(const char* pMessageData, int iSize);
	int SendPendingMessages(SOCKET streamSocket);


private:

	void HarvestRegularMessage(int iSize);

	CRingBuffer m_Outgoing;

	int  m_iPendeingIncomingSize;
	char m_IncomingData[INCOMING_DATA_BUFFER_SIZE];

	char m_CurrIncomingMessage[BIGGEST_INCOMING_MESSAGE_SIZE];

	char m_TempOutgoingBuff[BIGGEST_INCOMING_MESSAGE_SIZE];
};


#endif