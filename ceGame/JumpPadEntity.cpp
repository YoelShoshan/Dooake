#include "stdafx.h"
#include "JumpPadEntity.h"

#include "ControllableEntity.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

extern int g_idbgJumppadAddForceCount;

CJumpPadEntity::CJumpPadEntity()
{
	m_vForce = CVec3f(0.f,400.f,300.f);	

	m_pSound = NULL;
	g_SoundAPI.LoadSample("Sound/jumppad.wav",m_pSound);
}

CJumpPadEntity::~CJumpPadEntity()
{
	if (m_pSound)
	{
		g_SoundAPI.FreeSound(m_pSound);
		SafeDelete(m_pSound);
	}
}

bool CJumpPadEntity::TouchedBy(CEntity* pOther)
{
	if (!pOther->IsControllable())
		return false;

	
	/*g_pGraphicsAPI->startPrinting();
	g_pGraphicsAPI->AddDebugText("JUMP PAD\n");
	g_pGraphicsAPI->AddDebugText("JUMP PAD\n");
	g_pGraphicsAPI->AddDebugText("JUMP PAD\n");
	g_pGraphicsAPI->stopPrinting();*/

	//TODO: make sure it only happens once for a jump per human
	g_SoundAPI.PlaySample3D(m_pSound,m_pPhysicsModel->GetPosition(),255);
	
	CControllableEntity* pContEntity = (CControllableEntity*) pOther;


	std::map<unsigned int,double>::iterator it = m_TriggeredEntities.begin();

	it = m_TriggeredEntities.find(pOther->GetID());

	////////////////////////////////////////////////////////
	// debug debug debug
	//char blah[400];
	///////////////////////////////////////////////////////

	/*if (it!=m_TriggeredEntities.end()) // it was already on the list
	{
		double dDelta = g_pTimer->GetTime() - it->second;
		if (dDelta > AFFECT_PLAYER_MIN_DELTA_TIME) // if enough time passed
		{
			it->second = g_pTimer->GetTime();
			
			// debug debug
			//sprintf(blah,"Enough time passed - %f",dDelta);
			//g_pLogFile->OutPutPlainText(blah,LOG_MESSAGE_INFORMATIVE);
			////////////////
		}
		else
		{
			// debug debug
			//sprintf(blah,"Jump denied - not enough time passed");
			//g_pLogFile->OutPutPlainText(blah,LOG_MESSAGE_INFORMATIVE);
			////////////////
			return; // not enough time passed
		}

	}
	else
	{
		// debug debug
		//sprintf(blah,"First time effecting entity - adding to list.");
		//g_pLogFile->OutPutPlainText(blah,LOG_MESSAGE_INFORMATIVE);
		////////////////

		m_TriggeredEntities[pOther->GetID()]=g_pTimer->GetTime();
	}
	
	// debug 
	if (pContEntity->GetPhysicsModel()->m_vVelocity.y <= 0.f)
		return;*/
	
	/*sprintf(blah,"Triggered Entities map size = %d",m_TriggeredEntities.size());
	g_pLogFile->OutPutPlainText(blah,LOG_MESSAGE_INFORMATIVE);*/
	////////////////////////////////////////////////////////
	

	//if (m_vForce.Length()>1000.f)
		//m_vForce.Set(0.f,100.f,0.f);

	// restore
	//pContEntity->GetPhysicsModel()->ApplyForce(m_vForce);


	//pContEntity->GetPhysicsModel()->m_vVelocity = CVec3f(0.f,0.f,0.f);

	//pContEntity->GetPhysicsModel()->m_vVelocity = CVec3f(0.f,400.f,0.f);
	pContEntity->GetPhysicsModel()->m_vVelocity = m_vForce;

	//pContEntity->GetPhysicsModel()->m_vAcceleration = CVec3f(0.f,0.f,0.f);

	g_idbgJumppadAddForceCount++;

	return false;
	
}

void CJumpPadEntity::UpdateRenderModel(float fDeltaTime)
{
	
}



void CJumpPadEntity::Render(int iTech)
{
	// dummy
}


void CJumpPadEntity::SetDestination(const CVec3f& vDest)
{
	m_vDestination = vDest;

	CVec3f vJumpPadVelocity;
	//float fGravity = 4.9f;

	float fGravity = 3.8f;
	//float fGravity = 11.5f;
	

	//float fGravity = -0.0015f;

	
	/*vJumpPadVelocity.v[1] = sqrtf(2* (-g_vGravity.v[1])*( m_ppAllStaticEntities[m_iStaticEntitiesNum]->vTarget.v[1] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[1]));
	vJumpPadVelocity.v[0] = (((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget.v[0] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[0]) * ((-g_vGravity.v[1])*1.f) / vJumpPadVelocity.v[1];
	vJumpPadVelocity.v[2] = (((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget.v[2] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[2]) * ((-g_vGravity.v[1])*1.f)/ vJumpPadVelocity.v[1];*/
	

	CVec3f vPos = m_pPhysicsModel->GetPosition();

	//vJumpPadVelocity.y = sqrtf(2*fGravity*(m_vDestination.y - vPos.y));
	vJumpPadVelocity.y = sqrtf(fGravity*(m_vDestination.y - vPos.y));
	vJumpPadVelocity.x = ( m_vDestination.x -  vPos.x) * fGravity / vJumpPadVelocity.y;
	vJumpPadVelocity.z = ( m_vDestination.z -   vPos.z) * fGravity / vJumpPadVelocity.y;

	//BREAKPOINT(vJumpPadVelocity.y<0.f);

	m_vForce = vJumpPadVelocity;
	/*m_vForce.y*=50.f;
	m_vForce.x*=50.f;
	m_vForce.z*=50.f;*/

	/*m_vForce.x*=26.f;
	m_vForce.y*=23.f;
	m_vForce.z*=26.f;*/

	//m_vForce*=0.65f;

	float fFact = 0.5125f;
	//float fFact = 1.f;
	m_vForce.x*=fFact;
	m_vForce.z*=fFact;

	//m_vForce/=2.f;

	// debug
	//m_vForce.Set(0.f,1500.f,0.f);
}