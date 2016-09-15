#ifndef CRIMSON_MD5_ANIM_H
#define CRIMSON_MD5_ANIM_H

#pragma once

#include "MD5Mesh.h"
#include "RenderModel.h"

#define ANIMATED_NODE_TRANS_X (1<<0)
#define ANIMATED_NODE_TRANS_Y (1<<1)
#define ANIMATED_NODE_TRANS_Z (1<<2)
#define ANIMATED_NODE_ROT_X   (1<<3)
#define ANIMATED_NODE_ROT_Y   (1<<4)
#define ANIMATED_NODE_ROT_Z   (1<<5)


struct tMD5AnimFrame
{
	tMD5AnimFrame() { pData = NULL; };
	~tMD5AnimFrame() { SafeDeleteArray(pData); };


	float* pData; // animated components aray - compact version
};





// i should add:
// ANI_TYPE_IN_AIR
// ANI_TYPE_ROTATE_LEFT
// ANI_TYPE_ROTATE_RIGHT

#define ANI_TYPE_ATTACK_1 0
#define ANI_TYPE_ATTACK_2 1
#define ANI_TYPE_WALK 2
#define ANI_TYPE_WALK_BACK 3
#define ANI_TYPE_RUN  4
#define ANI_TYPE_RUN_BACK  5
#define ANI_TYPE_JUMP 6
#define ANI_TYPE_MID_AIR 7
#define ANI_TYPE_LAND 8
#define ANI_TYPE_STRAFE_LEFT 9
#define ANI_TYPE_STRAFE_RIGHT 10
#define ANI_TYPE_RUN_STRAFE_LEFT 11
#define ANI_TYPE_RUN_STRAFE_RIGHT 12
#define ANI_TYPE_ROTATE_LEFT 13
#define ANI_TYPE_ROTATE_RIGHT 14
#define ANI_TYPE_IDLE 15
#define ANI_TYPE_PAIN 16
#define ANI_TYPE_NUM  17


class CMD5Anim
{
public:
	CMD5Anim();
	~CMD5Anim();

	bool Load(const char* pchFileName);

	tMD5Joint* GetBaseJoints(void) const { return m_pJoints;};

	const tMD5AnimFrame* GetAnimFrames(void) const { return m_pFrames;};

	int GetJointsNum(void) const { return m_iJointsNum;};
	

	// -1 means not found
	int GetJointsIndex(const char* pcName) const;

	int GetFramesNum(void) const { return m_iFramesNum;};

	float GetAnimationFPS(void) const { return m_fFrameRate;};

	void SetLoopable(bool bLoopable);
	bool GetLoopable(void);

	void SetAniType(DWORD dwType);
	DWORD GetAniType();

	void SetPriority(int iChannel,int iPrio);
	int GetPriority(int iChannel) const;

	void ReverseFramesOrder(void);
	

	// frame triggered actions

	void SetFrameActions(int iFrameIndex,tFrameActions& frameActions);

	const tFrameActions* GetFramesActions(int iFrameIndex);
	

private:

	bool m_bLoopable;
	DWORD m_dwAniType;

	float m_fFrameRate;

	int        m_iJointsNum;
	tMD5Joint* m_pJoints;

	int            m_iFramesNum;
	tMD5AnimFrame* m_pFrames;

	int   m_iLegsPriority;
	int   m_iTorsoPriority;

	char* m_pchName;

	tFrameActions* m_pFrameActions;

};


#endif