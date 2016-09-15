#ifndef YOEL_TEXT_MESSAGES_H
#define YOEL_TEXT_MESSAGES_H

#pragma once

#define TEXT_MESSAGES_MAX_MESSAGES 7
//#define TEXT_MESSAGES_MAX_MESSAGES 15
#define MESSAGE_SIZE_MAX           200


#define MESSAGE_TIMEOUT            3500 

//#define MESSAGE_TIMEOUT            15000
//#define MESSAGE_TIMEOUT              25000
//#define MESSAGE_TIMEOUT            55000

//#define MESSAGE_TIMEOUT            1000

#define TEXT_MESSAGE_TYPE_CHAT_YOU     0
#define TEXT_MESSAGE_TYPE_CHAT_OPONENT 1
#define TEXT_MESSAGE_TYPE_COMMAND      2
#define TEXT_MESSAGE_TYPE_SYSTEM       3
#define TEXT_MESSAGE_TYPE_GAME         4

struct sMessage
{
	int iMessageType;
	float r,g,b;
	char caMessage[MESSAGE_SIZE_MAX];
};

class CTextMessages
{
public:

	CTextMessages();
	~CTextMessages() {};

	void UpdateMessages(void);
	void DrawMessages(void);
	
	void AddMessage(char* cpMessage);
	void AddMessage(char* cpMessage, float r, float g, float b,int iMessageType);

	void SetPlayerInfo(char* cpMessage);
	void SetPlayersScore(char* cpMessage);

	// incomging messages
	sMessage Messages[TEXT_MESSAGES_MAX_MESSAGES];
	float m_fLastMessageTime;
	int   m_iActiveMessages;

	// static text showing player info
	sMessage m_PlayerInfo;
	sMessage m_PlayersScore;

};



#endif // YOEL_TEXT_MESSAGES_H