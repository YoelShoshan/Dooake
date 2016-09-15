#include "stdafx.h"
#include "ParticleSystemEntity.h"

#include "DummyPhysicsModel.h"
#include "BasicPhysics.h"

CParticleSystemEntity::CParticleSystemEntity()
{
	//SetPhysicsModel(new CDummyPhysicsModel());
	//SetPhysicsModel(new CBasicPhysics());
	

	m_pParentEntity = NULL;
		
}

/*CParticleSystemEntity::CParticleSystemEntity(CEntity* pParentEntity)
{
	CParticleSystemEntity();

	m_pParentEntity = pParentEntity;

}*/

CParticleSystemEntity::~CParticleSystemEntity()
{
	/*HERE I NEED TO UPDATE PARENT ENTITY TO STOP UPDATING ME
	(REMOVE ME FROM owned particles systems list)*/

	if (!((CParticleSystem*)m_pRenderModel)->GetParentDied() && m_pParentEntity && m_pRenderModel)
		m_pParentEntity->RemovePS((CParticleSystem*)m_pRenderModel);
	
}

void CParticleSystemEntity::UpdateRenderModel(float fDeltaTime,bool bVisible)
{
	__super::UpdateRenderModel(fDeltaTime,bVisible);

	//TODO: really find out the bbox of the particle system

	if (((CParticleSystem*)m_pRenderModel)->GetCleanMe())
		m_bCleanMe = true;

	m_pPhysicsModel->SetPosition(((CParticleSystem*)m_pRenderModel)->GetOrigin());

}