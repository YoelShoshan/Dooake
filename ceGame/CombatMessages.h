#ifndef CRIMSON_COMBAT_MESSAGES_H
#define CRIMSON_COMBAT_MESSAGES_H

#pragma once


#define SERVER_PORT_NUM_UDP 7000

#define NOT_INIT_CONNECTION_ID 255

/////////////////////////////////////////////////////////////
// in game UDP messages
// the convention is: first  = message TYPE
//                    second = message ID


/*1. I need to realise who send me a udp message based on the sender-address filled by recvfrom
2. I need to remove uiPlayerEntityID from all of the message because of #1

perhaps i should change server::OnEvent to also get a uiPlayerEntityID*/

#define COMBAT_HELLO_SERVER 100
struct tCombatHelloServer
{
	tCombatHelloServer(char senderConnectionID):
		cMsgType(COMBAT_HELLO_SERVER),
		cSenderConnectionID(senderConnectionID) {};
	char cMsgType;
	char cSenderConnectionID;
};

#define COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE 101
struct tCombatEventAckLastKnownMessage
{
	tCombatEventAckLastKnownMessage(char senderConnectionID):
		cMsgType(COMBAT_EVENT_ACK_LAST_KNOWN_MESSAGE),
		cSenderConnectionID(senderConnectionID)
		{};
	char cMsgType;
	char cSenderConnectionID;
	int iLastKnownMessage;
};




#define COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL 102 // used internally inside the server
struct tCombatEventClientRequestCollectable_FULL
{
	tCombatEventClientRequestCollectable_FULL():
	cMsgType(COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL){};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	unsigned int uiItemID;
};


// partial is without the requesting player entity id information
// (since it can be achieved freely over UDP and gets filled by "recfrom()")
/*#define COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_PARTIAL 103
struct tCombatEventClientRequestCollectable_PARTIAL 
{
	tCombatEventClientRequestCollectable_PARTIAL():cMsgType(COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_PARTIAL) {};
	char cMsgType;
	//int  iMessageID;

	unsigned int uiItemID;
};*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// missile shooting

//////////////////////////////////////////////////////////////////
// a request from a client to shoot projectile

#define COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT 104
struct tCombatEventClientRequestProjectileShoot	 //request by anyone (including server from itself)
{
	tCombatEventClientRequestProjectileShoot():cMsgType(COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT){};
	char cMsgType;
	//char cSenderConnectionID;
	unsigned int  uiPlayerEntityID;
	unsigned char ucProjectileType;
	float fStartPosition[3];
	float fDirection[3]; // expected to be normalized
};
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// an update from a server about projectile shooting
#define COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT 105
struct tCombatEvent_ServerUpdate_ProjectileShoot
{
	tCombatEvent_ServerUpdate_ProjectileShoot(/*char senderConnectionID*/):
		cMsgType(COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT)/*,
		cSenderConnectionID(senderConnectionID)*/{};
	char cMsgType;
	char cSenderConnectionID;
	unsigned int  uiPlayerEntityID;
	unsigned char ucProjectileType;
	float fStartPosition[3];
	float fDirection[3]; // expected to be normalized
};






#define COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE 115
struct tCombatEventCollectablePickupUpdate
{
	tCombatEventCollectablePickupUpdate():cMsgType(COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	unsigned int uiItemID;
};

#define SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP     116
struct tMetTeleporterEvent_Server
{
	tMetTeleporterEvent_Server():cMsgType(SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	unsigned int uiTeleporterID;
};

// for example, landed in pit
#define SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP       117
struct tLevelDamageEvent_Server
{
	tLevelDamageEvent_Server():cMsgType(SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP) {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	unsigned int uiHurtEntity;
};


#define SERVER_EVENT_PROJECTILE_EXPLOTION_UDP     118
struct tProjectileExpoltionEvent_Server
{
	tProjectileExpoltionEvent_Server():cMsgType(SERVER_EVENT_PROJECTILE_EXPLOTION_UDP)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;


	CVec3f vPos;
	unsigned char cProjectileExplotionType;
	unsigned int  uiShotByPlayerID;
	bool		  bDirectHit;
};








#define GAME_EVENT_PLAYER_TELEPORTED 119
struct tTeleportedEvent_Game
{
	tTeleportedEvent_Game():cMsgType(GAME_EVENT_PLAYER_TELEPORTED)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;


	unsigned int uiPlayerEntityID;
	unsigned int uiTeleporterID;
	CVec3f vTeleportTarget;
};


// for example - falling into pits
#define GAME_EVENT_PLAYER_LEVEL_DAMAGE 120
struct tLevelDamageEvent_Game
{
	tLevelDamageEvent_Game():cMsgType(GAME_EVENT_PLAYER_LEVEL_DAMAGE)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	int iDamage;
};

#define GAME_EVENT_PLAYER_DAMAGE 121
struct tPlayerDamageEvent_Game
{
	tPlayerDamageEvent_Game():cMsgType(GAME_EVENT_PLAYER_DAMAGE)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	int iDamage;
	CVec3f vApplyForce;
};

#define GAME_EVENT_PLAYER_DIED_BY_WEAPON 122
struct tPlayerDiedByWeapon_Game
{
	tPlayerDiedByWeapon_Game():cMsgType(GAME_EVENT_PLAYER_DIED_BY_WEAPON)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID_victom;
	unsigned int uiPlayerEntityID_killer;
};

// for example, died by falling into pit
#define GAME_EVENT_PLAYER_DIED_BY_LEVEL 123
struct tPlayerDiedByLevel_Game
{
	tPlayerDiedByLevel_Game():cMsgType(GAME_EVENT_PLAYER_DIED_BY_LEVEL)/*,iMessageID(-1)*/ {};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
};


#define GAME_EVENT_RESPAWN_PLAYER 124
struct tRespawnPlayer_Game
{
	tRespawnPlayer_Game(char cID):cMsgType(GAME_EVENT_RESPAWN_PLAYER)/*,iMessageID(-1)*/ {cSenderConnectionID=cID;};
	char cMsgType;
	char cSenderConnectionID;

	unsigned int uiPlayerEntityID;
	char cStartPosition;
};




#define COMBAT_EVENT_CLIENT_STATE_UPDATE_PARTIAL 125
struct tClientStateUpdate_Combat_PARTIAL
{
	tClientStateUpdate_Combat_PARTIAL():cMsgType(COMBAT_EVENT_CLIENT_STATE_UPDATE_PARTIAL) {};
	char cMsgType;
	char cSenderConnectionID;
	unsigned int uiID; // "time" of this message

	// position
	float fPosX,fPosY,fPosZ;
	// angles
	float fAngX,fAngY;

	//animation state
	unsigned char cMoveAnima;

};




#define COMBAT_EVENT_CLIENT_STATE_UPDATE_FULL 126
struct tClientStateUpdate_Combat_FULL
{
	tClientStateUpdate_Combat_FULL():cMsgType(COMBAT_EVENT_CLIENT_STATE_UPDATE_FULL) {};
	char cMsgType;
	char cSenderConnectionID;
	unsigned int uiPlayerEntityID;
	unsigned int uiID; // "time" of this message

	// position
	float fPosX,fPosY,fPosZ;
	// angles
	float fAngX,fAngY;

	//animation state
	unsigned char cMoveAnima;
};





#endif