#include "stdafx.h"
#include "Entity.h"


unsigned int CEntity::s_uiID = 0;
unsigned int CEntity::s_uiPlayerID = 10000;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "ControllableEntity.h"
#include "ProjectileEntity.h"
#include "Character.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Server.h"
extern IServer*    g_pServer;

CEntity::CEntity()
{
	//static unsigned int s_uiID = 0;
	
	m_uiID = s_uiID++;

	m_pRenderModel = NULL;
	m_pPhysicsModel = NULL;

	m_bAlreadyDrawnThisFrame = false;

	m_bCleanMe = false;

	m_bCastShadow = true;

	//m_iFramesSinceLastUpdateArrived = 0;
}

CEntity::~CEntity()
{
	// clean all particle systems
	for (std::list<CParticleSystem*>::iterator it=m_ParticleSystems.begin();it!=m_ParticleSystems.end();++it)
	{
		(*it)->Die();	
		(*it)->InformParentDied();
	}




	SafeDelete(m_pRenderModel);
	SafeDelete(m_pPhysicsModel);
}

/*
// only used in networked players
void CEntity::ResetPhysicsFramesSinceLastUpdateArrived()
{
	m_iFramesSinceLastUpdateArrived = 0;
}

void CEntity::IncreasePhysicsFramesSinceLastUpdateArrived()
{
	m_iFramesSinceLastUpdateArrived++;
}

int  CEntity::GetPhysicsFramesSinceLastUpdateArrived() const
{
	return m_iFramesSinceLastUpdateArrived;
}
*/

LEAFS_INDICES_MAP* CEntity::GetLinkedLeafsMap(void)
{
	return &m_LinkedLeafs;
}


void CEntity::FindPhysicsStepDesiredPosition(float fStep)
{
	m_pPhysicsModel->FindPhysicsStepDesiredPosition(fStep);

}

void  CEntity::SetPhysicsModel(IPhysicsModel* pPM)
{
	m_pPhysicsModel = pPM;
	m_pPhysicsModel->SetParentEntity(this);
}

void CEntity::BlendPhysicsState(float fAlpha)
{
	m_pPhysicsModel->BlendPhysicsState(fAlpha);


	if (m_pRenderModel)
	{ // blah

		if (m_pRenderModel->GetAnglesBlending())
			m_pRenderModel->BlendAnglesState(fAlpha);

		CVec3f vMoveDown(0.f,0.f,0.f);
		
		if (IsControllable())
			vMoveDown.Set(0.f, -max(abs(m_pPhysicsModel->m_BBox.min.y),abs(m_pPhysicsModel->m_BBox.max.y)),0.f);

		m_pRenderModel->SetPosition(m_pPhysicsModel->GetPosition()+vMoveDown);
	}
}

CVec3f CEntity::GetCurrentBindPosition(int iBindNum) const
{
	if (m_pPhysicsModel)
		return m_pPhysicsModel->GetPosition();

	return CVec3f(0.f,0.f,0.f);

}

void CEntity::UpdateRenderModel(float fDeltaTime,bool bVisible)
{

	if (m_pRenderModel)
		m_pRenderModel->Update(fDeltaTime,bVisible);


	///// update particle systems - move this
	CVec3f vSpawnPS_Pos(0.f,0.f,0.f);

	for (std::list<CParticleSystem*>::iterator it=m_ParticleSystems.begin();it!=m_ParticleSystems.end();)
	{
		vSpawnPS_Pos = GetCurrentBindPosition((*it)->GetBindIndex());
		
		(*it)->SetOrigin(vSpawnPS_Pos);
		
		if ((*it)->GetFinishedDying())
		{		
			//(*it)->SetCleanMe();
			it = m_ParticleSystems.erase(it);
			continue;
		};

		++it;
	}

	
	
}

void CEntity::RemovePS(const CParticleSystem* pParticleSystem)
{
	for (std::list<CParticleSystem*>::iterator it=m_ParticleSystems.begin();it!=m_ParticleSystems.end();)
	{
		
		if ((*it)==pParticleSystem)
		{						
			it = m_ParticleSystems.erase(it);
			continue;
		};

		++it;
	}

}



void CEntity::Render(int iTech)
{
	if (m_pRenderModel)
		m_pRenderModel->Render(iTech);

	// render all of the particle systems
	/*for (std::list<CParticleSystem*>::iterator it=m_ParticleSystems.begin();it!=m_ParticleSystems.end();++it)
	{
		(*it)->Render(iTech);
	}*/

}


void CEntity::TriggerFrameActions(const tFrameActions* pFrameActions)
{
	BREAKPOINT(1); // means that a frame action was passed here because it couldn't be handled (unkown perhaps)

}