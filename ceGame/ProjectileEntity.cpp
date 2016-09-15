#include "stdafx.h"
#include "ProjectileEntity.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "StaticModel.h"
#include "ProjectilePhysics.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Server.h"
extern IServer*    g_pServer;
#include "ParticleSystemManager.h"


CProjectileEntity::CProjectileEntity()
{

}

CProjectileEntity::~CProjectileEntity()
{
	/*int k=13;
	k++;*/

}

void CProjectileEntity::UpdateServer(void)
{
	// send the request for the real projectile on the server side

	/*tCombatEventClientRequestProjectileShoot e;		
	CVec3f vNormalizedVel = m_pPhysicsModel->m_vVelocity;
	vNormalizedVel.Normalize();

	memcpy(e.fDirection,vNormalizedVel.v,sizeof(float)*3);
	memcpy(e.fStartPosition,m_pPhysicsModel->GetPosition().v,sizeof(float)*3);
	e.ucProjectileType = m_dwProjectileType;
	e.uiPlayerEntityID =m_dwShotByEntityID;

	g_pServer->OnEvent(COMBAT_EVENT_CLIENT_REQUEST_PROJECTILE_SHOOT,&e);*/

}

void CProjectileEntity::Init(DWORD dwType,const char* pcModel, const char* pcMaterial,const CVec3f& vOrigin, const CVec3f& vVelocity,DWORD dwShotByEntity)
{	
	m_dwProjectileType = dwType;	
	m_dwShotByEntityID = dwShotByEntity;

	CStaticModel* pRM = new CStaticModel();
		//pRM->SetShared(st_StaticModelManager->Load(pcModel,0.2f,0.2f,0.2f),RENDERABLE_GENERATE_PPL);		
	pRM->SetShared(st_StaticModelManager->Load(pcModel,0.07f,0.07f,0.07f),RENDERABLE_GENERATE_PPL);		

	//pRM->SetShared(st_StaticModelManager->Load(pcModel,1.f,1.f,1.f),RENDERABLE_GENERATE_PPL);		

		pRM->SetMaterialForAll(g_pRendererBack->LoadMaterial(pcMaterial));
		pRM->SetPosition(vOrigin);
		pRM->SetAngles(CVec3f(0.f,0.f,0.f));
		//pRM->SetAnglesRotateSpeed(CVec3f(0.f,15.f,0.f));
		pRM->Update(0.f,false);

	SetRenderModel(pRM);

		// create a new physics model, and set it
	IPhysicsModel* pPM = new CProjectilePhysics();
		// TODO: find out the real bbox here
		pPM->m_BBox = pRM->GetBBox();

		/*pPM->m_BBox.min/=2.f;
		pPM->m_BBox.max/=2.f;*/

		pPM->SetPosition(vOrigin);
		pPM->m_vVelocity = vVelocity;
		//pPM->m_vVelocity.Set(0.f,0.f,50.f);
		//pPM->SetActive(false);
						
	SetPhysicsModel(pPM);

	
	// start a particle system

	CParticleSystem* pPS = new CParticleSystem();

	
	CVec3f vNormVel(vOrigin);
	vNormVel.Normalize();

	pPS->SetOrigin(vOrigin-(vNormVel*10.f));
						
	switch(dwType)
	{
	case WEAPON_TYPE_ROCKET:
		{
			pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/monsters/rocket.ps"));
		}
	break;
	case WEAPON_TYPE_FIREBALL:
		{
			pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/monsters/fireball.ps"));
		}
	break;
	case WEAPON_TYPE_BLUE:
		{
			pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/monsters/blue.ps"));
		}
	break;

	case WEAPON_TYPE_WHITE:
		{
			pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/monsters/white.ps"));			
		}
	break;

	default:
		{
			BREAKPOINT(1);
		}

	}
	
						
	// link it to the map
	g_pQ3Map->SpawnParticleSystem(pPS,this);

	m_ParticleSystems.push_front(pPS);

}

CVec3f CProjectileEntity::GetCurrentBindPosition(int iBindNum) const
{
	if (m_pPhysicsModel)
	{
		CVec3f vNormVel(m_pPhysicsModel->m_vVelocity);
		vNormVel.Normalize();

		return m_pPhysicsModel->GetPosition()-vNormVel*10.f;
	}


	return CVec3f(0.f,0.f,0.f);
}

bool CProjectileEntity::TouchedBy(CEntity* pOther)
{

	m_bCleanMe = true;


	m_pPhysicsModel->m_vVelocity.Set(0.f,0.f,0.f);

	//m_bCleanMe = true;
	/*for (std::list<CParticleSystem*>::iterator it=m_ParticleSystems.begin();it!=m_ParticleSystems.end();++it)
	{
		(*it)->Die();
	}*/

	//TODO: make sure that it cleans itself after timeout

	CParticleSystem* pPS = new CParticleSystem();

	pPS->SetOrigin(m_pPhysicsModel->GetPosition());
						
	pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/explotion.ps"));
	
					
	// link it to the map
	g_pQ3Map->SpawnParticleSystem(pPS,NULL);


	return false;

}