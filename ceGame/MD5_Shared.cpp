#include "stdafx.h"
#include "MD5_Shared.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "ceParser.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "MD5Manager.h"

#include "ControllableEntity.h"

#include "AnimationController.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

CMD5_Shared::CMD5_Shared()
{
	m_pcCharacterName = NULL;
	m_iAnimationsNum = 0;

	m_ppAnimations = new CMD5Anim*[MD5_INSTANCE_ANIMATIONS_MAX];

	for (int i=0;i<MD5_INSTANCE_ANIMATIONS_MAX;i++)
		m_ppAnimations[i]=NULL;

	m_pMesh = NULL;
	m_ppIndexBuffers = NULL;
	m_bFlipLookUpDown = false;

	m_bAutoFire_Attack1 = false;
	m_bAutoFire_Attack2 = false;

	m_pcName = NULL;
}

CMD5_Shared::~CMD5_Shared()
{
	char message[300];
	sprintf(message,"deleting md5_shared %s...",m_pcCharacterName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
	sprintf(message,"deleting md5_shared %s DONE.",m_pcCharacterName);

	st_MD5Manager->Unregister(m_pcCharacterName);

	SafeDelete(m_pMesh);

	for (int i=0;i<MD5_INSTANCE_ANIMATIONS_MAX;i++)
		SafeDelete(m_ppAnimations[i]);
	SafeDeleteArray(m_ppAnimations);

	SafeDelete(m_pcCharacterName);

	if (m_ppIndexBuffers)
	{
		if (m_pMesh)
				for (int i=0;i<m_pMesh->GetSubMeshesNum();i++)
				SafeDelete(m_ppIndexBuffers[i]);
		SafeDeleteArray(m_ppIndexBuffers);
	}

	
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);

}

bool CMD5_Shared::GenerateIndexBuffers(void)
{
	for (int i=0;i<m_pMesh->GetSubMeshesNum();i++)
	{
		m_ppIndexBuffers[i] = g_pRendererBack->GenerateIndexBuffer();

		m_ppIndexBuffers[i]->Allocate(m_pMesh->GetSubMeshes()[i].iIndicesNum,IB_MEM_GPU);
		m_ppIndexBuffers[i]->Lock(CR_LOCK_WRITE_ONLY,0,0);

		UINT* pIndices = (UINT*) m_ppIndexBuffers[i]->GetData();

		memcpy(pIndices,m_pMesh->GetSubMeshes()[i].puiIndices,sizeof(unsigned int)*m_pMesh->GetSubMeshes()[i].iIndicesNum);
	
		m_ppIndexBuffers[i]->Unlock();

	}

	return true;
}

	


bool CMD5_Shared::AddAnim(CMD5Anim* pAnim)
{
	m_ppAnimations[m_iAnimationsNum] = pAnim;
	

	m_iAnimationsNum++;

	return true;
}

void CMD5_Shared::SetMesh(CMD5Mesh* pSet)
{
	if (m_pMesh)
		g_pLogFile->OutPutPlainText("MD5Inst::SetMesh already contained a mesh!",LOG_MESSAGE_ERROR);


	if (!pSet)
		g_pLogFile->OutPutPlainText("MD5Inst::SetMesh trying to set NULL CMD5Mesh!",LOG_MESSAGE_ERROR);

	m_pMesh = pSet;

	m_ppIndexBuffers = new IIndexBuffer*[m_pMesh->GetSubMeshesNum()];

	GenerateIndexBuffers();



	// debug debug debug
	//m_pMesh->AddJointsToChannel("waist",MD5_MESH_CHANNEL_TORSO,true);

}

bool CMD5_Shared::LoadCharacterFile(const char* pchFileName)
{
	char message[300];
	sprintf(message,"CMD5Inst::LoadCharacterFile loading %s character file...",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","440088",true,LOG_MESSAGE_INFORMATIVE);

	FILE* pFile;
	if ((pFile=fopen (pchFileName, "rb"))== NULL)
	{
		g_pLogFile->OutPutPlainText("CMD5Inst::LoadCharacterFile opening of Character file failed!",LOG_MESSAGE_ERROR);
		return false;
	}


	m_pcName = new char[strlen(pchFileName)+1];
	strcpy(m_pcName,pchFileName);

	m_pcCharacterName = new char[strlen(pchFileName)+1];
	strcpy(m_pcCharacterName,pchFileName);

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iFileLength+1];
	fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength] = NULL;

	char currentWord[300];
	char currentWord2[300];
	char currentLine[600];

	// create a parser containing it
	CParser par(pText,iFileLength+1);

	//par.EatWhiteSpace();

	
	bool bLineEnded = false;
	
	while (!par.ReachedEnd())
	{

		assert(_CrtCheckMemory());

		if (bLineEnded)
		{
			bLineEnded=false;
			par.NextLine();
//			par.NextWord_IC();

			assert(_CrtCheckMemory());

			par.FillCurrentWord(currentWord);

			assert(_CrtCheckMemory());
		}
		else
		{
			//assert(_CrtCheckMemory());
			par.NextWord_IC();	
			//assert(_CrtCheckMemory());
			par.FillCurrentWord(currentWord);
			//assert(_CrtCheckMemory());
		}

		if (strcmp(currentWord,"mesh")==0)
		{
			par.NextWord_IC();	
			par.FillCurrentQuetuePhrase(currentWord);


			/*CVec3f vMeshScale;

			par.NextWord_IC();	
			par.FillCurrentWord(currentWord2);
			vMeshScale.x = atof(currentWord2);
			

			par.NextWord_IC();	
			par.FillCurrentWord(currentWord2);
			vMeshScale.y = atof(currentWord2);
			

			par.NextWord_IC();	
			par.FillCurrentWord(currentWord2);
			vMeshScale.z = atof(currentWord2);
			
			if (vMeshScale.Length()==0.f)
				vMeshScale.Set(1.f,1.f,1.f);*/
			

			CMD5Mesh* pMesh = new CMD5Mesh();
			pMesh->Load(currentWord/*,vMeshScale*/);
			SetMesh(pMesh);
		}

		//assert(_CrtCheckMemory());

		if (strcmp(currentWord,"flipupdown")==0)
		{
			m_bFlipLookUpDown = true;
			continue;
		}

		if (strcmp(currentWord,"autofire_attack1")==0)
		{
			m_bAutoFire_Attack1 = true;			
			continue;
		}

		if (strcmp(currentWord,"autofire_attack2")==0)
		{
			m_bAutoFire_Attack2 = true;						
			continue;
		}


		//assert(_CrtCheckMemory());

		if (strcmp(currentWord,"anim")==0)
		{
			//assert(_CrtCheckMemory());

			par.NextWord_IC();	
			par.FillCurrentQuetuePhrase(currentWord);

			CMD5Anim* pAnim = new CMD5Anim();
			pAnim->Load(currentWord);	
			AddAnim(pAnim);

			//par.NextWord_IC();
			par.FillCurrentLine(currentLine);

			CParser linePar(currentLine, strlen(currentLine)+1);
				

			tFrameActions frameActions;

			//tFrameAction tempActions[FRAME_ACTIONS_MAX];
			//int iFrameActionsNum=0;

			//assert(_CrtCheckMemory());

			while (!linePar.ReachedEnd())
				{
					//assert(_CrtCheckMemory());

					linePar.NextWord();
					linePar.FillCurrentWord(currentWord);

					if (strlen(currentWord)==0)
					{
						// line ended
						bLineEnded=true;
						continue;
					}

					if (strcmp(currentWord,"reverse")==0)
					{
						pAnim->ReverseFramesOrder();		
					}

					//assert(_CrtCheckMemory());

					// base animations

					if (strcmp(currentWord,"idle")==0)
					{
						pAnim->SetAniType(ANI_TYPE_IDLE);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,1);
						pAnim->SetPriority(CHANNEL_TORSO,10);

						//assert(_CrtCheckMemory());

					}   else if (strcmp(currentWord,"walk")==0)
					{
						pAnim->SetAniType(ANI_TYPE_WALK);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,4);
						pAnim->SetPriority(CHANNEL_TORSO,0);

						//assert(_CrtCheckMemory());

					}	  else if (strcmp(currentWord,"walkback")==0)
					{
						pAnim->SetAniType(ANI_TYPE_WALK_BACK);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,5);
						pAnim->SetPriority(CHANNEL_TORSO,1);

						//assert(_CrtCheckMemory());

					} else if (strcmp(currentWord,"strafeleft")==0)
					{
						pAnim->SetAniType(ANI_TYPE_STRAFE_LEFT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,6);
						pAnim->SetPriority(CHANNEL_TORSO,2);

						//assert(_CrtCheckMemory());

					}  else if (strcmp(currentWord,"straferight")==0)
					{
						pAnim->SetAniType(ANI_TYPE_STRAFE_RIGHT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,7);
						pAnim->SetPriority(CHANNEL_TORSO,3);

						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"straferunleft")==0)
					{
						pAnim->SetAniType(ANI_TYPE_RUN_STRAFE_LEFT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,8);
						pAnim->SetPriority(CHANNEL_TORSO,4);

						//assert(_CrtCheckMemory());

					}  else if (strcmp(currentWord,"straferunright")==0)
					{
						pAnim->SetAniType(ANI_TYPE_RUN_STRAFE_RIGHT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,9);
						pAnim->SetPriority(CHANNEL_TORSO,5);

						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"rotateleft")==0)
					{
						pAnim->SetAniType(ANI_TYPE_ROTATE_LEFT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,10);
						pAnim->SetPriority(CHANNEL_TORSO,6);

						//assert(_CrtCheckMemory());

					}  else if (strcmp(currentWord,"rotateright")==0)
					{
						pAnim->SetAniType(ANI_TYPE_ROTATE_RIGHT);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,11);
						pAnim->SetPriority(CHANNEL_TORSO,7);

						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"run")==0)
					{
						pAnim->SetAniType(ANI_TYPE_RUN);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,12);
						pAnim->SetPriority(CHANNEL_TORSO,8);

						//assert(_CrtCheckMemory());

					}	else if (strcmp(currentWord,"runback")==0)
					{
						pAnim->SetAniType(ANI_TYPE_RUN_BACK);

						pAnim->SetLoopable(true);

						pAnim->SetPriority(CHANNEL_LEGS,13);
						pAnim->SetPriority(CHANNEL_TORSO,9);

						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"jump")==0)
					{
						pAnim->SetAniType(ANI_TYPE_JUMP);

						pAnim->SetPriority(CHANNEL_LEGS,14);
						pAnim->SetPriority(CHANNEL_TORSO,12);

						//assert(_CrtCheckMemory());

					} else  if (strcmp(currentWord,"midair")==0)
					{
						pAnim->SetAniType(ANI_TYPE_MID_AIR);

						pAnim->SetPriority(CHANNEL_LEGS,15);
						pAnim->SetPriority(CHANNEL_TORSO,13);

						//assert(_CrtCheckMemory());

					} else if (strcmp(currentWord,"land")==0)
					{
						pAnim->SetAniType(ANI_TYPE_LAND);

						pAnim->SetPriority(CHANNEL_LEGS,16);
						pAnim->SetPriority(CHANNEL_TORSO,14);
						//assert(_CrtCheckMemory());

					} 
					// non base animations
					else if (strcmp(currentWord,"attack1")==0)
					{
						pAnim->SetAniType(ANI_TYPE_ATTACK_1);

						pAnim->SetPriority(CHANNEL_LEGS,2);
						pAnim->SetPriority(CHANNEL_TORSO,15);
						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"attack2")==0)
					{
						pAnim->SetAniType(ANI_TYPE_ATTACK_2);

						pAnim->SetPriority(CHANNEL_LEGS,3);
						pAnim->SetPriority(CHANNEL_TORSO,16);
						//assert(_CrtCheckMemory());

					}else if (strcmp(currentWord,"pain")==0)
					{
						pAnim->SetAniType(ANI_TYPE_PAIN);

						pAnim->SetPriority(CHANNEL_LEGS,0);
						pAnim->SetPriority(CHANNEL_TORSO,11);
						//assert(_CrtCheckMemory());

					}/*else if (strcmp(currentWord,"taunt")==0)
					{
						pAnim->SetAniType(ANI_TYPE_TAUNT);

					}*/else if (strcmp(currentWord,"loopable")==0)
					{
						//assert(_CrtCheckMemory());
						pAnim->SetLoopable(true);						
					} else if (strcmp(currentWord,"trigger")==0)
					{

						//assert(_CrtCheckMemory());

						// FOR NOW, only allow single action inside a frame
						// (only need to extend PARSING to allow more, the rest supports this already!)

						tFrameAction frameAction;

						// get frame num

						linePar.NextWord();
						linePar.FillCurrentWord(currentWord);

						int iFrameNum = atoi(currentWord);


						//assert(_CrtCheckMemory());

						// get action type

						linePar.NextWord();
						linePar.FillCurrentWord(currentWord);			


						//assert(_CrtCheckMemory());

						if (strcmp(currentWord,"spawn_projectile_attack_1")==0)
						{
							//tempActions[iFrameActionsNum].uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_1;
							frameAction.uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_1;

						} else if (strcmp(currentWord,"spawn_projectile_attack_2")==0)
						{
							//tempActions[iFrameActionsNum].uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_2;
							frameAction.uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_2;

						} else if (strcmp(currentWord,"spawn_ps")==0)
						{
							//tempActions[iFrameActionsNum].uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PROJECTILE_2;
							frameAction.uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PARTICLE_SYSTEM;

						} else if (strcmp(currentWord,"sound")==0)
						{
							frameAction.uiAction = PLAYER_ANIM_TRIGGERED_ACTION_SOUND;
						} else
						{
							char message[400];
							sprintf(message,"CMD5_Shared::LoadCharacterFile trigger action unkown! [%s]!",currentWord);
							g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);

						}

						// get relevant joint
						if (frameAction.uiAction!=PLAYER_ANIM_TRIGGERED_ACTION_SOUND)
						{
							linePar.NextWord();
							linePar.FillCurrentWord(currentWord);		

							//tempActions[iFrameActionsNum].iJointIndex = pAnim->GetJointsIndex(currentWord);
							frameAction.iJointIndex = pAnim->GetJointsIndex(currentWord);
						}
						else
							frameAction.iJointIndex = 0;
												
						//assert(_CrtCheckMemory());

						char* pData = NULL;

						if ( (frameAction.uiAction == PLAYER_ANIM_TRIGGERED_ACTION_SPAWN_PARTICLE_SYSTEM) ||
							 (frameAction.uiAction == PLAYER_ANIM_TRIGGERED_ACTION_SOUND))
						{
							// we need to also read the particle system /sound name
							linePar.NextWord();
							//linePar.FillCurrentWord(currentWord);
							linePar.FillCurrentQuetuePhrase(currentWord);
							
							pData = (char*) frameAction.pData;

							pData = new char[strlen(currentWord)+1];
							strcpy(pData,currentWord);


							/*char temp[400];
							sprintf(temp,"particle system file: %s",pData);				
							g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

						}

						//assert(_CrtCheckMemory());

						//iFrameActionsNum++;

						// now ASSUMING only single action for frame
						// (BAD assumption ;) )

						frameAction.pData = pData;
																		
						frameActions.iActionsNum = 1;
						frameActions.pActions = new tFrameAction[1];

						memcpy(frameActions.pActions,&frameAction,sizeof(tFrameAction)*1);
				
						//BREAKPOINT(iFrameNum==41);
						pAnim->SetFrameActions(iFrameNum,frameActions);

						//assert(_CrtCheckMemory());
					}

					//assert(_CrtCheckMemory());

				} // end of going over anim parameters

				
			
				//assert(_CrtCheckMemory());

				



		}

		//assert(_CrtCheckMemory());

		if (strcmp(currentWord,"channel")==0)
		{

			//assert(_CrtCheckMemory());

			if (!m_pMesh)
			{
				char message[400];
				sprintf(message,"CMD5_Shared::LoadCharacterFile mesh must be define BEFORE channel!",currentWord);
				g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);

				par.NextLine();
			}

			par.NextWord_IC();
			par.FillCurrentLine(currentLine);

			CParser linePar(currentLine, strlen(currentLine)+1);

			linePar.FillCurrentWord(currentWord);


			int iChannelID = -1;

			if (strcmp(currentWord,"legs")==0)
			{
				iChannelID = MD5_MESH_CHANNEL_LEGS;			
			}
			else if (strcmp(currentWord,"torso")==0)
			{
				iChannelID = MD5_MESH_CHANNEL_TORSO;							
			}
			else
			{
				char message[400];
				sprintf(message,"CMD5_Shared::LoadCharacterFile unkown channel: %s, channel ignored!",currentWord);
				g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);
			}
						
			while (!linePar.ReachedEnd())
				{
					linePar.NextWord();
					linePar.FillCurrentWord(currentWord);

					int iActualJointNameStart = 0;

					if (strlen(currentWord)==0)
					{				
						bLineEnded=true;
						continue;
					}

					bool bRemove = false;

					if (currentWord[0]=='-')
					{
						bRemove = true;
						iActualJointNameStart++;
					}

					bool bRecurseDescendents = false;

					if (currentWord[iActualJointNameStart]=='*') // also recurse descendents
					{
						iActualJointNameStart++;
						bRecurseDescendents = true;

					}


					if (!bRemove) // add
						m_pMesh->AddJointsToChannel(&currentWord[iActualJointNameStart],iChannelID,bRecurseDescendents);
					else // remove
						m_pMesh->RemoveJointsFromChannel(&currentWord[iActualJointNameStart],iChannelID,bRecurseDescendents);

				}

		}
	}
	

	//assert(_CrtCheckMemory());

	
	m_pMesh->ConvertTempChannelsSetsIntoIndexArrays();


	//assert(_CrtCheckMemory());


	sprintf(message,"////CMD5Inst::LoadCharacterFile loading of %s character file Finished.",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","440088",true,LOG_MESSAGE_INFORMATIVE);

	return true;
}