#ifndef CRIMSON_BOT_ENTITY_H
#define CRIMSON_BOT_ENTITY_H

#pragma once

#include "ControllableEntity.h"

class CBotEntity : public CControllableEntity
{
public:
	CBotEntity();
	~CBotEntity();


	// ignoring actiong flags.
	void HandleActionRequests(DWORD dwActionFlags, float fDeltaTime);


	void ThinkAndAct(float fDeltaTime);

};






#endif