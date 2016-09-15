#ifndef CRIMSON_NETWORKED_PLAYER_ENTITY_H
#define CRIMSON_NETWORKED_PLAYER_ENTITY_H

#pragma once

#include "ControllableEntity.h"

class CNetPlayerEntity : public CControllableEntity
{
public:
	CNetPlayerEntity();
	~CNetPlayerEntity();

	
	void HandleActionRequests(DWORD dwActionFlags, float fDeltaTime);

	virtual bool AffectGamePlay(void) { return false; };


	void TriggerAppropriateAnimation(float fDeltaTime);

	// implements the dead reckoning
	// TODO: change name...

	void DeadReckoning(void);

private:

	bool m_bAttackingLoopable_attack1;
	bool m_bAttackingLoopable_attack2;

	bool m_bRotatingLeftInPlace;
	bool m_bRotatingRightInPlace;


	/*CVec3f m_vLastKnownVelocity;
	CVec3f m_vLastKnown;*/
};


#endif