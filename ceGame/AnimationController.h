#ifndef CRIMSON_ANIMATION_CONTROLLER_H
#define CRIMSON_ANIMATION_CONTROLLER_H

#pragma once

#include "MD5Anim.h"
#include "MD5Mesh.h"
#include "MD5Instance.h"
#include "Entity.h"

#include <vector>

#include <map>


struct tWeightedAnim
{
	int iAnimationNum;
	//float fStartTime;
	float fRunningTime;
	float fWeight;
	bool bLooped;
	bool bMoveAni;
	
	bool bRequestFadeOut;
	bool bRequestPlay;
};

typedef std::vector<tWeightedAnim*>                     WEIGHTED_ANIMS_MAP;
typedef std::vector<tWeightedAnim*>::iterator           WEIGHTED_ANIMS_MAP_IT;

//#define MAX_SIMULTANEOUS_BLENDED_ANIMATIONS 6 

#define CHANNEL_LEGS    0  
#define CHANNEL_TORSO   1
#define CHANNELS_NUM    2



//TODO: perhaps have a map of fade in/out according to the animation type
//#define FADE_IN_OUT_TIME 0.3f

#define FADE_IN_OUT_TIME 0.1f

class CMD5Inst;
class CAnimController
{
public:
	CAnimController(const CMD5Mesh* pMesh,CMD5Anim *const* pAnims,tMD5BindJoint* pCurrentBindJoints,CMD5Inst* pInstance,CEntity* pParentEntity);
	~CAnimController();


	void PlayAction(int iAnimNum);


	bool InterpolateBetweenFrames(tMD5BindJoint* pJoints,const int* pIndices, int iNumIndices, float fTimeSinceStart, int iAnimNum,bool bLoopable);

	void BlendJointsIndices(tMD5BindJoint* pJoints1, const tMD5BindJoint* pJoints2, float fT, const int* pIndices, int iNumIndices);

	void GetFrames(int &iFrameFrom, int &iFrameTo, float fTimeSinceStart,int iAnimNum);



	void InitWeightedAnims(void);

	void UpdateWeightedAnims(float fDeltaTime);

	void ComputeFinalSkeleton(float fDeltaTime);


	// debug
	tMD5BindJoint* GetCurrentBindJoints(void) { return m_pCurrentBindJoints; };

	void ActivateFK(void);

	void SetPriorityOrderedChannels(void);


	void RequestMoveState(int iMoveState);

	void FindFramesActions(int iAnimNum);

private:
	CAnimController();

	tMD5BindJoint* m_pTEMPBindJoints;
	
	tMD5BindJoint* m_pCurrentBindJoints;
	CMD5Anim *const* m_ppAnimations;
	const CMD5Mesh* m_pMesh;
	CEntity* m_pParentEntity;
	CMD5Inst* m_pInstance;

	tWeightedAnim* m_PriorityOrderedChannels[CHANNELS_NUM][ANI_TYPE_NUM];
	tWeightedAnim m_AnimOrderedChannels[CHANNELS_NUM][ANI_TYPE_NUM];

	

	float m_fFadeFrames;


	// Move States are: (the player can only request ONE of those at a given time)
	// it will fade in and rest are faded out if needed.
	//CHANNEL_SLOT_WALK         
	//CHANNEL_SLOT_WALK_BACK    
	//CHANNEL_SLOT_RUN          	
	//CHANNEL_SLOT_IDLE         
	//CHANNEL_SLOT_STRAFE_LEFT  
	//CHANNEL_SLOT_STRAFE_RIGHT 
	int m_iMoveState; 
	

	// frame anim triggers

	int*           m_pRenderedFromFrame_CURRENT;
	int*           m_pRenderedFromFrame_PREVIOUS;

	//const tFrameActions* m_pCurrentFrameAnimationsTriggers[FRAME_ACTIONS_MAX*10];

	//tFrameActions const** m_ppCurrentFrameAnimationsTriggers;

	
};



#endif