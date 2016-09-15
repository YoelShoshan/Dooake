#ifndef CRIMSON_HUMAN_PLAYER_ENTITY_H
#define CRIMSON_HUMAN_PLAYER_ENTITY_H

#pragma once

#include "ControllableEntity.h"

class CHumanPlayerEntity : public CControllableEntity
{
public:
	CHumanPlayerEntity();
	~CHumanPlayerEntity();



	void HandleActionRequests(DWORD dwActionFlags, float fDeltaTime);
private:

	int m_iTimesSentAttackInfo_Attack1;
	int m_iTimesSentAttackInfo_Attack2;

	
};






#endif