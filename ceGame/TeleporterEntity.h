#ifndef CRIMSON_TELEPORTER_ENTITY_H
#define CRIMSON_TELEPORTER_ENTITY_H

#pragma once

#include "StaticEntity.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;

class CTeleporterEntity : public CStaticEntity
{
public:
	CTeleporterEntity();
	~CTeleporterEntity();

	bool IsTeleporter(void) { return true;} ;

	void   SetDestination(const CVec3f& vDest) { m_vDestination = vDest;};
	CVec3f GetDestination(void) { return m_vDestination; };
			

	void SetRotYAngle(float fAngY) { m_fAngY = fAngY;};
	float GetRotYAngle(void) const { return m_fAngY; };

	bool TouchedBy(CEntity* pOther);

private:
	CVec3f m_vDestination;
	float m_fAngY;

	FSOUND_SAMPLE* m_pSound;
};




#endif