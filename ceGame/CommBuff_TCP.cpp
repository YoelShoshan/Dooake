#include "stdafx.h"
#include "CommBuff_TCP.h"

#include "Server.h"



CCommBuff_TCP::CCommBuff_TCP()
{
	m_iPendeingIncomingSize = 0;

}

CCommBuff_TCP::~CCommBuff_TCP()
{

}
void CCommBuff_TCP::HarvestRegularMessage(int iSize)
{
	// copy message into temp message
	memcpy(m_CurrIncomingMessage,m_IncomingData,iSize);

	m_iPendeingIncomingSize-=iSize;

	if (m_iPendeingIncomingSize>0)
		memmove(m_IncomingData,&m_IncomingData[iSize],m_iPendeingIncomingSize);

}

int CCommBuff_TCP::HarvestIncomingMessages(char*& pIncomingMessage, int& iSize )
{
	if (!m_iPendeingIncomingSize)
		return 0;

	int iStringLength = -1;
	int iTotalMsgLength = -1;

	int iMessageType = m_IncomingData[0];

	switch (iMessageType)
	{

	case TCP_MSG_CHAT:

		// if not enough 
		if (m_iPendeingIncomingSize<3)
			return NO_COMPLETE_INCOMING_MESSAGE;

		iStringLength = m_IncomingData[2];
		iTotalMsgLength = iStringLength+3;

		if (m_iPendeingIncomingSize<iTotalMsgLength)
			return NO_COMPLETE_INCOMING_MESSAGE;

		// if we're here then the entire string is available

		// copy message into temp message
		memcpy(m_CurrIncomingMessage,m_IncomingData,iTotalMsgLength);

		m_iPendeingIncomingSize-=iTotalMsgLength;

		if (m_iPendeingIncomingSize>0)
			memmove(m_IncomingData,&m_IncomingData[iTotalMsgLength],m_iPendeingIncomingSize);

		iSize = iTotalMsgLength;
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	case TCP_MSG_JOIN_REQUEST:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tClientJoinRequest))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tClientJoinRequest));

		iSize = sizeof(tClientJoinRequest);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	case TCP_MSG_SERVER_ACK_DATA:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tServerAckData))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tServerAckData));

		iSize = sizeof(tServerAckData);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	case TCP_MSG_UPDATE_CLIENT_IN_OTHER_CLIENT_JOIN:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tUpdateClientInOtherClientJoin))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tUpdateClientInOtherClientJoin));

		iSize = sizeof(tUpdateClientInOtherClientJoin);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	case TCP_MSG_UPDATE_CLIENT_DISC:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tUpdateClientDisc))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tUpdateClientDisc));

		iSize = sizeof(tUpdateClientDisc);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;


	case TCP_MSG_START_GAME:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tUpdateClientStartGame))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tUpdateClientStartGame));

		iSize = sizeof(tUpdateClientStartGame);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	case TCP_MSG_UPDATE_CLIENT_ENTITY_ID:

		// if not enough 
		if (m_iPendeingIncomingSize<sizeof(tUpdateClientGivenEntityID))
			return NO_COMPLETE_INCOMING_MESSAGE;

		HarvestRegularMessage(sizeof(tUpdateClientGivenEntityID));

		iSize = sizeof(tUpdateClientGivenEntityID);
		pIncomingMessage = m_CurrIncomingMessage;

		return iMessageType;

		break;

	default:
		BREAKPOINT(1); // unreocgnized message type!

	};


	return false;
}

int CCommBuff_TCP::RecieveIncoming(SOCKET streamSocket)
{
	int iRecievedBytes = recv(streamSocket,&m_IncomingData[m_iPendeingIncomingSize], INCOMING_DATA_BUFFER_SIZE - m_iPendeingIncomingSize ,0); 		
	
	if (iRecievedBytes==0)
		return 0;
	else if (iRecievedBytes<0)
	{
		int iError =  WSAGetLastError();
		
		if (iError==WSAEWOULDBLOCK)
			return 0;

		if (iError==WSAENOTSOCK || iError==WSAECONNABORTED ) // oponent disconnected
		{
			     	
		}

		return iError;
	}
	else // iRecievedBytes>0
	{
		m_iPendeingIncomingSize+=iRecievedBytes;
	}

	return 0;
}

void CCommBuff_TCP::PushSendMessage(const char* pMessageData, int iSize)
{
	m_Outgoing.Push(pMessageData,iSize);

}

int  CCommBuff_TCP::SendPendingMessages(SOCKET streamSocket)
{
	char* pData = NULL;
	int iSize = 0;
	int iSize2 = 0;

	bool bReachedCirclarity = m_Outgoing.GetCurrSend(pData,iSize);

	/*if (bReachedCirclarity) // if met buffer end, glue the 2 parts together [2)and this ..... 1)this]
	{
		//pData = m_TempBuff;

		memcpy(m_TempOutgoingBuff,pData,iSize);

		int iSize2 = -1;
		m_Outgoing.GetCurrSend(pData,iSize2); // get the second buffer (that will be in the begining of the buffer)
		
		memcpy(&m_TempOutgoingBuff[iSize],pData,iSize2);

		pData = m_TempOutgoingBuff;
	}*/

	if (!iSize)
		return 0;

	int iBytesSent = send(streamSocket,pData,iSize+iSize2,0);

	if (iBytesSent==0)
		return 0;
	else
	if (iBytesSent<0)
	{
		int iError =  WSAGetLastError();
		
		if (iError==WSAEWOULDBLOCK)
			return 0;

		if (iError==WSAENOTSOCK || iError==WSAECONNABORTED ) // oponent disconnected
		{
			     	
		}

		return iError;
	}

	m_Outgoing.InformDataSent(iBytesSent);

	return iBytesSent;
}