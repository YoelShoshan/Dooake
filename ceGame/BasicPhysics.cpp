#include "stdafx.h"
#include "BasicPhysics.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

/*#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;*/

CBasicPhysics::CBasicPhysics()
{
	m_bInAir = true;
	
	m_vPreviousTimeStepPos.Set(0.f,0.f,0.f);
	m_vCurrentTimeStepPos.Set(0.f,0.f,0.f);
	m_vPos.Set(0.f,0.f,0.f);
	m_vDesiredPos.Set(0.f,0.f,0.f);

}

CBasicPhysics::~CBasicPhysics()
{

}

void CBasicPhysics::FindPhysicsStepDesiredPosition(float fStep)
{	
	m_vVelocity+=m_vActiveForces;
	
	if (m_vVelocity.Length()<VELOCITY_MINIMUM_LENGTH)
		m_vVelocity.Set(0.f,0.f,0.f);

	m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;


	/*
	m_vVelocity += (m_vActiveForces*fStep)+(vGravity*fStep);

	
	m_vDesiredPos = m_vCurrentTimeStepPos+m_vVelocity;

	*/

	//m_vVelocity = m_vVelocity*fStep;

	m_vDesiredPos = m_vCurrentTimeStepPos+m_vVelocity*fStep;


	//m_v

	m_vActiveForces.Set(0.f,0.f,0.f);
	
}

void CBasicPhysics::ChangeVelocityBasedOnActualMovement(const CVec3f& vNewPos, const CVec3f& vOldPos,float fStep)
{
	CVec3f vActualVelocity = (vNewPos - vOldPos) /fStep;

	/*if (m_bInAir)
		m_vVelocity = vActualVelocity*0.49f;
		//m_vVelocity = vActualVelocity*0.46f;
	else // on ground
		m_vVelocity = vActualVelocity*0.35f;
		//m_vVelocity = vActualVelocity*0.5f;*/

	/*if (m_bInAir)
		m_vVelocity = (m_vVelocity / fStep)*0.95f;
	else // on ground
		m_vVelocity = (m_vVelocity / fStep)*0.85f;*/


	if (m_bInAir)
		m_vVelocity = m_vVelocity *0.98f;
	else // on ground
		m_vVelocity = m_vVelocity *0.85f;

	

	BREAKPOINT(   *((unsigned long*)&m_vVelocity.x) == 0xffc00000 );
	

}

void CBasicPhysics::BlendPhysicsState(float fAlpha)
{
	m_vPos = (m_vCurrentTimeStepPos*fAlpha) + (m_vPreviousTimeStepPos*(1.f-fAlpha));

	/*if (!m_pParentEntity)
		m_pParentEntity->GetRenderModel()->*/
		

	//CVec3f blendedAngles = (m_pParentEntity->GetRenderModel()->GetCurrentAngles()*fAlpha)+(m_pParentEntity->GetRenderModel()->GetPreviousAngles()*(1.f-fAlpha);
	
}

//its: static extents, static pos,staticpos, other radius, other pos, other dst pos, t , normal )

bool CBasicPhysics::CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal)
{
	vHitNormal.Set(0.f,0.f,0.f);
	// DEBUG DEBUG
	/*CVec3f vPos  = pOtherPM->GetPreviousTimeStepPos();
	CVec3f vMove = pOtherPM->GetDesiredPos() - vPos;*/

	// don't collide between 2 projectils
	if (GetParentEntity()->IsProjectile() && pOtherPM->GetParentEntity()->IsProjectile())
	{
		fMoveTillHit = 1.f;
		return false;
	}
	

	bool bInter  = 	AABBSweep( (m_BBox.max - m_BBox.min)*0.5f,
				                m_vCurrentTimeStepPos,
 			                    m_vCurrentTimeStepPos,
				               (pOtherPM->m_BBox.max - pOtherPM->m_BBox.min)*0.5f,
				               vStartPos, // start
				               vEndPos,
							   //pOtherPM->GetPreviousTimeStepPos() , pOtherPM->GetDesiredPos() - pOtherPM->GetPreviousTimeStepPos(),

							   //pOtherPM->GetPreviousTimeStepPos() , pOtherPM->GetDesiredPos(),
					
				               fMoveTillHit,vHitNormal);

	BREAKPOINT(   *((unsigned long*)&vHitNormal.x) == 0xffc00000 );

	return bInter;

}

void CBasicPhysics::ApplyForce(const CVec3f& vForce)
{
	if (m_bInAir)
		m_vActiveForces += vForce*0.1f;
	else // on ground
		m_vActiveForces += vForce;
}
