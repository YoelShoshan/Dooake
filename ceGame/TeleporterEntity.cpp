#include "stdafx.h"
#include "TeleporterEntity.h"

#include "ControllableEntity.h"

#include "Server.h"
extern IServer*    g_pServer;

#include "ProjectileEntity_Server.h"



CTeleporterEntity::CTeleporterEntity()
{
	/*CParticleSystem* pPS = new CParticleSystem();

	
	CVec3f vNormVel(vOrigin);
	vNormVel.Normalize();

	pPS->SetOrigin(vOrigin-(vNormVel*10.f));
						
	pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/monsters/mancubus_smoke.ps"));
	
						
	// link it to the map
	g_pQ3Map->SpawnParticleSystem(pPS,this);

	m_ParticleSystems.push_front(pPS);*/
	//m_pSound = NULL;

	m_pSound = NULL;
	g_SoundAPI.LoadSample("Sound/teleout.wav",m_pSound);

};

CTeleporterEntity::~CTeleporterEntity()
{
	if (m_pSound)
	{
		g_SoundAPI.FreeSound(m_pSound);
		SafeDelete(m_pSound);
	}

};

bool CTeleporterEntity::TouchedBy(CEntity* pOther)
{	

	if (g_pServer->m_dwGameType == SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT)
		return false;


	g_SoundAPI.PlaySample3D(m_pSound,m_vDestination,255);

	if (pOther->IsControllable())
	{
		CControllableEntity* pContEntity = (CControllableEntity*) pOther;

		UINT uiContEntityID = pContEntity->GetID();


		tMetTeleporterEvent_Server e;
		e.uiPlayerEntityID = uiContEntityID;
		e.uiTeleporterID   = GetID();

		g_pServer->OnEvent(SERVER_EVENT_PLAYER_MET_TELEPORTER_UDP,&e);

		return false;
	}
	else
	if (pOther->IsProjectile())
	{
		CProjectileEntity_Server* pProjEntity = (CProjectileEntity_Server*) pOther;

		pProjEntity->GetPhysicsModel()->SetPosition(m_vDestination);

		CVec3f vVelocity = pProjEntity->GetPhysicsModel()->m_vVelocity;

		float fSize = vVelocity.Length();

		CVec3f vPortalLookDir = GetVectorFromAngles(0.f,m_fAngY);

		vPortalLookDir.Normalize();

		pProjEntity->GetPhysicsModel()->m_vVelocity = vPortalLookDir*fSize;


		return false;
	}

	// remove this!
	/*static bool bHappenedOnce = false;

		
	if (bHappenedOnce)
		return;*/

	

	//bHappenedOnce=true;

	return false;
}


