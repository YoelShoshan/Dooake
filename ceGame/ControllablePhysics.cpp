#include "stdafx.h"
#include "ControllablePhysics.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Input.h"
#include "MD5Anim.h"

CControllablePhysics::CControllablePhysics()
{
	m_bInAir = true;

	m_vPreviousTimeStepPos.Set(0.f,0.f,0.f);
	m_vCurrentTimeStepPos.Set(0.f,0.f,0.f);
	m_vPos.Set(0.f,0.f,0.f);
	m_vDesiredPos.Set(0.f,0.f,0.f);
	m_vVelocity.Set(0.f,0.f,0.f);
}

CControllablePhysics::~CControllablePhysics()
{

}

void CControllablePhysics::FindPhysicsStepDesiredPosition(float fStep)
{
	BREAKPOINT(   *((unsigned long*)&m_vPreviousTimeStepPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vCurrentTimeStepPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vDesiredPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vVelocity.x) == 0xffc00000 );

	//ProcessInputRequests();

	//m_vVelocity+=m_vActiveForces;
	
	if (m_vVelocity.Length()<VELOCITY_MINIMUM_LENGTH)
		m_vVelocity.Set(0.f,0.f,0.f);

	m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;

	BREAKPOINT(!m_pParentEntity);

	m_fDistanceToFloor = g_pQ3Map->GetDistanceToFloor(m_vPreviousTimeStepPos,m_pParentEntity);

	CVec3f vGravity(0.f,0.f,0.f);
	

	
	m_bInAir = true;

	if (m_fDistanceToFloor>7.f)
		vGravity.Set(0.f,-26.f,0.f);
		//vGravity.Set(0.f,-50.f,0.f);
	else
	{
		m_bInAir = false;

		/*if (m_fDistanceToFloor<5.f)
			// original
			//m_vActiveForces+=CVec3f(0.f,0.5f,0.f);		
			m_vActiveForces+=CVec3f(0.f,2.5f,0.f);		*/


		if (m_fDistanceToFloor<1.f)
			m_vActiveForces+=CVec3f(0.f,2.5f,0.f);		
	}


	
	


	
	/*if (m_fDistanceToFloor>3.f)
	{
		m_bInAir = true;
		vGravity.Set(0.f,-10.f,0.f);		
	}
	else // on ground
	{
		m_bInAir = false;
		m_vVelocity.y *=0.03f;
	}*/
	

	/*if (m_bInAir)
		m_vActiveForces+=vGravity*fStep;*/



	//m_vVelocity = ( m_vVelocity*fStep)+(m_vActiveForces*fStep)+(vGravity*fStep);


	// debug - try fly mode!
	//vGravity.Set(0.f,0.f,0.f);

	m_vVelocity += (m_vActiveForces*fStep)+(vGravity*fStep);

	
	m_vDesiredPos = m_vCurrentTimeStepPos+m_vVelocity;


	m_vActiveForces.Set(0.f,0.f,0.f);

	BREAKPOINT(   *((unsigned long*)&m_vPreviousTimeStepPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vCurrentTimeStepPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vPos.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vVelocity.x) == 0xffc00000 );
	BREAKPOINT(   *((unsigned long*)&m_vDesiredPos.x) == 0xffc00000 );

}



/*void CControllablePhysics::ProcessInputRequests(void)
{
	BREAKPOINT(!m_pParent);
	
	if (m_bInput_WalkForward)
	{
		CVec3f vAngles = m_pParent->GetRenderModel()->GetAngles();
	
		CVec3f vRotateVec(0.f,0.f,-1.f);
		vRotateVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotateVec);


		float fSpeed = 50.f;

		ApplyForce(vRotateVec*fSpeed*2.5f);

	}

}

void CControllablePhysics::ClearInputBooleans(void)
{
	m_bInput_WalkForward = false;
	m_bInput_WalkBackwards = false;

}*/