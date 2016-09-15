#include "stdafx.h"
#include "NetPlayerEntity.h"
#include "Character.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#define MINIMAL_MOVEMENT 0.4f

CNetPlayerEntity::CNetPlayerEntity()
{
	m_bAttackingLoopable_attack1 = false;;
    m_bAttackingLoopable_attack2 = false;

	m_bRotatingLeftInPlace = false;
	m_bRotatingRightInPlace = false;

}

CNetPlayerEntity::~CNetPlayerEntity()
{

}



void CNetPlayerEntity::TriggerAppropriateAnimation(float fDeltaTime)
{
	CVec3f vDelta = m_pPhysicsModel->GetCurrentTimeStepPos() - m_pPhysicsModel->GetPreviousTimeStepPos();


	float fDistanceToFloor = g_pQ3Map->GetDistanceToFloor(m_pPhysicsModel->GetPosition(),this);
	

	/*if (fDistanceToFloor>10.f)
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_IDLE);
	else
	{
		// if any movement on x/z coords
		if (vDelta.x > MINIMAL_MOVEMENT  || vDelta.x < -MINIMAL_MOVEMENT || vDelta.z > MINIMAL_MOVEMENT  || vDelta.z < -MINIMAL_MOVEMENT)
		{
			((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_RUN);	
		}
		else
			((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_IDLE);		
	}*/


	//TODO: add that if angles changed and no move then rotate in place animation
	
	unsigned char ucMove   = m_cMoveState & 0x0F;
	unsigned char ucAction = m_cMoveState & 0xF0;

	switch(ucMove)
	{
		case PLAYER_MOVE_ANIMATION_IDLE:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_IDLE);				
		break;

		case PLAYER_MOVE_ANIMATION_RUN_FORWARD:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_RUN);				
		break;

		case PLAYER_MOVE_ANIMATION_RUN_BACKWARDS:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_RUN_BACK);				
		break;

		case PLAYER_MOVE_ANIMATION_WALK_FORWARD:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_WALK);				
		break;

		case PLAYER_MOVE_ANIMATION_WALK_BACKWARDS:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_WALK_BACK);				
		break;

		case PLAYER_MOVE_ANIMATION_STRAFE_LEFT:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_STRAFE_LEFT);				
		break;

		case PLAYER_MOVE_ANIMATION_STRAFE_RIGHT:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_STRAFE_RIGHT);				
		break;

		case PLAYER_MOVE_ANIMATION_ROTATE_LEFT:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_LEFT);		
		m_bRotatingLeftInPlace = true;
		break;

		case PLAYER_MOVE_ANIMATION_ROTATE_RIGHT:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_RIGHT);				
		m_bRotatingRightInPlace = true;
		break;

		default:
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_IDLE);				
		//BREAKPOINT(1);
		break;
	};


	if (ucMove!=PLAYER_MOVE_ANIMATION_ROTATE_LEFT)
		m_bRotatingLeftInPlace = false;

	if (ucMove!=PLAYER_MOVE_ANIMATION_ROTATE_RIGHT)
		m_bRotatingRightInPlace = false;


	if (m_bRotatingLeftInPlace)
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_LEFT);	;

	if (m_bRotatingRightInPlace)
		((CCharacter*)m_pRenderModel)->RequestMoveState(ANI_TYPE_ROTATE_RIGHT);	;




	

	switch(ucAction)
	{
		case PLAYER_MOVE_ANIMATION_ATTACK_1:
		//g_TextMessages.AddMessage("Client is attacking!!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_1);				

		if ( ((CCharacter*)m_pRenderModel)->GetMD5Inst()->GetShareIsLoopable_Attack1())
			m_bAttackingLoopable_attack1 = true;
		break;

		case PLAYER_MOVE_ANIMATION_ATTACK_2:
		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_2);				
		
		if ( ((CCharacter*)m_pRenderModel)->GetMD5Inst()->GetShareIsLoopable_Attack2())
			m_bAttackingLoopable_attack2 = true;

		break;

		default: // if i add stuff then default won't work properly!!

			m_bAttackingLoopable_attack1 = false;
			m_bAttackingLoopable_attack1 = false;
		//BREAKPOINT(1);
	}


	if (m_bAttackingLoopable_attack1)
		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_1);	
	else
	if (m_bAttackingLoopable_attack2)
		((CCharacter*)m_pRenderModel)->PlayAction(ANI_TYPE_ATTACK_2);	
	


}



void CNetPlayerEntity::HandleActionRequests(DWORD dwActionFlags, float fDeltaTime)
{	
	
}