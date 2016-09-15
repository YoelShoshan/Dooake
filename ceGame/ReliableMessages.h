#ifndef CRIMSON_RELIABLE_MESSAGES_H
#define CRIMSON_RELIABLE_MESSAGES_H

#pragma once

#define MAX_MAP_NAME_LENGTH 100
#define MAX_CHARACTER_NAME_LENGTH 100
#define MAX_CLIENT_NAME_LENGTH 100

#define SERVER_PORT_NUM_TCP 6000
#define SERVER_MAX_CLIENTS_ACCEPTED 32
#define FIRST_CONNECTION_MAX_TIME 600000

#define TCP_MSG_JOIN_REQUEST 1
struct tClientJoinRequest
{
	tClientJoinRequest():cMsgType(TCP_MSG_JOIN_REQUEST) {};

	char cMsgType;
	char name[MAX_CLIENT_NAME_LENGTH];
	char characterName[MAX_CHARACTER_NAME_LENGTH];
	//char 
};


#define TCP_MSG_SERVER_ACK_DATA 2
struct tServerAckData
{
	tServerAckData():cMsgType(TCP_MSG_SERVER_ACK_DATA) {};

	char cMsgType;
	char mapName[MAX_MAP_NAME_LENGTH];
	char cID;

};

// a chat message is:
// [byte: msg_code][byte:sender_id][byte:string size][message string ...]

#define TCP_MSG_CHAT            3


#define TCP_MSG_UPDATE_CLIENT_IN_OTHER_CLIENT_JOIN    4
struct tUpdateClientInOtherClientJoin
{
	tUpdateClientInOtherClientJoin():cMsgType(TCP_MSG_UPDATE_CLIENT_IN_OTHER_CLIENT_JOIN) {};

	char cMsgType;
	char cID;
	char name[MAX_CLIENT_NAME_LENGTH];	
	char characterName[MAX_CHARACTER_NAME_LENGTH];
};

#define TCP_MSG_UPDATE_CLIENT_DISC    5
struct tUpdateClientDisc
{
	tUpdateClientDisc():cMsgType(TCP_MSG_UPDATE_CLIENT_DISC) {};
	char cMsgType;
	char cID;
};

#define TCP_MSG_START_GAME 6
struct tUpdateClientStartGame
{
	tUpdateClientStartGame():cMsgType(TCP_MSG_START_GAME) {};
	char cMsgType;
};

#define TCP_MSG_UPDATE_CLIENT_ENTITY_ID 7
struct tUpdateClientGivenEntityID
{
	tUpdateClientGivenEntityID():cMsgType(TCP_MSG_UPDATE_CLIENT_ENTITY_ID) {};

	char cMsgType;
	char cID;
	DWORD dwEntity;

};

#define INFORM_START_GAME 8 // used internally inside real server


















#endif