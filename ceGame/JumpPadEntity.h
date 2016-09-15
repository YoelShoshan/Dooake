#ifndef CRIMSON_JUMPPAD_ENTITY_H
#define CRIMSON_JUMPPAD_ENTITY_H

#pragma once

#include "StaticEntity.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;

#include <map>

//#define AFFECT_PLAYER_MIN_DELTA_TIME 500
#define AFFECT_PLAYER_MIN_DELTA_TIME 0

class CJumpPadEntity : public CStaticEntity
{
public:
	CJumpPadEntity();
	~CJumpPadEntity();

	virtual bool RelinkMe(void) { return false; };

	bool TouchedBy(CEntity* pOther);

	void UpdateRenderModel(float fDeltaTime);
	void Render(int iTech);

	void SetDestination(const CVec3f& vDest);
	

private:
	CVec3f m_vDestination;
	CVec3f m_vForce;


	std::map<unsigned int,double> m_TriggeredEntities;

	FSOUND_SAMPLE* m_pSound;	
};




#endif