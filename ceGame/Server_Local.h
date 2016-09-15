#ifndef CRIMSON_ENGINE_LOCAL_SERVER
#define CRIMSON_ENGINE_LOCAL_SERVER

#include "server.h"

#pragma once

class CServer_Local : public IServer
{
public:
	CServer_Local();
	~CServer_Local();

	void OnEvent(DWORD dwEventID,void* pEventData);

	// only for now, since i don't have death animation, and can't trigger respawning from inside an md5 frames
	void Think(void);


	// pData ignored
	bool Connect(void* pData);

	bool HandShake(void);

	void SendMessageToAll_UDP(char* pMsg) {}; // dummy

	
	void SendChatMessage(const char* pMessage,char cID);	
	void SendPendingMessages_TCP(void) {};
	void RecievePendingMessages_TCP(void) {};
	void SendPendingMessages_UDP(void) {};
	void RecievePendingMessages_UDP(void) {};

	const char* GetMapName() const;

private:

	

	/*void HandlePlayerDeath(unsigned int uiPlayerID);*/

};



#endif