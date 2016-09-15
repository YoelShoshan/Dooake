#include "stdafx.h"
#include "Character.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "ControllableEntity.h"

#include "GMath.h"

#include "ceParser.h"
#include "ceLibrary.h"

#include "Q3AMap.h"

tAction::tAction()
{
	/*if (iButtonsNum==0)
	{
		m_piButtons = NULL;
		BREAKPOINT(1);
	}

	m_piButtons = new int [iButtonsNum];*/

	m_piButtons = NULL;
}

tAction::~tAction()
{
	SafeDeleteArray(m_piButtons);
}


CCharacter::CCharacter()
{
	m_MD5Inst = new CMD5Inst();
	SetPosition(CVec3f(0.f,0.f,0.f));	
	
	m_Angles.Set(0.f,0.f,0.f);

	m_vCurrentAngles.Set(0.f,0.f,0.f);
	m_vPreviousAngles.Set(0.f,0.f,0.f);

	m_pActions = NULL;
	m_iLastAction = 0;

	m_LookAt = CVec3f(0.f,-1.f,0.f);

	m_pcName = NULL;

	//m_pCurrentTriggeredActions = NULL;
	m_iRegularWeaponType = WEAPON_TYPE_NOT_INITED;
	m_fRegularWeaponSpeed = 0.f;
};

CCharacter::~CCharacter()
{
	SafeDelete(m_MD5Inst);
	SafeDeleteArray(m_pActions);
};

void CCharacter::ClearEffectingLights(void)
{
	m_MD5Inst->ClearLightsLinkage();


}

void CCharacter::AddEffectingLight(ILight_Export* pLight)
{
	m_MD5Inst->AddLightToAllRends(pLight);

}

void CCharacter::SetShared(CSmartPointer<CMD5_Shared> spSet,CEntity* pParentEntity)
{
	//assert(_CrtCheckMemory());
	m_MD5Inst->SetShared(spSet,pParentEntity);

	//assert(_CrtCheckMemory());

	m_pcName = new char[strlen(spSet->GetName())+1];
	strcpy(m_pcName,spSet->GetName());

	//assert(_CrtCheckMemory());

	if (strcmp(m_pcName,"MD5/mancubus/mancubus.chr")==0)
	{
		m_iRegularWeaponType = WEAPON_TYPE_ROCKET;
		m_fRegularWeaponSpeed = WEAPON_ROCKET_SPEED;

	}else if (strcmp(m_pcName,"MD5/vagary/vagary.chr")==0)
	{
		m_iRegularWeaponType = WEAPON_TYPE_BLUE;
		m_fRegularWeaponSpeed = WEAPON_FIREBALL_SPEED;

	}else if (strcmp(m_pcName,"MD5/archvile/archvile.chr")==0)
	{
		m_iRegularWeaponType = WEAPON_TYPE_FIREBALL;
		m_fRegularWeaponSpeed = WEAPON_FIREBALL_SPEED;

	}else if (strcmp(m_pcName,"MD5/hellknight/hellknight.chr")==0)
	{
		m_iRegularWeaponType = WEAPON_TYPE_WHITE;
		m_fRegularWeaponSpeed = WEAPON_WHITE_SPEED;

	}else
	{
		// ...
	}



	// debug debug
	//m_MD5Inst->SetSkeletonRender(true);
}

void  CCharacter::ComputeAnimationState(float fDeltaTime,bool bVisible)
{
	
	float fTimePerChar;

	char message[100];


	fTimePerChar = g_pTimer->GetTime();

	

	m_MD5Inst->UpdateAni(RENDERABLE_GENERATE_PPL, fDeltaTime,bVisible);

	
		
	// i need to only update when animation ended
	// not considering animation origin offset


	//m_vPosition+=vOriginOffset;

	//m_AnimationEffectedPos = m_vPosition+vOriginOffset;

	// hack?
	/*if (vOriginOffset == CVec3f(0.f,0.f,0.f))
		m_vPosition = m_AnimationEffectedPos;*/

	


	fTimePerChar = g_pTimer->GetTime() - fTimePerChar;
	
	sprintf(message,"%f",fTimePerChar);
	

	// not sure if i can do that, i might use the renderable matrices somewhere...
	//if (bVisible)
		UpdateRenderablesMatrices();

	
}

void CCharacter::UpdateRenderablesMatrices(void)
{
	float pfWorld[16];
	CMatrix trans,rot;

	// RESTORE RESTORE RESTORE
	trans.MatMakeMove(m_vPosition.v[0],m_vPosition.v[1],m_vPosition.v[2]);
	//trans.MatMakeMove(m_AnimationEffectedPos.v[0],m_AnimationEffectedPos.v[1],m_AnimationEffectedPos.v[2]);

	//rot.MatMakeRot(DEG2RAD(m_Angles.v[0]),DEG2RAD(m_Angles.v[1]),DEG2RAD(m_Angles.v[2]));

	rot.MatMakeRot(DEG2RAD(0.f),DEG2RAD(m_Angles.v[1]),DEG2RAD(0.f));

	rot = trans*rot;

	rot.ReturnArray(pfWorld);
	m_MD5Inst->SetTransformForAll(pfWorld);

}

void CCharacter::PlayAction(int iAnimNum)
{
	m_MD5Inst->PlayAction(iAnimNum);	
}

void CCharacter::Die(void)
{
	m_MD5Inst->Die();
}

/*bool CCharacter::OnlyBase(void)
{
	return m_MD5Inst->OnlyBase();
}*/

/*void CCharacter::TriggerAction(int iAction)
{
	//if (iAction==m_iLastAction)
	//	iAction=-1;

	//if (iAction==-1)
	//{
	//	PlayBaseAni(m_pActions[m_iLastAction].m_iEndLoopAni);
	//	return;
	//}

	//if (m_iLastAction!=iAction)
	//	PlayAnimation(m_pActions[iAction].m_iActionAni);
	//		
	//PlayBaseAni(m_pActions[iAction].m_iEndLoopAni);


	//m_iLastAction = iAction;

	PlayAction(m_pActions[iAction].m_iActionAni);
	
}*/

bool CCharacter::LoadALFile(const char* pchFileName)
{
	char message[300];
	sprintf(message,"CCharacter::LoadALFile loading %s character action list file...",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","440088",true,LOG_MESSAGE_INFORMATIVE);

	FILE* pFile;
	if ((pFile=fopen (pchFileName, "rb"))== NULL)
	{
		g_pLogFile->OutPutPlainText("CCharacter::LoadALFile opening of character action list file failed!",LOG_MESSAGE_ERROR);
		return false;
	}

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iFileLength+1];
	fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength] = NULL;

	char currentWord[500];

	// create a parser containing it
	CParser par(pText,iFileLength+1);

	int iCurrentActionNum = 0;

	while (!par.ReachedEnd())
	{
		par.NextWord_IC();	
		par.FillCurrentWord(currentWord);

		if (strcmp(currentWord,"ActionsNum")==0)
		{
			par.NextWord_IC();	
			par.FillCurrentWord(currentWord);
			int iActionsNum = atoi(currentWord);

			m_pActions = new tAction[iActionsNum];		
		}
		else
		if (strcmp(currentWord,"action")==0)
		{
			par.NextWord_IC();	
			par.FillCurrentWord(currentWord);
			int iButtonsNum = atoi(currentWord);

			m_pActions[iCurrentActionNum].m_piButtons = new int[iButtonsNum];

			for (int i=0;i<iButtonsNum;i++)
			{
				par.NextWord_IC();	
				par.FillCurrentWord(currentWord);
				int iCurrentButtonNum = atoi(currentWord);

				m_pActions[iCurrentActionNum].m_piButtons[i] = iCurrentButtonNum;
			}

			par.NextWord_IC();	
			par.FillCurrentWord(currentWord);
			m_pActions[iCurrentActionNum].m_iActionAni = atoi(currentWord);

			par.NextWord_IC();	
			par.FillCurrentWord(currentWord);
			m_pActions[iCurrentActionNum].m_iEndLoopAni = atoi(currentWord);


			iCurrentActionNum++;
		}

	
	}

	return true;

}

void CCharacter::SetPosition(const CVec3f& vPos)
{
	m_vPosition = vPos;

//	m_AnimationEffectedPos = m_vPosition;
}

void CCharacter::BlendAnglesState(float fAlpha) 
{
	m_Angles = (m_vCurrentAngles*fAlpha)+(m_vPreviousAngles*(1.f-fAlpha));

	if (!m_MD5Inst->GetSharedFlipUpDown())
		m_MD5Inst->SetLookUpAngle(m_Angles.x);
	else
		m_MD5Inst->SetLookUpAngle(-m_Angles.x);

	/*char msg[150];
	sprintf(msg,"blended angles state:%f,%f",m_Angles.x,m_Angles.y,m_Angles.z);
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/

}

void CCharacter::SetAngles(const CVec3f& vAngles)
{
	CVec3f vNewAngles = vAngles;

	if (vAngles.x > MAX_LOOK_UP_DOWN_ANG)
		vNewAngles.x = MAX_LOOK_UP_DOWN_ANG;

	if (vAngles.x < -MAX_LOOK_UP_DOWN_ANG)
		vNewAngles.x = -MAX_LOOK_UP_DOWN_ANG;
	
	m_MD5Inst->SetLookUpAngle(vNewAngles.x);

	m_vPreviousAngles = m_vCurrentAngles;
	m_vCurrentAngles = vNewAngles;	
	m_Angles = vNewAngles;

	m_LookAt = GetVectorFromAngles(m_Angles.x,m_Angles.y-90.f);
}

CVec3f CCharacter::GetLookAt(void) const
{
	return m_LookAt;
}

CVec3f CCharacter::GetLeft(void) const
{
	return RotateVectorByAngle(m_Angles.x,m_Angles.y-90.f,CVec3f(-1.f,0.f,0.f));

}

void CCharacter::Render(int iTech)
{
	//debug
	//return;
	m_MD5Inst->Render(iTech);
	//m_MD5Inst->Render(1);

	

	
}



void  CCharacter::Update(float fDeltaTime,bool bVisible)
{
	ComputeAnimationState(fDeltaTime,bVisible);


/*

	// in case of triggering - update the following:
	//
	// * PLAYER_ANIM_TRIGGERED_SPAWN_PARTICLE_SYSTEM
	if (ppFrameActions && ppFrameActions[0])
	{
				
		while (ppFrameActions[0])
		{
			for (int i=0;i<ppFrameActions[0]->iActionsNum;i++)
			{
				//sprintf(temp,"Triggered frame action! action:[%u], joint:[%u]",ppFrameActions[0]->pActions[i].uiAction,ppFrameActions[0]->pActions[i].iJointIndex);				
				//g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);



				switch(ppFrameActions[0]->pActions[i].uiAction)
				{
					case PLAYER_ANIM_TRIGGERED_SPAWN_PARTICLE_SYSTEM:					
					{
						
						//g_TextMessages.AddMessage("Triggered Particle System!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);						
						CVec3f vSpawnPS_Pos = GetCurrentFinalJointPosition(ppFrameActions[0]->pActions[i].iJointIndex);

						CParticleSystem* pPS = new CParticleSystem();
						pPS->SetBindIndex(ppFrameActions[0]->pActions[i].iJointIndex);
						pPS->SetOrigin(vSpawnPS_Pos);
						pPS->Init();
									

						

						m_ParticleSystems.push_front(pPS);

					}
					break;

					
				default:
					{
					//BREAKPOINT(1); // unrecognized message
					break;
					}
				}

			}

			ppFrameActions++;
		}


	}



	
*/

	
}

void CCharacter::RequestMoveState(int iMoveState)
{
	m_MD5Inst->RequestMoveState(iMoveState);
}

CVec3f CCharacter::GetCurrentFinalJointPosition(int iJointIndex)
{
	return m_MD5Inst->GetJointPosition(iJointIndex);	
}