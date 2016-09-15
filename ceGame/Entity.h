#ifndef CRIMSON_ENTITY_BASE_H
#define CRIMSON_ENTITY_BASE_H

#pragma once

#include "RenderModel.h"
#include "PhysicsModel.h"
#include <list>
#include "ParticleSystem.h"

typedef std::map<int,int> LEAFS_INDICES_MAP;
typedef std::map<int,int>::iterator LEAFS_INDICES_MAP_I;

class IPhysicsModel;
class CEntity
{
public:
	CEntity();
	virtual ~CEntity();

	

	////// setting/getting member variables
	//void SetEntitiesList(CEntity** ppEntities) { m_ppEntities = ppEntities; };

	// used for determining if new linking is needed
	virtual bool IsStatic(void) { return false;};
	virtual bool IgnoreMeInPhysics(void) { return false; };

	virtual bool RelinkMe(void) { return true; };
	
	virtual bool IsControllable(void) { return false;};
	virtual bool IsProjectile(void) { return false;} ;
	virtual bool IsTeleporter(void) { return false;} ;
	virtual bool IsCollectable(void) { return false;} ;



	virtual bool AffectGamePlay(void) { return true; };




	virtual void TriggerFrameActions(const tFrameActions* pFrameActions);

	void   SetAngles(const CVec3f& vVec){m_pRenderModel->SetAngles(vVec);}
	CVec3f GetAngles(void) { return m_pRenderModel->GetAngles();};
	
	// set/get RenderModel
	void           SetRenderModel(IRenderModel* pRM) { m_pRenderModel = pRM;};
	IRenderModel*  GetRenderModel(void) { return m_pRenderModel;};

	void           SetPhysicsModel(IPhysicsModel* pPM);
	
	IPhysicsModel* GetPhysicsModel(void) { return m_pPhysicsModel;};

	// set/get PhysicsModel

	virtual void FindPhysicsStepDesiredPosition(float fStep);
	virtual void BlendPhysicsState(float fAlpha);

	virtual void UpdateRenderModel(float fDeltaTime,bool bVisible);

	virtual void Render(int iTech);

	// if return value is true, then this encounter should be IGNORED in the physics model
	virtual bool TouchedBy(CEntity* pOther) { return false; };
	
	static void ResetIDCounter(void) { s_uiID = 0;};
	unsigned int GetID(void) { return m_uiID;};

	// used to prevent double rendering in the case that an entity is linked to more then one leaf
	//unsigned int m_uiLastFrameDrawn;

	bool m_bAlreadyDrawnThisFrame;
	bool m_bAlreadyComputedRenderModelStateThisFrame;

	bool GetCleanMe(void) const { return m_bCleanMe; };


	LEAFS_INDICES_MAP* GetLinkedLeafsMap(void);

	void SetCleanMe(void) { m_bCleanMe=true;};

	virtual CVec3f GetCurrentBindPosition(int iBindNum) const;

	void RemovePS(const CParticleSystem* pParticleSystem);

	// only used in networked players
	/*void ResetPhysicsFramesSinceLastUpdateArrived();
	void IncreasePhysicsFramesSinceLastUpdateArrived();
	int  GetPhysicsFramesSinceLastUpdateArrived() const;*/

	bool GetCastShadow(void) const { return m_bCastShadow;};
	void SetCastShadow(bool bSet) { m_bCastShadow = bSet; };


protected:
	//static CEntity** m_ppEntities;  // a pointer array of all entities in the game


	IRenderModel*  m_pRenderModel;
	IPhysicsModel* m_pPhysicsModel;

	std::list<CParticleSystem*> m_ParticleSystems;

	bool m_bCleanMe;

protected:

	bool m_bCastShadow;

	unsigned int m_uiID;

	// In order to make this persistent, splitting the entities given IDs into regions:

	// For a player (controllable) >= 10000
	// For other entities [0,9999]

	static unsigned int s_uiID;

	static unsigned int s_uiPlayerID;

	
	LEAFS_INDICES_MAP m_LinkedLeafs;

	/*int m_iFramesSinceLastUpdateArrived;*/

	
	
};



#endif