#ifndef CRIMSON_HIRT_ENTITY_H
#define CRIMSON_HIRT_ENTITY_H

#pragma once

#include "StaticEntity.h"




class CHurtEntity : public CStaticEntity
{
public:
	CHurtEntity();
	~CHurtEntity();

	virtual bool RelinkMe(void) { return false; };

	bool TouchedBy(CEntity* pOther);


	void SetDamage(int iDamage) { m_iDamage = iDamage;};
	int  GetDamage(void) const { return m_iDamage;};

	

private:
	int m_iDamage;
};




#endif