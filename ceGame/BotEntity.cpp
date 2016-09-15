#include "stdafx.h"
#include "BotEntity.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

CBotEntity::CBotEntity()
{

}

CBotEntity::~CBotEntity()
{

}

void  CBotEntity::ThinkAndAct(float fDeltaTime)
{

	//m_pRenderModel->SetAngles(m_pRenderModel->GetAngles());
	//Attack(1);
	//return;

	static int iBlah = 0;
	static int iTimeTillNextDirChange=1000;
	static bool bDirection=false;
	
	
	
	static double dShootCycle = 0.0;

	dShootCycle+=fDeltaTime;

	//removing for debug
	if (dShootCycle<4000)
	//if (0)
		Attack(1);
	else
		m_bInsideAttackRequest = false;

	if (dShootCycle>8000)
		dShootCycle=0.0;



	iBlah++;
	if (iBlah>iTimeTillNextDirChange)
	{
		iBlah=0;
		iTimeTillNextDirChange = rand()%1000+500;
		bDirection=!bDirection;
	}
	
	
	//Attack(1);


	/*if (bDirection)
		Move_RotateLeft(fDeltaTime/2.f);
	else
		Move_RotateRight(fDeltaTime/2.f);*/

	if (bDirection)
		m_pRenderModel->SetAngles(m_pRenderModel->GetAngles()+CVec3f(0.f,fDeltaTime*0.1f/2.f,0.f));
	else
		m_pRenderModel->SetAngles(m_pRenderModel->GetAngles()-CVec3f(0.f,fDeltaTime*0.1f/2.f,0.f));


	


	
	//return;

	CVec3f vAngles = m_pRenderModel->GetAngles();
	vAngles.x = 0.f;
	vAngles.z = 0.f;
	
	CVec3f vRotatedVec(0.f,0.f,-1.f);
	vRotatedVec	= RotateVectorByAngle(vAngles.x,vAngles.y-90.f,vRotatedVec);

	CVec3f vNewPos = m_pPhysicsModel->GetPosition() + vRotatedVec * 100.f;

	float fDistanceToFloor = g_pQ3Map->GetDistanceToFloor(vNewPos,this);
		
	if (fDistanceToFloor>7.f)
	{
		//Move_WalkBackwards(fDeltaTime);
		return;
	}
		
	

	// RESTORE for moving
	Move_RunForward(fDeltaTime);


	// debug
	/*const int** ppArray = new const int*[30];
	(*ppArray[10]) = 15;*/
}

void CBotEntity::HandleActionRequests(DWORD dwActionFlags, float fDeltaTime)
{
	// ignoring dwActionFlags

	ThinkAndAct(fDeltaTime);		
}

/*void CBotEntity::FindPhysicsStepDesiredPosition(float fStep)
{
	ThinkAndAct(fStep);

	__super::FindPhysicsStepDesiredPosition(fStep);
}*/