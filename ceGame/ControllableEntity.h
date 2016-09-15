#ifndef CRIMSON_CONTROLLABLE_ENTITY_H
#define CRIMSON_CONTROLLABLE_ENTITY_H

#pragma once

#include "Entity.h"
#include "Math3d.h"

#include "Entity.h"


#define PLAYER_ANIM_TRIGGERED_ACTION_NOT_INITED            0
#define PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_1    1
#define PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_2    2
#define PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PARTICLE_SYSTEM 3 
#define PLAYER_ANIM_TRIGGERED_ACTION_SOUND                 4







// requests

#define PLAYER_ACTION_REQUEST_FLAG_RUN_FORWARD    (1<<0)
#define PLAYER_ACTION_REQUEST_FLAG_RUN_BACKWARDS  (1<<1)
#define PLAYER_ACTION_REQUEST_FLAG_WALK_FORWARD   (1<<2)
#define PLAYER_ACTION_REQUEST_FLAG_WALK_BACKWARDS (1<<3)
#define PLAYER_ACTION_REQUEST_FLAG_STRAFE_LEFT    (1<<4)
#define PLAYER_ACTION_REQUEST_FLAG_STRAFE_RIGHT   (1<<5)
#define PLAYER_ACTION_REQUEST_FLAG_ROTATE_LEFT    (1<<6)
#define PLAYER_ACTION_REQUEST_FLAG_ROTATE_RIGHT   (1<<7)
#define PLAYER_ACTION_REQUEST_FLAG_JUMP           (1<<8)
#define PLAYER_ACTION_REQUEST_FLAG_ATTACK_1       (1<<9)
#define PLAYER_ACTION_REQUEST_FLAG_ATTACK_2       (1<<10)

// actual move state
// used by networked controllable to decide which animation to show 

#define PLAYER_MOVE_ANIMATION_IDLE               0x01
#define PLAYER_MOVE_ANIMATION_RUN_FORWARD        0x02
#define PLAYER_MOVE_ANIMATION_RUN_BACKWARDS      0x03
#define PLAYER_MOVE_ANIMATION_WALK_FORWARD       0x04
#define PLAYER_MOVE_ANIMATION_WALK_BACKWARDS     0x05
#define PLAYER_MOVE_ANIMATION_STRAFE_LEFT        0x06
#define PLAYER_MOVE_ANIMATION_STRAFE_RIGHT       0x07
#define PLAYER_MOVE_ANIMATION_RUN_STRAFE_LEFT    0x08
#define PLAYER_MOVE_ANIMATION_RUN_STRAFE_RIGHT   0x09
#define PLAYER_MOVE_ANIMATION_ROTATE_LEFT        0x0A
#define PLAYER_MOVE_ANIMATION_ROTATE_RIGHT       0x0B
#define PLAYER_MOVE_ANIMATION_JUMP               0x0C

#define PLAYER_MOVE_ANIMATION_ATTACK_1           0x10
#define PLAYER_MOVE_ANIMATION_ATTACK_2           0x20



#define ATTACK_1_WAIT_TIME 200
#define ATTACK_2_WAIT_TIME 150



class CControllableEntity : public CEntity
{
public:
	CControllableEntity();
	~CControllableEntity();


	virtual void HandleActionRequests(DWORD dwActionFlags, float fDeltaTime) = 0;

	virtual void TriggerFrameActions(const tFrameActions* pFrameActions);


	bool IsControllable(void) { return true;};

	void Revive(void);

	void SetDying(void);
	bool IsDying(void) const;

	virtual bool RelinkMe(void) { return true; };
		
	void Jump(void);


	void Move_WalkForward(float fDelta);
	void Move_WalkBackwards(float fDelta);

	void Move_Up_DEBUG(float fDelta);

	void Move_RunForward(float fDelta);
	void Move_RunBackwards(float fDelta);

	void Move_StrafeLeft(float fDelta);
	void Move_StrafeRight(float fDelta);

	void Move_RotateLeft(float fDelta/*,bool bOnlyAnimate*/);
	void Move_RotateRight(float fDelta/*,bool bOnlyAnimate*/);


	virtual bool Attack(int iAttackType);

	void AddAngles(const CVec3f& vAngles);

	void SetAngles(const CVec3f& vAngles);


	
	
	virtual CVec3f GetCurrentBindPosition(int iBindNum) const;

	///////////////////////////////////////////////////////////////////////

	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; };


	//void SetRenderRenderModel(bool bRender) { m_bRenderRenderModel=bRender;};
	void SetRenderRenderModel(bool bRender);

	void UpdateRenderModel(float fDeltaTime,bool bVisible);
	void Render(int iTech);
	

	void	SetLife(int iLife);
	int		GetLife(void) const { return m_iLife;};
	

	// ammo type can be 1 or 2
	int		GetAmmo(int iNum) const;

	void PickUp(DWORD dwItemType);


	unsigned char GetMoveState() const { return m_cMoveState;};
	void SetMoveState(unsigned char cMoveState) { m_cMoveState = cMoveState; };

	DWORD m_dwRequests;

protected:
	unsigned char m_cMoveState;
	bool m_bInsideAttackRequest;
	

private:
	//
	int m_iLife;
	int m_iAmmo_1;
	int m_iAmmo_2;
	
	
	float m_fSpeed;
	float m_fJumpSpeed;

	bool m_bRenderRenderModel;
	bool m_bRenderRenderModel_ForShadow;

	bool m_bDying;	
	

	double m_dLastAttackTime_Attack1;
	double m_dLastAttackTime_Attack2;

	// touching related

	/*CSmartPointer<IMaterial_Export> m_TouchingMaterials[30];
	
	CSmartPointer<CParticleSystem_Shared> m_TouchingPS[30];*/

};



#endif