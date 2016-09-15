#include "stdafx.h"
#include "ControllableEntity.h"

#include "Character.h"

#include "CollectableEntity.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Server.h"
extern IServer*    g_pServer;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "ProjectileEntity.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "ParticleSystemManager.h"

CControllableEntity::CControllableEntity()
{
	m_uiID = s_uiPlayerID++;
	m_dwRequests = 0;
	//m_fSpeed = 15.f;
	m_fSpeed = 4.f;
	Revive();	


	// hacked "touching"

	/*m_TouchingMaterials[0] = g_pRendererBack->LoadMaterial("Materials/ParticleSystems/hellholefire.utm");
	m_TouchingMaterials[1] = g_pRendererBack->LoadMaterial("Materials/ParticleSystems/fire1.utm");
	m_TouchingMaterials[2] = g_pRendererBack->LoadMaterial("Materials/ParticleSystems/barrelpoof.utm");
	m_TouchingMaterials[3] = g_pRendererBack->LoadMaterial("Materials/ParticleSystems/magic.utm");*/
	



	/*m_TouchingPS[0] = st_ParticleSystemManager->Load("ParticleSystems/explotion.ps");
	m_TouchingPS[1] = st_ParticleSystemManager->Load("ParticleSystems/monsters/rocket.ps");
	m_TouchingPS[1] = st_ParticleSystemManager->Load("ParticleSystems/monsters/fireball.ps");
	m_TouchingPS[1] = st_ParticleSystemManager->Load("ParticleSystems/monsters/blue.ps");
	m_TouchingPS[2] = st_ParticleSystemManager->Load("ParticleSystems/fire.ps");
	m_TouchingPS[2] = st_ParticleSystemManager->Load("ParticleSystems/cast.ps");*/
	


	//debug
	//m_iLife = 9999999;
		


}

void CControllableEntity::Revive(void)
{		
	m_bDying = false;
	m_fJumpSpeed = 10.f;

	m_bRenderRenderModel = true;
	m_bRenderRenderModel_ForShadow = true;

	m_dLastAttackTime_Attack1 = 0.0;
	m_dLastAttackTime_Attack2 = 0.0;

	/*m_bAttackingLoopable_Attack1 = false;
	m_bAttackingLoopable_Attack2 = false;*/

	m_iLife = 50;
	
	m_iAmmo_1 = 10;
	m_iAmmo_2 = 10;

}

void CControllableEntity::SetDying(void)
{
	m_bDying = true;

	// added 14.02.2011
	((CCharacter*)m_pRenderModel)->Die();

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_IDLE);	

	

	
}

bool CControllableEntity::IsDying(void) const
{
	return m_bDying;
}

CControllableEntity::~CControllableEntity()
{

}



/*void CControllableEntity::ApplyForceInLookDirection(float fForce)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	CVec3f vLookAt = GetVectorFromAngles(vAngles.x,vAngles.y-90.f);

	m_pPhysicsModel->ApplyForce(vLookAt*fForce);
}*/

/*CVec3f CControllableEntity::GetLookAt(void)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	return GetVectorFromAngles(vAngles.x,vAngles.y-90.f);
}*/


void CControllableEntity::Move_Up_DEBUG(float fDelta)
{
	//m_pPhysicsModel->ApplyForce(CVec3f(0.f,20.f,0.f)*fDelta);

	m_pPhysicsModel->ApplyForce(CVec3f(0.f,14.f,0.f)*fDelta);
	
	
	((CCharacter*) m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_2);

}

void CControllableEntity::Move_WalkForward(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(0.f,0.f,-1.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);
	// ignore deltas for now

	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_WALK);	
}

void CControllableEntity::Move_WalkBackwards(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(0.f,0.f,1.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);
	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_WALK_BACK);	
}

void CControllableEntity::Move_RunForward(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(0.f,0.f,-1.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*2.5f*fDelta);
	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*2.f*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_RUN);	
}

void CControllableEntity::Move_RunBackwards(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(0.f,0.f,1.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);
	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*2.f*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_RUN_BACK);	
}



void CControllableEntity::Move_StrafeLeft(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(-1.f,0.f,0.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);
	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_STRAFE_LEFT);	
}

void CControllableEntity::Move_StrafeRight(float fDelta)
{
	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotateVec(1.f,0.f,0.f);
	vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);

	//m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);
	m_pPhysicsModel->ApplyForce(vRotateVec*m_fSpeed*fDelta);

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_STRAFE_RIGHT);	
}

void CControllableEntity::AddAngles(const CVec3f& vAngles)
{
	m_pRenderModel->SetAngles(m_pRenderModel->GetAngles()+vAngles);
	
}

void CControllableEntity::SetAngles(const CVec3f& vAngles)
{
	m_pRenderModel->SetAngles(vAngles);
	
}


void CControllableEntity::Move_RotateLeft(float fDelta/*, bool bOnlyAnimate*/)
{
	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_LEFT);
	
	//if (!bOnlyAnimate)
		//m_pRenderModel->SetAngles(m_pRenderModel->GetAngles() + CVec3f(0.f,fDelta*0.1f,0.f));	
	//m_pRenderModel->SetAngles(m_pRenderModel->GetAngles());	
}

void CControllableEntity::Move_RotateRight(float fDelta/*, bool bOnlyAnimate*/)
{
	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_RIGHT);
	
	//if (!bOnlyAnimate)
		//m_pRenderModel->SetAngles(m_pRenderModel->GetAngles() + CVec3f(0.f,-fDelta*0.1f,0.f));	
	//m_pRenderModel->SetAngles(m_pRenderModel->GetAngles());	
}

void CControllableEntity::Jump(void)
{	
	if (m_pPhysicsModel->GetDistanceToFloor() > 7.f)
		return;
	
	//m_pPhysicsModel->ApplyForce(CVec3f(0.f,m_fJumpSpeed,0.f));

	if (m_pPhysicsModel->m_vVelocity.y>=0.f)
		m_pPhysicsModel->m_vVelocity.y+=m_fJumpSpeed;
	else
		m_pPhysicsModel->m_vVelocity.y=m_fJumpSpeed;

	((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_STRAFE_RIGHT);	

}

int CControllableEntity::GetAmmo(int iNum) const
{
	if (1==iNum)
		return m_iAmmo_1;
	else if (2==iNum)
		return m_iAmmo_2;
	BREAKPOINT(1); // unkown ammo type

}

// if can't attack, returns false
bool CControllableEntity::Attack(int iAttackType)
{


	/*if (m_iRegularWeaponType==WEAPON_TYPE_NOT_INITED)
	{
		char error[500];
		sprintf(error,"No weapon defined for character: %s!\n",m_pRenderModel->GetName());
		g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	}*/

	bool bLoopedAttack = false;

	CVec3f vPos;
	CVec3f vVelocity;
	DWORD dwProjectileType=0;



	if (1==iAttackType)
	{
		if (m_iAmmo_1<1)
		{
			if (!m_bInsideAttackRequest)
			{
				//g_TextMessages.AddMessage("Out of ammo (Weapon 1)!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/weapons/noammo.wav");						
				spSound->Play(m_pPhysicsModel->GetPosition(),255);



				m_bInsideAttackRequest = true;
			}
			
			return false;
		}

		if ( ((CCharacter*)m_pRenderModel)->GetMD5Inst()->GetShareIsLoopable_Attack1())
			bLoopedAttack = true;

		if (g_pTimer->GetTime() - m_dLastAttackTime_Attack1 < ATTACK_1_WAIT_TIME)
		{
			if (bLoopedAttack)
				((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_1);
			return true;
		}

		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_1);

		m_dLastAttackTime_Attack1 = g_pTimer->GetTime();

	} else if (2==iAttackType)
	{
		if (m_iAmmo_2<1)
		{
			//g_TextMessages.AddMessage("Out of ammo (Weapon 2)!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

		if ( ((CCharacter*)m_pRenderModel)->GetMD5Inst()->GetShareIsLoopable_Attack2())
			bLoopedAttack = true;

		if (g_pTimer->GetTime() - m_dLastAttackTime_Attack2 < ATTACK_2_WAIT_TIME)
		{
			if (bLoopedAttack)
				((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_2);
			return true;
		}

		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_1);	

		m_dLastAttackTime_Attack2 = g_pTimer->GetTime();

	} else
		BREAKPOINT(1); // unrecognized attack type

	/*if (bLoopedAttack)
		return true;*/


	return true;
}




void CControllableEntity::UpdateRenderModel(float fDeltaTime,bool bVisible)
{
	
	if (m_bRenderRenderModel)
		__super::UpdateRenderModel(fDeltaTime,bVisible);


}

void CControllableEntity::Render(int iTech)
{
		if (m_bRenderRenderModel)
		__super::Render(iTech);

}

void CControllableEntity::SetLife(int iLife)
{
	m_iLife = iLife;


	if (iLife>100)
		m_iLife = 100;
	
	if (m_iLife<0)
		m_iLife = 0;

}

void CControllableEntity::PickUp(DWORD dwItemType)
{
	if (dwItemType == COLLECTABLE_ITEM_TYPE_HEALTH) // small health add
	{
		m_iLife+=5;
		if (m_iLife>75)
			m_iLife=75;
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_ARMOR_BODY) // body armor
	{
		m_iLife+=15;
		if (m_iLife>75)
			m_iLife=75;
	}else if (dwItemType == COLLECTABLE_ITEM_TYPE_ARMOR_COMBAT) // combat armot
	{
		m_iLife+=20;
		if (m_iLife>75)
			m_iLife=75;
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_ARMOR_SHARD) // ammo 1
	{
			
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_AMMO_1) // ammo 1
	{
		m_iAmmo_1+=5;	
		if (m_iAmmo_1>50)
			m_iAmmo_1=50;
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_AMMO_2) // ammo 2
	{
		m_iAmmo_1+=5;
		if (m_iAmmo_1>50)
			m_iAmmo_1=50;
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_AMMO_3) // ammo 3
	{
		m_iAmmo_1+=5;						
		if (m_iAmmo_1>50)
			m_iAmmo_1=50;
	}
	else if (dwItemType == COLLECTABLE_ITEM_TYPE_NOT_INIT)
	{
		BREAKPOINT(1); // item type was not init!
	}
	else 
	{
		BREAKPOINT(1); // unrecognized item type!
	}
	

}


CVec3f CControllableEntity::GetCurrentBindPosition(int iBindNum) const
{
	if (m_pRenderModel)
		return ((CCharacter*)m_pRenderModel)->GetCurrentFinalJointPosition(iBindNum);

	if (m_pPhysicsModel)
		return m_pPhysicsModel->GetPosition();

	return CVec3f(0.f,0.f,0.f);
}

void CControllableEntity::SetRenderRenderModel(bool bRender)
{
	m_bRenderRenderModel=bRender;
}



void CControllableEntity::TriggerFrameActions(const tFrameActions* pFrameActions)
{
	if (!pFrameActions)
		return;

	for (int i=0;i<pFrameActions->iActionsNum;i++)
			{
				//sprintf(temp,"Triggered frame action! action:[%u], joint:[%u]",ppFrameActions[0]->pActions[i].uiAction,ppFrameActions[0]->pActions[i].iJointIndex);				
				//g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



				switch(pFrameActions->pActions[i].uiAction)
				{
					case PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PARTICLE_SYSTEM:					
					{
						
						//g_TextMessages.AddMessage("Triggered Particle System!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);						
						CVec3f vSpawnPS_Pos = ((CCharacter*)m_pRenderModel)->GetCurrentFinalJointPosition(pFrameActions->pActions[i].iJointIndex);
						

						CParticleSystem* pPS = new CParticleSystem();
						pPS->SetBindIndex(pFrameActions->pActions[i].iJointIndex);
						pPS->SetOrigin(vSpawnPS_Pos);
						
						if (!pFrameActions->pActions[i].pData)
							pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/explotion.ps"));			
						else
							pPS->SetShared(st_ParticleSystemManager->Load((char*)pFrameActions->pActions[i].pData));
									

						// link it to the map
						g_pQ3Map->SpawnParticleSystem(pPS,this);


						m_ParticleSystems.push_front(pPS);

					}
					break;

					case PLAYER_ANIM_TRIGGERED_ACTION_SOUND:					
					{
						
												
						BREAKPOINT(!pFrameActions->pActions[i].pData)
						
						//st_SoundManager->PlaySound((char*)pFrameActions->pActions[i].pData,m_pPhysicsModel->GetPosition(),255);
						CSmartPointer<CSound> spSound = st_SoundManager->Load((char*)pFrameActions->pActions[i].pData);						
						spSound->Play(m_pPhysicsModel->GetPosition(),255);
						

						/*char temp[500];
						sprintf(temp,"Triggered sound %s",(char*)pFrameActions->pActions[i].pData);

						g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
						

						// g_SoundAPI.PlaySound(g_SoundManager.LoadSound(pFrameActions->pActions[i].pData),255);
					}
					break;

					case PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_1:
					{

					int iWeaponType = ((CCharacter*)m_pRenderModel)->GetRegularWeaponType();
					float fWeaponSpeed = ((CCharacter*)m_pRenderModel)->GetRegularWeaponSpeed();

					if (AffectGamePlay())
					{

						if (IsControllable())
						{
							if (! ((CControllableEntity*)this)->m_dwRequests&PLAYER_ACTION_REQUEST_FLAG_ATTACK_1)						
							{
								g_TextMessages.AddMessage("DEBUG:Trigger point ignored because player didn't press attack",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
								continue;
							}

							if ( iWeaponType == WEAPON_TYPE_NOT_INITED)
							{
								char error[500];
								sprintf(error,"No weapon defined for character [%s]!",m_pRenderModel->GetName());
								g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
								continue;
							}
						}

						CVec3f vAngles = m_pRenderModel->GetAngles();	
						vAngles.z = 0.f;	
						CVec3f vLookDir(0.f,0.f,-1.f);
						vLookDir	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vLookDir);		

						

						
						CVec3f vSpawnProjectilePos = ((CCharacter*)m_pRenderModel)->GetCurrentFinalJointPosition(pFrameActions->pActions[i].iJointIndex);

						CVec3f vShootDir = (GetPhysicsModel()->GetPosition() + (vLookDir*10000.f)) - vSpawnProjectilePos;
						vShootDir.Normalize();

						//CVec3f vPos += vLookDir*95.f;
						

						//CVec3f vPos = (*ppFrameActions[0]->pActions[i].vJointPos)+ vLookDir*95.f;
						

						CVec3f vVelocity = vShootDir * fWeaponSpeed;
						DWORD dwProjectileType = iWeaponType;

						DWORD dwProjectileID = g_pQ3Map->SpawnProjectile(dwProjectileType,vSpawnProjectilePos,vVelocity,GetID());	

						CEntity* proj = g_pQ3Map->GetEntity(dwProjectileID);

						////////
						tCombatEventClientRequestProjectileShoot e;		
						CVec3f vNormalizedVel = proj->GetPhysicsModel()->m_vVelocity;
						vNormalizedVel.Normalize();

						memcpy(e.fDirection,vNormalizedVel.v,sizeof(float)*3);
						memcpy(e.fStartPosition,proj->GetPhysicsModel()->GetPosition().v,sizeof(float)*3);
						e.ucProjectileType = dwProjectileType;
						e.uiPlayerEntityID = GetID();

						g_pServer->OnEvent(COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT,&e);

						if (m_iAmmo_1 > 0)
							m_iAmmo_1--;					
						
					}
					}
					break;

					case PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_2:
					if (AffectGamePlay())
					{
					
					}
					break;

					
				default:
					{
						// pass the trigger up to the parent, perhaps he can handle it
						__super::TriggerFrameActions(pFrameActions);
					
					break;
					}
				}

			}
	



}