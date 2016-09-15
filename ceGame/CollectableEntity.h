#ifndef CRIMSON_COLLECTABLE_ENTITY_H
#define CRIMSON_COLLECTABLE_ENTITY_H

#pragma once

#include "StaticEntity.h"

#define COLLECTABLE_ITEM_TYPE_NOT_INIT     0
#define COLLECTABLE_ITEM_TYPE_HEALTH       1
#define COLLECTABLE_ITEM_TYPE_ARMOR_SHARD  2
#define COLLECTABLE_ITEM_TYPE_ARMOR_BODY   3
#define COLLECTABLE_ITEM_TYPE_ARMOR_COMBAT 4
#define COLLECTABLE_ITEM_TYPE_AMMO_1       5
#define COLLECTABLE_ITEM_TYPE_AMMO_2       6
#define COLLECTABLE_ITEM_TYPE_AMMO_3       7


class CCollectableEntity : public CStaticEntity
{
public:
	CCollectableEntity();
	~CCollectableEntity();

	void Init(DWORD dwType,const char* pcModel, const char* pcMaterial,const CVec3f& vOrigin);

	bool TouchedBy(CEntity* pOther);

	bool IsCollectable(void) { return true;} ;

	virtual bool IgnoreMeInPhysics(void) { return m_bCollected; };

	void PickedUp(void);

	void UpdateRenderModel(float fDeltaTime,bool bVisible);
	void Render(int iTech);

	DWORD GetItemType(void);

	bool GetCollected(void) const { return m_bCollected;};

private:
	bool m_bCollected;
	bool m_bCollectRequested;
	float m_fTimeTillReAppearance;

	DWORD m_dwItemType;
};




#endif