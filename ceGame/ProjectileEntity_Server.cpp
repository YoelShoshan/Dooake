#include "stdafx.h"
#include "ProjectileEntity_Server.h"

#include "Server.h"
extern IServer*    g_pServer;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "Sound.h"
#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;

#include "ControllableEntity.h"

CProjectileEntity_Server::CProjectileEntity_Server()
{

}

CProjectileEntity_Server::~CProjectileEntity_Server()
{

}

void CProjectileEntity_Server::UpdateServer(void)
{
	/*tCombatEvent_ServerUpdate_ProjectileShoot e;
	CVec3f vNormalizedVel = m_pPhysicsModel->m_vVelocity;
	vNormalizedVel.Normalize();

	memcpy(e.fDirection,vNormalizedVel.v,sizeof(float)*3);
	memcpy(e.fStartPosition,m_pPhysicsModel->GetPosition().v,sizeof(float)*3);
	e.ucProjectileType = m_dwProjectileType;
	e.uiPlayerEntityID = m_dwShotByEntityID;

	g_pServer->OnEvent(COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT,&e);*/

	
}

bool CProjectileEntity_Server::TouchedBy(CEntity* pOther)
{
	if (pOther->IsTeleporter())
		return false;


	if (pOther->IsCollectable())
	{
		return true; // true means ignore me... 
	}

	// IF THIS BREAKS SOMETHING, MOVE TO AFTER __SUPER
		//TODO: add "&& timeSinceLaunch<100 ms"
	if (m_dwShotByEntityID==pOther->GetID())
		return true;

	//TODO:check if this didn't break anything!
	__super::TouchedBy(pOther);



	m_pPhysicsModel->m_vVelocity.Set(0.f,0.f,0.f);
	
	/*
	#define SERVER_EVENT_PROJECTILE_EXPLOTION      3
struct tProjectileExpoltionEvent_Server
{
	CVec3f vPos;
	unsigned char cProjectileExplotionType;
};
*/
	/*if (!pOther->IsControllable())
		return;*/

	tProjectileExpoltionEvent_Server e;
	e.cProjectileExplotionType = m_dwProjectileType;
	e.uiShotByPlayerID = m_dwShotByEntityID;
	e.vPos = m_pPhysicsModel->GetPosition();

	if (pOther->IsControllable()) // if hit directly a controllable - direct hit!!!
	{
		CControllableEntity* pControllable = (CControllableEntity*) pOther;

		if (!pControllable->IsDying())
		{
			g_TextMessages.AddMessage("Direct Hit !!!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_GAME);
			
			CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/metal_solid_impact_bullet3.wav");						
					spSound->Play(pOther->GetPhysicsModel()->GetPosition(),255);
		}				
		e.bDirectHit = true;
	} else
	{
		e.bDirectHit = false;
	}


	g_pServer->OnEvent(SERVER_EVENT_PROJECTILE_EXPLOTION_UDP,&e);


	return false;
}