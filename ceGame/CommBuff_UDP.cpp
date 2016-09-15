#include "stdafx.h"
#include "CommBuff_UDP.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

CCommBuff_UDP::CCommBuff_UDP()
{
	m_uiLastAckedMsgID = -1;
	m_iCurrentMsgID=0;

	// debug
	m_Outgoing.SetPrintDebugInfo();
};

CCommBuff_UDP::~CCommBuff_UDP()
{

};

void CCommBuff_UDP::PushSendMessage(char* pMessageData, int iSize)
{
	m_Outgoing.Push(pMessageData,iSize);

	// here i should update some map with the SIZE of the current message

	m_IdsSizesMap[m_iCurrentMsgID] = iSize;

	m_iCurrentMsgID++;
}

void CCommBuff_UDP::UpdateAck(int iMessageID)
{
	if (iMessageID<m_uiLastAckedMsgID)
		return;

	if (m_IdsSizesMap.empty())
		return;

	/*std::map<int,int>::iterator it=m_IdsSizesMap.find(m_uiLastAckedMsgID+1);

	if (it==m_IdsSizesMap.end())
	{
		int bla=3;
		bla++;
	}*/

	int iSum=0;

	for (std::map<int,int>::iterator it=m_IdsSizesMap.begin();it!=m_IdsSizesMap.end();)
	{
		if (it->first> iMessageID)
			break;

		iSum+=it->second;

		it = m_IdsSizesMap.erase(it);
		continue;

		//++it;
	}


	if (!iSum)
		return;


	m_Outgoing.InformDataSent(iSum);

	m_uiLastAckedMsgID = iMessageID;
}

void CCommBuff_UDP::SendRawData(const SOCKET streamSocket,const sockaddr_in* address,char* pData, int iLength)
{
	int iBytesSent=sendto(streamSocket,pData, iLength, 0,(sockaddr *)address, sizeof (sockaddr_in));

	if (iBytesSent==0)
		return;
	
	if (iBytesSent<0)
	{
		int iError =  WSAGetLastError();
		
		if (iError==WSAEWOULDBLOCK)
			return;

		if (iError==WSAENOTSOCK || iError==WSAECONNABORTED ) // oponent disconnected
		{
			char temp[200];
			sprintf(temp,"connection fail!");
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			     	
		}

		return;
	}
}

int CCommBuff_UDP::SendPendingMessages(const SOCKET streamSocket, const sockaddr_in* address )
{
	// a message in udp is made like this:

	//[type = <pos_and_ang_update> or <game_delta_update>] , [first message ID], [ last message ID], [...Messages...]

	char temp[400];

	char* pData = NULL;
	int iSize = 0;
	int iSize2 = 0;

	bool bReachedCirclarity = m_Outgoing.GetCurrSend(pData,iSize);

	
	*((char*)&m_TempOutgoingBuff[0]) = GAME_DELTA_MESSAGE_UDP; 
	*((int*)&m_TempOutgoingBuff[1])  = m_uiLastAckedMsgID+1;;
	*((int*)&m_TempOutgoingBuff[1+sizeof(int)]) =  m_iCurrentMsgID-1;


	char* pTempAfterHeader = &m_TempOutgoingBuff[1+4+4]; // byte+int+int
	
	memcpy(pTempAfterHeader,pData,iSize);

	if (bReachedCirclarity) // if met buffer end, glue the 2 parts together [2)and this ..... 1)this]
		g_TextMessages.AddMessage("Ring Buffer Reached Circularity!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);


	if (!iSize)
		return 0;


	int iBytesSent=0;
	int iTotalShouldBeSent = iSize+iSize2+1+4+4;

	iBytesSent=sendto(streamSocket,m_TempOutgoingBuff, iTotalShouldBeSent, 0,(sockaddr *)address, sizeof (sockaddr_in));

	if (iBytesSent==0)
		return 0;
	
	if (iBytesSent<0)
	{
		int iError =  WSAGetLastError();
		
		if (iError==WSAEWOULDBLOCK)
			return 0;

		if (iError==WSAENOTSOCK || iError==WSAECONNABORTED ) // oponent disconnected
		{
			char temp[200];
			sprintf(temp,"connection fail!");
			g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			     	
		}

		return iError;
	}

	/*static int s_iID=0;
	sprintf(temp,"%i)   sent %i bytes in udp",s_iID,iBytesSent);
	g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	s_iID++;*/

	return iBytesSent;

}

