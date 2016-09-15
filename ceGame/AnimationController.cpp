#include "stdafx.h"
#include "AnimationController.h"


#include "Timer_IE.h"
extern ITimer* g_pTimer;
#include "ceLibrary.h"

/*extern float        g_fDEBUGRotJointX;
extern float        g_fDEBUGRotJointY;
extern float        g_fDEBUGRotJointZ;
extern int          g_iDEBUGJoint;*/

extern bool g_bBlendingIn;
extern bool g_bBlendingOut;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include <map>

CAnimController::CAnimController()
{
	// leave this empty!


}


CAnimController::~CAnimController()
{
	SafeDeleteArray(m_pTEMPBindJoints);


	SafeDeleteArray(m_pRenderedFromFrame_CURRENT);
	SafeDeleteArray(m_pRenderedFromFrame_PREVIOUS);
	//SafeDeleteArray(m_ppCurrentFrameAnimationsTriggers);
}

CAnimController::CAnimController(const CMD5Mesh* pMesh,CMD5Anim *const* pAnims,tMD5BindJoint* pCurrentBindJoints,CMD5Inst* pInstance,CEntity* pParentEntity)
{

	m_pParentEntity = pParentEntity;

	m_iMoveState = ANI_TYPE_IDLE;

	m_pMesh = pMesh;
	m_ppAnimations = pAnims;
	m_pCurrentBindJoints = pCurrentBindJoints;
	m_pInstance = pInstance;


	m_pTEMPBindJoints = new tMD5BindJoint[m_pMesh->GetJointsNum()];

	InitWeightedAnims();

	SetPriorityOrderedChannels();

	//m_pAnimatedRends = pAnimatedRends;


	int iAnimsNum = m_pInstance->GetSharedAnimationsNum();

	m_pRenderedFromFrame_CURRENT = new int[iAnimsNum];

	for (int i=0;i<iAnimsNum;i++)
		m_pRenderedFromFrame_CURRENT[i] = -1;

	m_pRenderedFromFrame_PREVIOUS = new int[iAnimsNum];

	for (int i=0;i<iAnimsNum;i++)
		m_pRenderedFromFrame_PREVIOUS[i] = -1;


	//m_ppCurrentFrameAnimationsTriggers = new const tFrameActions*[FRAME_ACTIONS_MAX*10];


}



void CAnimController::UpdateWeightedAnims(float fDeltaTime)
{
	float fFadeTime=FADE_IN_OUT_TIME;


	g_bBlendingIn = false;
	g_bBlendingOut = false;

	for (int c=0;c<CHANNELS_NUM;c++)
		for (int s=0;s<ANI_TYPE_NUM;s++)
		{
			/*if (s==CHANNEL_SLOT_IDLE)
				continue; // ignore idle*/

			// move state

			int iAnimType = m_ppAnimations[m_PriorityOrderedChannels[c][s]->iAnimationNum]->GetAniType();

			if (m_PriorityOrderedChannels[c][s]->bMoveAni)
			{

				if (iAnimType == m_iMoveState)
				{
					m_PriorityOrderedChannels[c][s]->fWeight += fDeltaTime/fFadeTime;

					if (m_PriorityOrderedChannels[c][s]->fWeight > 1.f)
						m_PriorityOrderedChannels[c][s]->fWeight = 1.f;
				} else
				{
					m_PriorityOrderedChannels[c][s]->fWeight -= fDeltaTime/fFadeTime;

					if (m_PriorityOrderedChannels[c][s]->fWeight < 0.f)
						m_PriorityOrderedChannels[c][s]->fWeight = 0.f;

				}

			}

		
			if (m_PriorityOrderedChannels[c][s]->bRequestPlay)
			{
				if (m_PriorityOrderedChannels[c][s]->fWeight < 1.f) 
				{
					g_bBlendingIn = true;
					m_PriorityOrderedChannels[c][s]->fWeight += fDeltaTime/fFadeTime;
				}
				

				if (m_PriorityOrderedChannels[c][s]->fWeight > 1.f)
				{
					m_PriorityOrderedChannels[c][s]->fWeight = 1.f;

					if (!m_PriorityOrderedChannels[c][s]->bLooped)
						m_PriorityOrderedChannels[c][s]->bRequestPlay = false;
				}

			} else if (m_PriorityOrderedChannels[c][s]->bRequestFadeOut)
			{

				if (m_PriorityOrderedChannels[c][s]->fWeight>0.f)
				{
					g_bBlendingOut = true;
					m_PriorityOrderedChannels[c][s]->fWeight -= fDeltaTime/fFadeTime;
				}


				if (m_PriorityOrderedChannels[c][s]->fWeight<=0.f)
				{
					// fade out already occured, do not request it anymore
					m_PriorityOrderedChannels[c][s]->bRequestFadeOut = false;
					m_PriorityOrderedChannels[c][s]->fWeight=0.f;
				}							
			}


				// a loopable animation that isn't a move animation
			// for example - a machine gun
			if (m_PriorityOrderedChannels[c][s]->bLooped && !m_PriorityOrderedChannels[c][s]->bMoveAni)
			{
				// if no one requested it, it should fade away
				if (!m_PriorityOrderedChannels[c][s]->bRequestPlay)
				{
					if (m_PriorityOrderedChannels[c][s]->fWeight>0.f)
					{
						g_bBlendingOut = true;
						m_PriorityOrderedChannels[c][s]->fWeight -= fDeltaTime/fFadeTime;
					}

					if (m_PriorityOrderedChannels[c][s]->fWeight < 0.f)
						m_PriorityOrderedChannels[c][s]->fWeight = 0.f;

				} else
					m_PriorityOrderedChannels[c][s]->bRequestPlay = false;

				continue;

			}


		}

		
}

void CAnimController::InitWeightedAnims(void)
{
	for (int c=0;c<CHANNELS_NUM;c++)
		for (int s=0;s<ANI_TYPE_NUM;s++)
		{	
			m_AnimOrderedChannels[c][s].iAnimationNum=s;

			int iAnimType = m_ppAnimations[m_AnimOrderedChannels[c][s].iAnimationNum]->GetAniType();

			m_AnimOrderedChannels[c][s].fRunningTime = 0.f;
			m_AnimOrderedChannels[c][s].bRequestPlay=false;

			//if (s==CHANNEL_SLOT_IDLE)
			if (iAnimType == ANI_TYPE_IDLE)
				m_AnimOrderedChannels[c][s].fWeight=1.f;
			else
				m_AnimOrderedChannels[c][s].fWeight=0.f;		
			
			

			if (m_ppAnimations[m_AnimOrderedChannels[c][s].iAnimationNum]->GetLoopable())
				m_AnimOrderedChannels[c][s].bLooped=true;
			else
				m_AnimOrderedChannels[c][s].bLooped=false;



			// NOTICE: add new move anims here!

			if ( (iAnimType == ANI_TYPE_WALK) ||
				(iAnimType == ANI_TYPE_WALK_BACK) ||
				(iAnimType == ANI_TYPE_RUN) ||
				(iAnimType == ANI_TYPE_RUN_BACK) ||
				(iAnimType == ANI_TYPE_IDLE) ||
				(iAnimType == ANI_TYPE_ROTATE_LEFT) ||
				(iAnimType == ANI_TYPE_ROTATE_RIGHT) ||
				(iAnimType == ANI_TYPE_STRAFE_LEFT) ||
				(iAnimType == ANI_TYPE_STRAFE_RIGHT) ||
				(iAnimType == ANI_TYPE_RUN_STRAFE_LEFT) ||
				(iAnimType == ANI_TYPE_RUN_STRAFE_RIGHT) 								
				)
				m_AnimOrderedChannels[c][s].bMoveAni = true;
			else
				m_AnimOrderedChannels[c][s].bMoveAni = false;
			
			//m_Channels[c][s].bPassedAnimEnd = false;

			m_AnimOrderedChannels[c][s].bRequestFadeOut = false;

			//m_Channels[c][s].bAniEnded = false; 

			//m_Channels[c][s].bRequestBlendTo = false;

			//m_Channels[c][s].bFadeOutStarted = false; 
			//m_Channels[c][s].dwEffectChannels = EFFECT_ALL_CHANNELS;
		}
}

void CAnimController::GetFrames(int &iFrameFrom, int &iFrameTo, float fTimeSinceStart,int iAnimNum)
{
	bool bLoopable = m_ppAnimations[iAnimNum]->GetLoopable();
	int  iFPS = m_ppAnimations[iAnimNum]->GetAnimationFPS();
	int  iFramesNum = m_ppAnimations[iAnimNum]->GetFramesNum();

	
	iFrameFrom = (int)(floorf(fTimeSinceStart));
	//iFrameTo   = (int)(floorf(fTimeSinceStart))+1;

	iFrameTo   = (int)(ceilf(fTimeSinceStart));



	///////////////////////////////////////////////////
	// a fix to support slow motion in timer
	/*if (iFrameFrom < 0);
		iFrameFrom *= -1;

	if (iFrameTo < 0);
		iFrameTo *= -1;*/
	///////////////////////////////////////////////////


	iFrameFrom %= iFramesNum;
	iFrameTo %= iFramesNum;

	BREAKPOINT(iFrameFrom<0);
	BREAKPOINT(iFrameTo<0);

	


	/*if (iFrameTo == 0)
	{
		iFrameFrom = 0;
		iFrameTo   = 1;
	}*/
}


void CAnimController::FindFramesActions(int iAnimNum)
{

	BREAKPOINT(iAnimNum>= m_pInstance->GetSharedAnimationsNum());

	int iFrameTriggeredActions = 0;
	
	const tFrameActions* pFrameActions = NULL;

	int iFramesNum = m_ppAnimations[iAnimNum]->GetFramesNum();

	char temp[500];

	for (int i=0;i<iFramesNum;i++)
	{
		if (m_pRenderedFromFrame_PREVIOUS[iAnimNum]<0) // not init
		{
			for (int j=0;j<m_pRenderedFromFrame_CURRENT[iAnimNum]+1;j++)
			{
				pFrameActions = m_ppAnimations[iAnimNum]->GetFramesActions(j);
				if (pFrameActions)
				{
					//m_ppCurrentFrameAnimationsTriggers[iFrameTriggeredActions++] = pFrameActions;						

					m_pParentEntity->TriggerFrameActions(pFrameActions);
					
					/*sprintf(temp,"1)Triggered frame action! ani type:[%u], frame:[%u]",i,j);				
					g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
					/*sprintf(temp,"1)Previous:[%d] Current[%d]",m_pRenderedFromFrame_PREVIOUS[iAnimNum],m_pRenderedFromFrame_CURRENT[iAnimNum]);
					g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
				}
			}

			m_pRenderedFromFrame_PREVIOUS[iAnimNum] = m_pRenderedFromFrame_CURRENT[iAnimNum];
			
		}
		else if (m_pRenderedFromFrame_PREVIOUS[iAnimNum]!=m_pRenderedFromFrame_CURRENT[iAnimNum])
		{

			// TODO: wrap it up in some single nice modulo
			// regular case - 2 -----> 7
			if (m_pRenderedFromFrame_PREVIOUS[iAnimNum]<m_pRenderedFromFrame_CURRENT[iAnimNum])
			{
				for (int j=m_pRenderedFromFrame_PREVIOUS[iAnimNum]+1;j<m_pRenderedFromFrame_CURRENT[iAnimNum]+1;j++)
				{

					pFrameActions = m_ppAnimations[iAnimNum]->GetFramesActions(j);
					if (pFrameActions)
					{
						//m_ppCurrentFrameAnimationsTriggers[iFrameTriggeredActions++] = pFrameActions;

						m_pParentEntity->TriggerFrameActions(pFrameActions);

						/*sprintf(temp,"2)Triggered frame action! ani type:[%u], frame:[%u]",i,j);				
						g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

						/*sprintf(temp,"2)Previous:[%d] Current[%d]",m_pRenderedFromFrame_PREVIOUS[iAnimNum],m_pRenderedFromFrame_CURRENT[iAnimNum]);
						g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
					}
				}

				m_pRenderedFromFrame_PREVIOUS[iAnimNum] = m_pRenderedFromFrame_CURRENT[iAnimNum];
			}
			else if (m_ppAnimations[iAnimNum]->GetLoopable())// warp around required
			{
				int iCurrentFrameTrigger = (m_pRenderedFromFrame_PREVIOUS[iAnimNum]+1)%iFramesNum;

				while (iCurrentFrameTrigger!=(m_pRenderedFromFrame_CURRENT[iAnimNum]+1)%iFramesNum)
				{

					//BREAKPOINT(iCurrentFrameTrigger==7);

					pFrameActions = m_ppAnimations[iAnimNum]->GetFramesActions(iCurrentFrameTrigger);
					if (pFrameActions)
					{
						//m_ppCurrentFrameAnimationsTriggers[iFrameTriggeredActions++] = pFrameActions;
						m_pParentEntity->TriggerFrameActions(pFrameActions);
						
						/*sprintf(temp,"Triggered frame action! ani type:[%u], frame:[%u]",i,iCurrentFrameTrigger);				
						g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
					}


					iCurrentFrameTrigger++;
					iCurrentFrameTrigger%=iFramesNum;
				}
				
				m_pRenderedFromFrame_PREVIOUS[iAnimNum] = m_pRenderedFromFrame_CURRENT[iAnimNum];
			}
			
			
		}

		//m_pRenderedFromFrame_PREVIOUS[i] = m_pRenderedFromFrame_CURRENT[i];
		
	}


	


	//m_ppCurrentFrameAnimationsTriggers[iFrameTriggeredActions] = NULL;


	/*if (m_ppCurrentFrameAnimationsTriggers)
		if (m_ppCurrentFrameAnimationsTriggers[0])
			BREAKPOINT(m_ppCurrentFrameAnimationsTriggers[0]->iActionsNum < 0);

	return m_ppCurrentFrameAnimationsTriggers;*/
	

}

void CAnimController::ComputeFinalSkeleton(float fDeltaTime)
{

	UpdateWeightedAnims(fDeltaTime);
	const int* pChannelIndices = NULL;
	int iChannelIndicesNum = -1;

	bool bRequestFadeOut = false;

		
	std::map<int,int> m_ParticipatedAnims;


	

	

	for (int c=0;c<MD5_MESH_CHANNELS_NUM;c++)
	{				
		pChannelIndices = m_pMesh->GetChannelJointsIndices(c);
		iChannelIndicesNum = m_pMesh->GetChannelJointsIndicesNum(c);

		float fTotalWeight = 0.f;
		
		for (int s=0;s<ANI_TYPE_NUM;s++)
		{
			//TODO: perhaps find a better place for this
			m_PriorityOrderedChannels[c][s]->fRunningTime+= fDeltaTime;

			if (m_PriorityOrderedChannels[c][s]->fWeight==0.f)
				continue;
			
			if (fTotalWeight >=1.f)
				break;
			
			int iAnimNum = m_PriorityOrderedChannels[c][s]->iAnimationNum;

			BREAKPOINT(iAnimNum<0);
			

			if (fTotalWeight == 0.f)
			{
				

				bRequestFadeOut = InterpolateBetweenFrames(m_pCurrentBindJoints,
										pChannelIndices,
										iChannelIndicesNum,										
										//(g_pTimer->GetTimeSecondsFAKE() - m_PriorityOrderedChannels[c][s]->fStartTime) ,																														
										m_PriorityOrderedChannels[c][s]->fRunningTime,
										iAnimNum,
										true);



				fTotalWeight = m_PriorityOrderedChannels[c][s]->fWeight;

				/*g_pGraphicsAPI->AddDebugText(50,50,"AnimNum:%i - Weight:%f\n",iAnimNum,m_PriorityOrderedChannels[c][s]->fWeight);*/
			}
			else
			{
				bRequestFadeOut = InterpolateBetweenFrames(m_pTEMPBindJoints,
										pChannelIndices,
										iChannelIndicesNum,
										//(g_pTimer->GetTimeSecondsFAKE() - m_PriorityOrderedChannels[c][s]->fStartTime) ,										
										m_PriorityOrderedChannels[c][s]->fRunningTime,
										iAnimNum,
										true);

				float fThisWeight = m_PriorityOrderedChannels[c][s]->fWeight;

				fThisWeight=min(fThisWeight,1.0-fTotalWeight);

				fTotalWeight+=fThisWeight;

				float fCurrentWeight=fThisWeight/fTotalWeight;


				/*g_pGraphicsAPI->AddDebugText(50,50,"AnimNum:%i - Weight:%f\n",iAnimNum,fCurrentWeight);	*/
				BlendJointsIndices(m_pCurrentBindJoints,m_pTEMPBindJoints,fCurrentWeight,pChannelIndices,iChannelIndicesNum);
			}


			// for a test, just the actual frames that partcipiated,
			// i MUST also consider missing frames


			// trigger all of the events declared in the animation (in the character file)
			//FindFramesActions(iAnimNum);

			m_ParticipatedAnims[iAnimNum]=iAnimNum;
			
			
			if (bRequestFadeOut)
				m_PriorityOrderedChannels[c][s]->bRequestFadeOut = true;
					
		}

	

	}


	ActivateFK();

//	BREAKPOINT(ppRet && ppRet[0]);

	//std::map<int,int> m_ParticipatedAnims;
	for (std::map<int,int>::iterator it=m_ParticipatedAnims.begin();it!=m_ParticipatedAnims.end();++it)
	{
		FindFramesActions(it->first);
	}

}

void CAnimController::ActivateFK(void)
{
	CMatrix rotMat;
	const tMD5Joint* pJoint;

	for (int i=0;i<m_pMesh->GetJointsNum();i++)
	{

		//pJoint = &m_ppAnimations[iAnimNum]->GetBaseJoints()[i];	

		pJoint = &m_pMesh->GetJoints()[i];
		
		

		CVec3f interPos = m_pCurrentBindJoints[i].bindPos;
		CQuat  interRot = m_pCurrentBindJoints[i].bindRot;

		if (!pJoint->pParent)
		{
			m_pCurrentBindJoints[i].bindPos = interPos;
			m_pCurrentBindJoints[i].bindRot = interRot;
		}
		else
		{
			// convert quaternion of parent of current joint into the matrix rotMat
			m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindRot.GetMatrix(rotMat);	

			m_pCurrentBindJoints[i].bindPos = (rotMat*interPos) + m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindPos;
			m_pCurrentBindJoints[i].bindRot = interRot * m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindRot;										
		}
	}

	/*for (int i=0;i<m_pMesh->GetJointsNum();i++)
	{
		
		const CVec3& vTrans=pQuats[i].t;

		if (pBones[i].pParent)
		{
			pBones[i].mBindMat = mRot*pBones[i].pParent->mBindMat;
			pBones[i].vBindPos = pBones[i].pParent->vBindPos+pBones[i].pParent->mBindMat*vTrans;

			//if (g_cvShowBones)
			//{
			//	glVertex3fv((float*)&pBones[i].vBindPos);
			//	glVertex3fv((float*)&pBones[i].pParent->vBindPos);
			//}
		}
		else
		{
			pBones[i].mBindMat=mRot;
			pBones[i].vBindPos=CVec3(0,0,0);//vTrans;
		}

	}*/

}




void CAnimController::PlayAction(int iAnimNum)
{		
	for (int c=0;c<CHANNELS_NUM;c++)
	{
		/*if (c==0)
			continue;*/

		if (m_AnimOrderedChannels[c][iAnimNum].bMoveAni)
		{
			g_pLogFile->OutPutPlainText("CAnimController::PlayAction play action was used with a move type animation!","4","440088",true,LOG_MESSAGE_ERROR);
			return;
		}

		if (!m_AnimOrderedChannels[c][iAnimNum].bLooped)
		{
			if (m_AnimOrderedChannels[c][iAnimNum].fWeight > 0.f)
				continue;
		}

		/*char temp[500];
		sprintf(temp,"Play Action!");
		g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

		m_AnimOrderedChannels[c][iAnimNum].bRequestPlay = true;

		if (!m_AnimOrderedChannels[c][iAnimNum].bLooped)
			m_pRenderedFromFrame_PREVIOUS[iAnimNum] = -1;

		if (m_AnimOrderedChannels[c][iAnimNum].fWeight == 0.f)
			m_AnimOrderedChannels[c][iAnimNum].fRunningTime = 0.f;
		
		m_AnimOrderedChannels[c][iAnimNum].bRequestFadeOut = false;

		if (!m_AnimOrderedChannels[c][iAnimNum].bLooped)
			m_AnimOrderedChannels[c][iAnimNum].fWeight = 0.f;		
	}

}



bool CAnimController::InterpolateBetweenFrames(tMD5BindJoint* pJoints,const int* pIndices, int iNumIndices, float fTimeSinceStart, int iAnimNum,bool bLoopable)
{

	bool bRequestFadeOut = false;
	int iFrameFrom,iFrameTo;

	float fTimeSinceStartTimedFPS = fTimeSinceStart * ((float) m_ppAnimations[iAnimNum]->GetAnimationFPS());

	GetFrames(iFrameFrom,iFrameTo,fTimeSinceStartTimedFPS,iAnimNum);

	


	float fTotalTimeForAnim = m_ppAnimations[iAnimNum]->GetFramesNum() / ((float) m_ppAnimations[iAnimNum]->GetAnimationFPS());

	if (!m_ppAnimations[iAnimNum]->GetLoopable())
		if ( (fTotalTimeForAnim - fTimeSinceStart <= FADE_IN_OUT_TIME) ||
			fTimeSinceStart > fTotalTimeForAnim )

			bRequestFadeOut = true;

	

	/////////////////////////////////////////////////////////////////////
	// frame action triggers related
	//if (!bRequestFadeOut)
		m_pRenderedFromFrame_CURRENT[iAnimNum] = iFrameFrom;

	////////////////////////////////////////////////////////////////////




	/*if (iFrameTo == 0)
		bPassedAnimEnd = true;*/


	/*BREAKPOINT(iFrameFrom>=m_ppAnimations[iAnimNum]->GetFramesNum());
	BREAKPOINT(iFrameFrom<0);

	BREAKPOINT(iFrameTo>=m_ppAnimations[iAnimNum]->GetFramesNum());
	BREAKPOINT(iFrameTo<0);*/



	const tMD5AnimFrame* pFrameFrom = &m_ppAnimations[iAnimNum]->GetAnimFrames()[iFrameFrom];
	const tMD5AnimFrame* pFrameTo = &m_ppAnimations[iAnimNum]->GetAnimFrames()[iFrameTo];


	
	// getting only the fraction
	float fFrameT = fTimeSinceStartTimedFPS - floorf(fTimeSinceStartTimedFPS);


	float fMatrix[16];			
	int iElementCount=0;
	tMD5Joint* pJoint;
	
	CVec3f CurrentBindPos;
	CQuat  CurrentBindRot;


	
	int i=0;
	
	for (int ti=0;ti<iNumIndices;ti++)
	{
		i = pIndices[ti];

		pJoint = &m_ppAnimations[iAnimNum]->GetBaseJoints()[i];			

		/////////////////////////////////////////////////////////////////
		// compute from

		CVec3f newPosFrom=pJoint->pos;
		CQuat  newRotFrom=pJoint->rot;
	
		iElementCount=pJoint->iStartIndex;

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_X)
			newPosFrom.v[0] = pFrameFrom->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Y)
			newPosFrom.v[1] = pFrameFrom->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Z)
			newPosFrom.v[2] = pFrameFrom->pData[ iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_X)
			newRotFrom.x = pFrameFrom->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Y)
			newRotFrom.y = pFrameFrom->pData[ iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Z)
			newRotFrom.z = pFrameFrom->pData[ iElementCount++];

		newRotFrom.ComputeW();

		///////////////////////////////////////////////////////////////////
		// compute to

		CVec3f newPosTo=pJoint->pos;
		CQuat  newRotTo= pJoint->rot;

		iElementCount=pJoint->iStartIndex;

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_X)
			newPosTo.v[0] = pFrameTo->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Y)
			newPosTo.v[1] = pFrameTo->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Z)
			newPosTo.v[2] = pFrameTo->pData[ iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_X)
			newRotTo.x = pFrameTo->pData[iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Y)
			newRotTo.y = pFrameTo->pData[ iElementCount++];

		if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Z)
			newRotTo.z = pFrameTo->pData[ iElementCount++];

		newRotTo.ComputeW();

		// debug - trying to make walk animation stay in place 
		if (i==0)
		{
			newPosTo.Set(0.f,0.f,0.f);
			newPosFrom.Set(0.f,0.f,0.f);
		}

		// temp try to add aim up/down

		pJoints[i].bindPos = newPosFrom*(1.f-fFrameT) + newPosTo*fFrameT;
		pJoints[i].bindRot = newRotFrom.Slerp(newRotTo,fFrameT);

		if (strcmp(strlwr(pJoint->pchName),"waist")==0)
			{
				CQuat add(DEG2RAD(-m_pInstance->GetLookUpAngle()),0.f,0.f);
				
				//pJoints[i].bindRot = add * pJoints[i].bindRot;

				pJoints[i].bindRot = pJoints[i].bindRot*add ;
			}
	}



	return bRequestFadeOut;
}


void CAnimController::BlendJointsIndices(tMD5BindJoint* pJoints1, const tMD5BindJoint* pJoints2, float fT, const int* pIndices, int iNumIndices)
{
     CQuat q;
     CVec3f v;

     for (int ti=0;ti<iNumIndices;ti++)
     {
          int i=pIndices[ti];
		  pJoints1[i].bindRot = pJoints1[i].bindRot.Slerp(pJoints2[i].bindRot,fT);
		  pJoints1[i].bindPos = (pJoints1[i].bindPos*(1.f-fT)) + pJoints2[i].bindPos*fT;			          
     }
}

void CAnimController::RequestMoveState(int iMoveState)
{

	// randomly chosen channel - CHANNEL_LEGS
	if (!m_AnimOrderedChannels[CHANNEL_LEGS][iMoveState].bMoveAni)
		{
			g_pLogFile->OutPutPlainText("CAnimController::RequestMoveState requestd a NON move animation!","4","440088",true,LOG_MESSAGE_ERROR);
			return;
		}

	if ( (m_AnimOrderedChannels[0][iMoveState].fWeight == 0.f ) ||
		(m_AnimOrderedChannels[1][iMoveState].fWeight == 0.f ))
	{
		for (int c=0;c<CHANNELS_NUM;c++)
		{
			m_AnimOrderedChannels[c][iMoveState].fRunningTime = 0.f;
		}

	}

	m_iMoveState = iMoveState;
}


void CAnimController::SetPriorityOrderedChannels(void)
{
	// init priority ordered channels to anim ordered

	for (int c=0;c<CHANNELS_NUM;c++)
		for (int s=0;s<ANI_TYPE_NUM;s++)
		{	
			m_PriorityOrderedChannels[c][s] = & m_AnimOrderedChannels[c][s];
		}


	tWeightedAnim* pTemp = NULL;
	int iAnim1Prio,iAnim2Prio;

	for (int c=0;c<CHANNELS_NUM;c++)
	{
		// a seperate sort per channel

		// lame bubble sort for now *g

		for (int i=0;i<ANI_TYPE_NUM-1;i++)
			for (int j=i+1;j<ANI_TYPE_NUM;j++)
			{
				iAnim1Prio = m_ppAnimations[m_PriorityOrderedChannels[c][i]->iAnimationNum]->GetPriority(c);
				iAnim2Prio = m_ppAnimations[m_PriorityOrderedChannels[c][j]->iAnimationNum]->GetPriority(c);

				if (iAnim1Prio < iAnim2Prio)
				{
					pTemp = m_PriorityOrderedChannels[c][i];
					m_PriorityOrderedChannels[c][i] = m_PriorityOrderedChannels[c][j];
					m_PriorityOrderedChannels[c][j] = pTemp;
				}

			}


	}
	

}
