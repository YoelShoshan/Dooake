#include "stdafx.h"
#include "HurtEntity.h"

#include "ControllableEntity.h"

#include "Server.h"
extern IServer*    g_pServer;

CHurtEntity::CHurtEntity()
{

}

CHurtEntity::~CHurtEntity()
{

}

bool CHurtEntity::TouchedBy(CEntity* pOther)
{
	if (!pOther->IsControllable())
		return false;

	CControllableEntity* pContEntity = (CControllableEntity*) pOther;

	if (pContEntity->GetLife()<=0)
		return false;


	tLevelDamageEvent_Server e;
	e.uiPlayerEntityID = pContEntity->GetID();
	e.uiHurtEntity = GetID();

	g_pServer->OnEvent(SERVER_EVENT_PLAYER_LEVEL_DAMAGE_UDP, &e);
	
	return false;
}