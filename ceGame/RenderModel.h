#ifndef CRIMSON_RENDER_MODEL_H
#define CRIMSON_RENDER_MODEL_H

#pragma once

#include "Light.h"

#include "Math3D.h"

#define FRAME_ACTIONS_MAX 50


struct tFrameAction
{
	tFrameAction():pData(NULL){};

	unsigned int uiAction;
	int           iJointIndex;

	void* pData;
};

struct tFrameActions
{

	tFrameActions():iActionsNum(0),pActions(NULL) {};

	int iActionsNum;

	tFrameAction* pActions;	
};

class IRenderModel
{
public:
	IRenderModel() {m_bAnglesBlending=true;m_vPosition.Set(0.f,0.f,0.f);};
	virtual ~IRenderModel() {};


	virtual void ClearEffectingLights(void) = 0;
	virtual void AddEffectingLight(ILight_Export* pLight) = 0;

	virtual void SetPosition(const CVec3f& vPos) = 0;
	
	
	virtual void   SetAngles(const CVec3f& vAngles) = 0;
	virtual CVec3f GetAngles(void) const= 0;

	
	virtual void BlendAnglesState(float fAlpha) =0;
	virtual CVec3f GetCurrentAngles(void) const=0;
	virtual CVec3f GetPreviousAngles(void) const=0;
	
	void SetAnglesBlending(bool bBlending) { m_bAnglesBlending = bBlending; };
	bool GetAnglesBlending(void) const { return m_bAnglesBlending;};

	virtual void Update(float fDeltaTime,bool bVisible) = 0;
	
	virtual const char* GetName(void) const = 0;


	virtual void Render(int iTech) = 0;	

	//virtual void PlayAction(int iAnimNum) {};
	//virtual void RequestMoveState(int iMoveState) {};

	//virtual CVec3f GetCurrentFinalJointPosition(int iJointIndex) {return m_vPosition;};
protected:
	bool m_bAnglesBlending;

	CVec3f m_vPosition;


};


#endif
