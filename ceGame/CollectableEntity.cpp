#include "stdafx.h"
#include "CollectableEntity.h"

#include "ControllableEntity.h"

#include "Server.h"
extern IServer*    g_pServer;

#include "StaticModelManager.h"

#include "StaticBasicPhysics.h"

#include "EntitiesParser.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "StaticModel.h"

CCollectableEntity::CCollectableEntity()
{
	m_bCollected = false;
	m_bCollectRequested = false;

	m_dwItemType = COLLECTABLE_ITEM_TYPE_NOT_INIT;
}

CCollectableEntity::~CCollectableEntity()
{

}

void CCollectableEntity::Init(DWORD dwType,const char* pcModel, const char* pcMaterial,const CVec3f& vOrigin)
{
	m_bCollected = false;
	m_bCollectRequested = false;
	m_dwItemType = dwType;	

	CStaticModel* pRM = new CStaticModel();
	pRM->SetShared(st_StaticModelManager->Load(pcModel,0.35f,0.35f,0.35f),RENDERABLE_GENERATE_PPL);		
	//pRM->SetShared(st_StaticModelManager->Load(pcModel,0.3f,0.3f,0.3f),RENDERABLE_GENERATE_PPL);		
		pRM->SetMaterialForAll(g_pRendererBack->LoadMaterial(pcMaterial));
		pRM->SetPosition(vOrigin+CVec3f(0.f,10.f,0.f));
		pRM->SetAngles(CVec3f(0.f,0.f,0.f));
		pRM->SetAnglesRotateSpeed(CVec3f(0.f,50.f,0.f));
		pRM->Update(0.f,false);

	SetRenderModel(pRM);

		// create a new physics model, and set it
	IPhysicsModel* pPM = new CBasicPhysics_Static();
		// TODO: find out the real bbox here
		pPM->m_BBox = pRM->GetBBox();
		pPM->SetPosition(vOrigin);
		//pPM->m_vVelocity.Set(0.f,0.f,50.f);
		pPM->SetActive(false);
			
			
	SetPhysicsModel(pPM);
}

DWORD CCollectableEntity::GetItemType(void)
{
	if (m_dwItemType == COLLECTABLE_ITEM_TYPE_NOT_INIT)
		BREAKPOINT(1); // type was not inited !!!!

	return m_dwItemType;
}

bool CCollectableEntity::TouchedBy(CEntity* pOther)
{
	if (!pOther->IsControllable())
		return false;

	if (m_bCollected)
		return false;

	if (m_bCollectRequested)
		return false;

	m_bCollectRequested = true;


	CControllableEntity* pContEntity = (CControllableEntity*) pOther;

	UINT uiContEntityID = pContEntity->GetID();


	
	tCombatEventClientRequestCollectable_FULL e; // place-holder
	e.uiPlayerEntityID = uiContEntityID;
	e.uiItemID         = GetID();

	g_pServer->OnEvent(COMBAT_EVENT_CLIENT_REQUEST_COLLECTABLE_UDP_FULL,&e);

	/*m_bCollected = true;
	m_fTimeTillReAppearance = 7.f;

	m_pPhysicsModel->SetActive(false);*/
	return false;
}

void CCollectableEntity::PickedUp(void)
{
	m_bCollected = true;
	m_fTimeTillReAppearance = 7.f;

	//m_pPhysicsModel->SetActive(false);
}


void CCollectableEntity::UpdateRenderModel(float fDeltaTime, bool bVisible)
{
	if (m_bCollected)
		m_fTimeTillReAppearance -= fDeltaTime;

	if (m_fTimeTillReAppearance<=0.f)
	{
		m_bCollected = false;
		m_bCollectRequested = false;
	}


	if (!m_bCollected && !m_bCollectRequested)
	{
		__super::UpdateRenderModel(fDeltaTime,bVisible);

		// FIXME:doesn't belong here, related to physics and shouldn't be in UpdateRenderModel ... 
		//m_pPhysicsModel->SetActive(true);
	}

	
}



void CCollectableEntity::Render(int iTech)
{
	if (!m_bCollected && !m_bCollectRequested)
		__super::Render(iTech);
}