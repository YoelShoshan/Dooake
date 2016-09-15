#include "stdafx.h"
#include "TextMessages.h"


#include <stdio.h>

#include "CTimer.h"
#include "Singleton.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;


#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;
	

CTextMessages::CTextMessages()
{
	for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES;i++)
		ZeroMemory(&Messages[i].caMessage[0],MESSAGE_SIZE_MAX);		

	m_fLastMessageTime=-1.f;
	m_iActiveMessages=0;
		
	/*sprintf(&Messages[0].caMessage[0],"First Message");
	sprintf(&Messages[1].caMessage[0],"Second Message");
	sprintf(&Messages[2].caMessage[0],"Third Message");
	sprintf(&Messages[3].caMessage[0],"Fourth Message");
	sprintf(&Messages[4].caMessage[0],"Fifth Message");

	m_iActiveMessages=5;*/
	
	sprintf(m_PlayerInfo.caMessage,"");
	m_PlayerInfo.r = 1.f;m_PlayerInfo.g = 1.f;m_PlayerInfo.b = 1.f;
	m_PlayerInfo.iMessageType = TEXT_MESSAGE_TYPE_GAME;

	sprintf(m_PlayersScore.caMessage,"");
	m_PlayersScore.r = 1.f;m_PlayersScore.g = 1.f;m_PlayersScore.b = 1.f;
	m_PlayersScore.iMessageType = TEXT_MESSAGE_TYPE_GAME;
	
}

void CTextMessages::SetPlayerInfo(char* cpMessage)
{
	strcpy_s(m_PlayerInfo.caMessage,MESSAGE_SIZE_MAX,cpMessage);
}

void CTextMessages::SetPlayersScore(char* cpMessage)
{
	strcpy_s(m_PlayersScore.caMessage,MESSAGE_SIZE_MAX,cpMessage);
}

void CTextMessages::DrawMessages(void)
{

	char temp[400];

	CVec3f vColor = CVec3f(1.f,1.f,1.f);

	for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES;i++)
	if (Messages[i].caMessage[0]!=NULL)
	{
		vColor.Set(1.f,1.f,1.f);

		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_SYSTEM)
		{
			sprintf(temp,"Sys: %s",&Messages[i].caMessage[0]);
			vColor.Set(1.f,0.f,0.f);
			
		}

		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_GAME)
		{
			sprintf(temp,"Game: %s",&Messages[i].caMessage[0]);
			//vColor.Set(0.f,1.f,0.f);		
			vColor.Set(Messages[i].r,Messages[i].g,Messages[i].b);
			
		}


		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_CHAT_YOU)
		{
			sprintf(temp,"%s",&Messages[i].caMessage[0]);
			vColor.Set(0.f,0.f,1.f);		
		}


		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_CHAT_OPONENT)
		{
			sprintf(temp,"%s",&Messages[i].caMessage[0]);
			vColor.Set(0.f,1.f,1.f);					
		}
		
		g_pGraphicsAPI->printf(vColor,40,30+25*i,temp);		

		//g_Fonts.glPrint(&Messages[i].caMessage[0]);	// Print GL Text To The Screen			
	}

	vColor.Set(m_PlayerInfo.r,m_PlayerInfo.g,m_PlayerInfo.b);
	g_pGraphicsAPI->printf(vColor,40,g_pGraphicsAPI->GetResY()-150,m_PlayerInfo.caMessage);	

	vColor.Set(m_PlayersScore.r,m_PlayersScore.g,m_PlayersScore.b);
	g_pGraphicsAPI->printf(vColor,40,g_pGraphicsAPI->GetResY()-120,m_PlayersScore.caMessage);	

}

void CTextMessages::UpdateMessages(void)
{
	if (m_iActiveMessages<1)
		return;

	if (g_pTimer->GetTime()-m_fLastMessageTime>MESSAGE_TIMEOUT)
	{		
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
				 
		}

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);
		

		m_fLastMessageTime=g_pTimer->GetTime();
		m_iActiveMessages--;
	}

}

void CTextMessages::AddMessage(char* cpMessage, float r, float g, float b,int iMessageType)
{
	g_pLogFile->OutPutPlainText(cpMessage,LOG_MESSAGE_INFORMATIVE);

	if (!cpMessage)
		return;

	if (strlen(cpMessage)>MESSAGE_SIZE_MAX-1)
		return;

	if (m_iActiveMessages>TEXT_MESSAGES_MAX_MESSAGES-1)
	{
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
		}

		

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);		
		

		m_iActiveMessages--;
	}
	

	m_fLastMessageTime=g_pTimer->GetTime();
	Messages[m_iActiveMessages].r = r;
	Messages[m_iActiveMessages].g = g;
	Messages[m_iActiveMessages].b = b;
	Messages[m_iActiveMessages].iMessageType = iMessageType;

	strcpy(&Messages[m_iActiveMessages++].caMessage[0],cpMessage);	


	//g_LogFile.OutPutPlainText(cpMessage,"3","000099",true);

}

void CTextMessages::AddMessage(char* cpMessage)
{
	g_pLogFile->OutPutPlainText(cpMessage,LOG_MESSAGE_WARNING_LVL_3);

	if (!cpMessage)
		return;

	if (strlen(cpMessage)>MESSAGE_SIZE_MAX-1)
		return;

	if (m_iActiveMessages>TEXT_MESSAGES_MAX_MESSAGES-1)
	{
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
		}

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);		
		

		m_iActiveMessages--;
	}
	

	m_fLastMessageTime=g_pTimer->GetTime();
	Messages[m_iActiveMessages].r = 0.f;
	Messages[m_iActiveMessages].g = 1.f;
	Messages[m_iActiveMessages].b = 0.f;
	Messages[m_iActiveMessages].iMessageType = TEXT_MESSAGE_TYPE_CHAT_YOU;


	strcpy(&Messages[m_iActiveMessages++].caMessage[0],cpMessage);	

	//g_LogFile.OutPutPlainText(cpMessage,"3","000099",true);
	
}