#include "stdafx.h"
#include "HumanPlayerEntity.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

CHumanPlayerEntity::CHumanPlayerEntity()
{
	m_cMoveState = PLAYER_MOVE_ANIMATION_IDLE;

	m_iTimesSentAttackInfo_Attack1 = -1;
	m_iTimesSentAttackInfo_Attack2 = -1;

	
};

CHumanPlayerEntity::~CHumanPlayerEntity()
{

};

void CHumanPlayerEntity::HandleActionRequests(DWORD dwActionFlags, float fDeltaTime)
{
	m_dwRequests = dwActionFlags;

	//TODO: handle run+strafe

	bool bMoveAction = false;

	if (! 
		((dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_RUN_FORWARD) && (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_RUN_BACKWARDS))) // if both ignore
	{
		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_RUN_FORWARD)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_RUN_FORWARD;
			Move_RunForward(fDeltaTime);
			bMoveAction = true;
		}


		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_RUN_BACKWARDS)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_RUN_BACKWARDS;
			Move_RunBackwards(fDeltaTime);
			bMoveAction = true;
		}

		
	}

	if (! 
		((dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_STRAFE_LEFT) && (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_STRAFE_RIGHT))) // if both ignore
	{
		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_STRAFE_LEFT)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_STRAFE_LEFT;
			Move_StrafeLeft(fDeltaTime);
			bMoveAction = true;
		}


		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_STRAFE_RIGHT)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_STRAFE_RIGHT;
			Move_StrafeRight(fDeltaTime);
			bMoveAction = true;
		}

		
	}


	if (! 
		((dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ROTATE_LEFT) && (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ROTATE_RIGHT))) // if both ignore
	{
		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ROTATE_LEFT)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_ROTATE_LEFT;
			Move_RotateLeft(fDeltaTime/*,false*/);
			bMoveAction = true;
		}


		if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ROTATE_RIGHT)
		{
			m_cMoveState = PLAYER_MOVE_ANIMATION_ROTATE_RIGHT;
			Move_RotateRight(fDeltaTime/*,false*/);
			bMoveAction = true;
		}

		
	}

	if (!bMoveAction)
		m_cMoveState = PLAYER_MOVE_ANIMATION_IDLE;


	

	if (!(dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ATTACK_1))
	{
		m_bInsideAttackRequest = false;
	}

	if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ATTACK_1)
	{			
		m_iTimesSentAttackInfo_Attack1 = 1;
		// restore!
		if (Attack(1))
			m_cMoveState|= PLAYER_MOVE_ANIMATION_ATTACK_1;	
	}
	else if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_ATTACK_2)
	{
		m_iTimesSentAttackInfo_Attack2 = 1;
		// restore!
		if (Attack(2))
			m_cMoveState|= PLAYER_MOVE_ANIMATION_ATTACK_2;
	}

	if (dwActionFlags&PLAYER_ACTION_REQUEST_FLAG_JUMP)
	{
		m_cMoveState = PLAYER_MOVE_ANIMATION_JUMP;
		Jump();
	}

	if (m_iTimesSentAttackInfo_Attack1>0)
	{
		m_cMoveState|=PLAYER_MOVE_ANIMATION_ATTACK_1;
		m_iTimesSentAttackInfo_Attack1++;
	}

	if (m_iTimesSentAttackInfo_Attack1>3)
		m_iTimesSentAttackInfo_Attack1=-1;


	if (m_iTimesSentAttackInfo_Attack2>0)
	{
		m_cMoveState|=PLAYER_MOVE_ANIMATION_ATTACK_2;
		m_iTimesSentAttackInfo_Attack2++;
	}

	if (m_iTimesSentAttackInfo_Attack2>3)
		m_iTimesSentAttackInfo_Attack2=-1;




}