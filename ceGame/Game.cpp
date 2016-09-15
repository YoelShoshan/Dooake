#include "stdafx.h"
#include "Game.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "CollectableEntity.h"
#include "ControllableEntity.h"
#include "TeleporterEntity.h"
#include "ProjectileEntity.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "Server_Local.h"
extern IServer*    g_pServer;

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;

#include "EngineOptions.h"
extern IEngineOptions* g_pEngineOptions;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "Server.h"
extern IServer*    g_pServer;

#include "Buttons.h"
extern CButtons g_Buttons;

int g_iLightSourceEntityUID;

extern bool g_bDeadReckoning;



extern CCamera g_Camera_FirstPerson;
extern CCamera g_Camera_FlashLight;
extern CCamera g_Camera_FreeFly;
#include "Frustum.h"
extern CFrustum g_Frustum;

#include "Input.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;


#include "NetPlayerEntity.h"
#include "BotEntity.h"
#include "HumanPlayerEntity.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;
#include "Profiler.h"

/*#include "GameApplication.h"
extern CGameApplication* g_pGameApplication;*/
#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;

#include "Light_IE.h"

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

extern int g_idbgEntitiesDrawnNum;
extern int g_idbgEntitiesOnlyNecessaryRMUpdate;

//int g_iHumanPlayerNum;
unsigned int g_uiControlledHumanPlayer;


extern bool g_bPostProcessing;

extern bool g_bRenderableRequirePostProcessing;

#define NO_LEADER 0xFFFF
#define NO_WINNER 0xFFFF

CGame::CGame()
{
	m_pBox = NULL;
	m_pBox2 = NULL;

	m_pHumanPlayerEntity = NULL;
	m_pBackgroundMusic = NULL;
	
	//m_iLeaderScore = 0;
	m_uiLeaderID = NO_LEADER;

	m_uiCameraMode = CAMERA_MODE_FIRST_PERSON;


	m_pActiveCamera		= &g_Camera_FirstPerson;
	m_pPreviousCamera	= &g_Camera_FirstPerson;
}

CGame::~CGame()
{
	m_spCrossHair = NULL;
	SafeDelete(m_pBox);
	
	SafeDelete(g_pQ3Map);

	if (m_pBackgroundMusic)
	{
		g_SoundAPI.StopStream(m_pBackgroundMusic);
		g_SoundAPI.FreeStream(m_pBackgroundMusic);
		SafeDelete(m_pBackgroundMusic);
	}
	
}

/*void  CGame::RenderCrossHair(void)
{
		
	//g_pRendererBack->Render2DQuad_Perc(m_spCrossHair,42.5,42.5,15,15);
}*/

void  CGame::AddPlayer(tPlayerGameState* pPlayerState)
{
	pPlayerState->uiEntityID = g_pQ3Map->AddPlayer(pPlayerState);

	if (g_pServer->m_dwGameType == SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT)
	{
		tUpdateClientGivenEntityID msg;
		msg.cID = pPlayerState->m_ConnectID;
		msg.dwEntity = pPlayerState->uiEntityID;

		g_pServer->OnEvent(TCP_MSG_UPDATE_CLIENT_ENTITY_ID,&msg);

		m_uiClientsAdded++;
	}
}

void CGame::AddPlayers(std::vector<tPlayerGameState*>* pPlayersGameState)
{
	BREAKPOINT(!pPlayersGameState);
	for (int i=0;i<m_pPlayersGameState->size();i++)
	{
		BREAKPOINT(!(*m_pPlayersGameState)[i]);
		if (!(*m_pPlayersGameState)[i]->m_bEntityCreated)
		{
			AddPlayer((*m_pPlayersGameState)[i]);		
			(*m_pPlayersGameState)[i]->m_bEntityCreated = true;
		}		
	}
}

bool CGame::Load(std::vector<tPlayerGameState*>* pPlayersGameState)
{
	m_pPlayersGameState = pPlayersGameState;
	g_iLightSourceEntityUID = 34;

	g_pQ3Map = new CQuake3BSP();
	

	g_pQ3Map->LoadBSP(g_pServer->GetMapName());

	AddPlayers(pPlayersGameState);		
	

	if (g_pServer->m_dwGameType == SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT)
	{
		// send "game started" message to everyone

		tUpdateClientStartGame msg;
		g_pServer->OnEvent(TCP_MSG_START_GAME,&msg);
	}

	// load the crosshair: ///////////////////////////////////////////////////////////////////////////////////////////


	/*m_pCrossHair = new CStaticModel();
	m_pCrossHair->SetShared(st_StaticModelManager->Load("%2",0.5f,1.0f,0.5f),RENDERABLE_GENERATE_NONE);		
	m_pCrossHair->SetMaterialForAll(g_pRendererBack->LoadMaterial("!GUI/Cross.tga"));*/

	//BREAKPOINT(m_spCrossHair);

	//m_spCrossHair = g_pRendererBack->LoadMaterial("!GUI/Cross.tga");
	//m_spCrossHair = g_pRendererBack->LoadMaterial("!GUI/Cross.tga");
	
		
	//m_pCrossHair->SetTransformForAll(NULL);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	// remove this

	//g_uiControlledHumanPlayer = (*pPlayersGameState)[0]->uiEntityID;

	//g_pQ3Map->LoadBSP("Q3Maps/q3dm1.bsp");
	//g_pQ3Map->LoadBSP("Q3Maps/q3dm1.bsp");
	//g_pQ3Map->LoadBSP("Q3Maps/gallery2_qc2k1.bsp");
	

	ILight_Export** ppLights = g_pQ3Map->GetLights();
	int iLightsNum           = g_pQ3Map->GetLightsNum();

	//g_pRendererFront->SetQ3MapBatchingIBSize(g_pQ3Map->GetMaxTrisSharingMaterialNum(),g_pQ3Map->GetRenderable(0));
	////////////////////////////////////////////////////////////////////////////////////////
	// load static model

	m_DebugMaterialGreen = g_pRendererBack->LoadMaterial("Materials/green.utm");

if (g_pEngineOptions->GetRendRTTBox())
{

	m_pBox = new CStaticModel();
	
	
	//m_pBox->SetShared(st_StaticModelManager->Load("Models/shipping_crates.3ds",1.f),RENDERABLE_GENERATE_PPL);		
	//m_pBox->SetShared(st_StaticModelManager->Load("Models/shipping_crates.3ds",1.f),RENDERABLE_GENERATE_PPL);		

	//m_pBox->SetShared(st_StaticModelManager->Load("Models/billboard2.3ds",0.5f,1.0f,0.5f),RENDERABLE_GENERATE_PPL);		

	m_pBox->SetShared(st_StaticModelManager->Load("$5",100.f,100.f,0.f),RENDERABLE_GENERATE_PPL);		

	
		
	// todo: why this doesn't load ????
	//m_pBox->SetShared(st_StaticModelManager->Load("Models/ferarri.3ds",1.f),RENDERABLE_GENERATE_PPL);		
	
	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/test1.mat"));
	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/alphaBloomTest1.mat"));


	// RESTORE
	m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/box-rtt-effect-flagged.mat"));

	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/Characters/hellknight.utm"));

	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/PP/Warp_1.mat"));

	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/shipping_crates_pm.utm"));
	

	
	/*for (int i=0;i<iLightsNum;i++)
		m_pBox->AddLightToAllRends(ppLights[i]);*/

	//m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/TestRenderDepth.mat"));
	
	//TODO: render new scene into a box showing the renderToTexture features
	

	
	float pfWorld[16];
	CMatrix world;
	CMatrix trans;		
	trans.MatMakeMove(495.50f,70.f,113.80f);	
	trans.ReturnArray(pfWorld);
	
	m_pBox->SetTransformForAll(pfWorld);
}




/*	m_pBox2 = new CStaticModel();
	m_pBox2->SetShared(st_StaticModelManager->Load("Models/shipping_crates.3ds",1.f,1.f,1.f),RENDERABLE_GENERATE_PPL);		
	m_pBox2->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/bricks-PM.utm"));
	
	for (int i=0;i<iLightsNum;i++)
		m_pBox2->AddEffectingLight(ppLights[i]);
	
	
	float pfWorld[16];
	CMatrix world;
	CMatrix trans;	
	//trans.MatMakeMove(270.82f,242.28f,-1459.07f);
	trans.MatMakeMove(495.50f,0.f,113.80f);	
	trans.ReturnArray(pfWorld);

	m_pBox2->SetTransformForAll(pfWorld);
*/
	////////////////////////////////////////////////////////////////////////////////////////////

	
	


	//g_pRendererBack->AddPostProcessEffectMaterial("Materials/RenderDepthFromDepthCamera.mat");

	
	//if (g_pEngineOptions->GetDepthOfField())
	if (g_bPostProcessing)
	{	
		g_pRendererBack->CleanPostProcessChain();
		g_pRendererBack->AddPostProcessEffectMaterial("Materials/PP/Bloom.mat");
	}

	
	//g_SoundAPI.LoadStream("Music/fight.mp3",m_pBackgroundMusic);
	g_SoundAPI.LoadStream("Music/fight.wav",m_pBackgroundMusic);
	//g_SoundAPI.LoadStream("Music/fight.ogg",m_pBackgroundMusic);
	//g_SoundAPI.LoadStream("Music/fight.ogg",m_pBackgroundMusic);
	g_SoundAPI.PlayStream(m_pBackgroundMusic,170);

	g_TextMessages.AddMessage("Game Started.",1.f,1.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);


	RestartMatch();
	
	return true;
}

void CGame::HandleCameraSwitch()
{
	if (CAMERA_MODE_FIRST_PERSON == m_uiCameraMode)
	{
		m_uiCameraMode = CAMERA_MODE_FREE_FLY;

		m_pPreviousCamera = m_pActiveCamera;
		m_pActiveCamera = &g_Camera_FreeFly;

		g_pQ3Map->SetRenderHumanPlayer(true);

	} else if (CAMERA_MODE_FREE_FLY == m_uiCameraMode)
	{
		m_uiCameraMode = CAMERA_MODE_FIRST_PERSON;

		m_pPreviousCamera = m_pActiveCamera;
		m_pActiveCamera = &g_Camera_FirstPerson;

		g_pQ3Map->SetRenderHumanPlayer(false);
	} else
		BREAKPOINT(1); // unknown camera mode

	
	// copy previous camera state into current, in order to start in the same location

	m_pActiveCamera->SetPosition(*m_pPreviousCamera->GetPosition());	
	m_pActiveCamera->SetAngles(m_pPreviousCamera->GetXAng(),m_pPreviousCamera->GetYAng(),m_pPreviousCamera->GetZAng());	
}
	
void CGame::ShowScore()
{
	char str[512];
	str[0] = NULL;
	char str_curr[255];

	for (int i=0;i<m_pPlayersGameState->size();i++)
	{		
		sprintf_s(str_curr,255,"%s:%d  ",
			(*m_pPlayersGameState)[i]->pName,
			(*m_pPlayersGameState)[i]->iScore);
		strcat_s(str,512,str_curr);		
	}
	
	g_TextMessages.SetPlayersScore(str);
}

void CGame::UpdateLogics(void)
{


	g_Camera_FirstPerson.InitMoveRotFlags();
	g_Camera_FlashLight.InitMoveRotFlags();
	g_Camera_FreeFly.InitMoveRotFlags();


	static bool bEscPressed=false;
	static bool bPressedJumpButton=false;
	static bool bPressed_5 = false;
	static bool bPressed_6 = false;





	if (!g_pMainMenu->GetChatActive())
	{
		if (st_Input->KeyPressed(DIK_ESCAPE))
		{
			if (!bEscPressed)
			{
				bEscPressed=true;


				if (g_pMainMenu->GetActive())
				{
					if (!g_pMainMenu->GetIsDeepInMenus())
						g_pMainMenu->SetActive(false);
				}
				else
					g_pMainMenu->SetActive(true);

			}			
		}
		else
			bEscPressed=false;
	}
	

	DWORD dwPlayerActionFlags = 0;


	if (CAMERA_MODE_FIRST_PERSON == m_uiCameraMode)
	{
			/*********************************************************************************************/
		// handle human player action requests
		
		if (!g_pMainMenu->GetActive() && !g_pMainMenu->GetChatActive())
		{

		/*	if (st_Input->KeyPressed(DIK_LEFT))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ROTATE_LEFT;

			if (st_Input->KeyPressed(DIK_RIGHT))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ROTATE_RIGHT;*/

			if (st_Input->KeyPressed(DIK_A))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_STRAFE_LEFT;		

			if (st_Input->KeyPressed(DIK_D))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_STRAFE_RIGHT;

			if (st_Input->KeyPressed(DIK_W) || st_Input->KeyPressed(DIK_UP))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_RUN_FORWARD;
					
			if (st_Input->KeyPressed(DIK_S) || st_Input->KeyPressed(DIK_DOWN))
				dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_RUN_BACKWARDS;


		
			if(st_Input->KeyPressed(DIK_SPACE) || st_Input->MouseButtonDown(DIMOUSE_RIGHTBUTTON))
			{
				// restore restore
				if (!bPressedJumpButton)
				{
					bPressedJumpButton = true;
					//pControlEntity->Jump();
					dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_JUMP;
				}
			}
			else
				bPressedJumpButton = false;
		

		
		
		
			if(st_Input->KeyPressed(DIK_5) || st_Input->MouseButtonDown(DIMOUSE_LEFTBUTTON))
			{
				if (!bPressed_5 ||                  
					(m_pHumanPlayerEntity && 
					( (CCharacter*)m_pHumanPlayerEntity->GetRenderModel())->GetMD5Inst()->GetShareIsLoopable_Attack1()
					)
					)
				{
					bPressed_5 = true;
					dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ATTACK_1;
				}
			}
			else
				bPressed_5 = false;

			
			if(st_Input->KeyPressed(DIK_6))
			{
				if (!bPressed_6)
				{
					bPressed_6 = true;
					dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ATTACK_2;
				}
			}
			else
				bPressed_6 = false;
		}


	} else if (CAMERA_MODE_FREE_FLY == m_uiCameraMode)
	{
		if (st_Input->KeyPressed(DIK_W))
			m_pActiveCamera->AddMoveRotateFlags(CAMERA_ACTION_MOVE_Z_FORWARD);

		if (st_Input->KeyPressed(DIK_S))
			m_pActiveCamera->AddMoveRotateFlags(CAMERA_ACTION_MOVE_Z_BACKWARDS);

		if (st_Input->KeyPressed(DIK_A))
			m_pActiveCamera->AddMoveRotateFlags(CAMERA_ACTION_MOVE_X_LEFT);

		if (st_Input->KeyPressed(DIK_D))
			m_pActiveCamera->AddMoveRotateFlags(CAMERA_ACTION_MOVE_X_RIGHT);

	} else
		BREAKPOINT(1); // unkown camera mode

	




	

/*********************************************************************************************************************/

	float fDeltaTime = g_pTimer->GetDeltaTimeFAKE();

///////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////
	// handle mouse
	

	// TODO: CHANGE this, idle shouldn't be ALWAYS set, only when no other input is given
	/*if (m_pHumanPlayerEntity)
	m_pHumanPlayerEntity->GetRenderModel()->RequestMoveState(ANI_TYPE_IDLE);*/
	

	float fTempMouseX,fTempMouseY,fMax=43.f;

	// no smoothing for now
	fTempMouseX = st_Input->GetRelativeX(false);
	fTempMouseY = st_Input->GetRelativeY(false);


	if (fTempMouseX>fMax)
		fTempMouseX=fMax;
	if (fTempMouseX<-fMax)
		fTempMouseX=-fMax;

	if (fTempMouseY>fMax)
		fTempMouseY=fMax;
	if (fTempMouseY<-fMax)
		fTempMouseY=-fMax;


	float fSensetivity = 5.f;

	//TODO: add animation for rotating at place

	// restore for tank character mode
	//g_Camera.AddAngles(-fTempMouseY/fSensetivity,-fTempMouseX/fSensetivity,0.f);

	if (fTempMouseY>0)
		dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ROTATE_RIGHT;
	else if (fTempMouseY<0)
		dwPlayerActionFlags|= PLAYER_ACTION_REQUEST_FLAG_ROTATE_LEFT;

	
	if (CAMERA_MODE_FIRST_PERSON == m_uiCameraMode)
	{
		if (m_pHumanPlayerEntity)
		{
			// restore for camera->character
			if (!g_pMainMenu->GetActive())			
				m_pHumanPlayerEntity->AddAngles(CVec3f(-fTempMouseY/fSensetivity,-fTempMouseX/fSensetivity,0.f));

			m_pActiveCamera->SetAngles(m_pHumanPlayerEntity->GetAngles().x,m_pHumanPlayerEntity->GetAngles().y-90.f,m_pHumanPlayerEntity->GetAngles().z);
		}
		else // first frame
		{
			m_pActiveCamera->SetAngles(0.f,0.f,0.f);
		}
	}
	else if (CAMERA_MODE_FREE_FLY == m_uiCameraMode)
	{
		g_Camera_FreeFly.AddAngles(-fTempMouseY/fSensetivity,-fTempMouseX/fSensetivity,0.f);

	} else
		BREAKPOINT(1); // known camera mode

	
	
	static bool bPressedF9Key = false;
	if(st_Input->KeyPressed(DIK_F9))
	{
		if (!bPressedF9Key)
		{
			bPressedF9Key = true;
			HandleCameraSwitch();
		}
	}
	else
		bPressedF9Key = false;






	unsigned int uiEnemyID = 0xFFFF;

	unsigned int  uiHumanPlayerID = 0xFFFFFF;
	

	CBotEntity* pBotEntity = NULL;
	m_pHumanPlayerEntity = NULL;

	for (int i=0;i<g_pServer->m_PlayersGameState.size();i++)
	{
		
		if (g_pServer->m_dwGameType == SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT)  
		{
			if (i>0 && !g_pServer->m_PlayersGameState[i]->m_bReceivedCombatHello)
					continue;
		}

		switch(g_pServer->m_PlayersGameState[i]->cControlType)
		{			
		case PLAYER_CONTROL_TYPE_HUMAN:
			
			uiHumanPlayerID = g_pServer->m_PlayersGameState[i]->uiEntityID;
			g_pServer->m_uiHumanControlledID = g_pServer->m_PlayersGameState[i]->uiEntityID;

			m_pHumanPlayerEntity = (CHumanPlayerEntity*) g_pQ3Map->GetEntity(uiHumanPlayerID);

			if (g_iLightSourceEntityUID==34)
			{
				//g_Camera_FlashLight.SetPosition(165.38f,1146.83f,-55.41f);
				g_Camera_FlashLight.SetPosition(165.38f+100.f,1146.83f,-55.41f);
				g_Camera_FlashLight.SetAngles(-87.80f,-269.40f,0.00f);					
				g_Camera_FlashLight.UpdatePositionAndMatrices(fDeltaTime);
			}

			if (!m_pHumanPlayerEntity->IsDying())
				m_pHumanPlayerEntity->HandleActionRequests(dwPlayerActionFlags,fDeltaTime);
			break;
		case PLAYER_CONTROL_TYPE_BOT:

			pBotEntity = (CBotEntity*) g_pQ3Map->GetEntity( g_pServer->m_PlayersGameState[i]->uiEntityID);


			//// make camera 2 look from the eyes of this bot
			if (g_iLightSourceEntityUID==35 || g_iLightSourceEntityUID==36)
			if (g_pServer->m_PlayersGameState[i]->uiEntityID==g_iLightSourceEntityUID)
			{


				g_Camera_FlashLight.SetPosition(pBotEntity->GetPhysicsModel()->GetPosition().x,
					pBotEntity->GetPhysicsModel()->GetPosition().y,
					pBotEntity->GetPhysicsModel()->GetPosition().z);
				
				g_Camera_FlashLight.SetAngles(pBotEntity->GetAngles().x,pBotEntity->GetAngles().y-90,pBotEntity->GetAngles().z);

				CVec3f lookat = g_Camera_FlashLight.GetLookAt();
				CVec3f newPos = pBotEntity->GetPhysicsModel()->GetPosition() - lookat*40.f;
				newPos+=CVec3f(0.f,60.f,0.f);

				g_Camera_FlashLight.SetPosition(newPos.x,newPos.y,newPos.z);
				
				g_Camera_FlashLight.UpdatePositionAndMatrices(fDeltaTime);
			}


			// handle dying
			if (!pBotEntity->IsDying())
				pBotEntity->HandleActionRequests(0,fDeltaTime);
			// debug
			if (uiEnemyID == 0xFFFF)
				uiEnemyID = pBotEntity->GetID();
			break;
		case PLAYER_CONTROL_TYPE_NETWORKED:
		{
			CNetPlayerEntity* pNetPlayer = (CNetPlayerEntity*) g_pQ3Map->GetEntity( g_pServer->m_PlayersGameState[i]->uiEntityID);

			pNetPlayer->TriggerAppropriateAnimation(fDeltaTime);

			
			
			break;
		}
		default:
			BREAKPOINT(1); // unrecognized control type!

		};


	}


	
	

	g_uiControlledHumanPlayer = uiHumanPlayerID;
		



	CVec3f vLookDir = RotateVectorByAngle(m_pHumanPlayerEntity->GetAngles().x,m_pHumanPlayerEntity->GetAngles().y-90.f,vLookDir);		
	vLookDir.Normalize();
	vLookDir*=-1.f;


	CVec3f vCameraPos(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition().x,
							m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition().y,
							m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition().z);

	/*vCameraPos+=vLookDir*50.f;
	vCameraPos.y+=100.f;*/
	
	if (CAMERA_MODE_FIRST_PERSON == m_uiCameraMode)
	{
		m_pActiveCamera->SetPosition(vCameraPos+CVec3f(0.f,45.f,0.f));
	}



	CVec3f vListenerPosition = *m_pActiveCamera->GetPosition();
	//vListenerPosition.z*=-1.f;


	g_SoundAPI.UpdateListenerPosition(vListenerPosition,m_pActiveCamera->GetLookAt());
	g_SoundAPI.Update();

	// debug
	/*if(st_Input->KeyPressed(DIK_Z))
		g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_Z_BACKWARDS);*/


	//if (st_Input->KeyPressed(DIK_8))
		//g_pRendererBack->EnableLightScissorRectTest();

	
	//if (st_Input->KeyPressed(DIK_9))
		g_pRendererBack->DisableLightScissorRectTest(); // i have some problem with this!


	// while in chat mode, don't trigger player events


	////////////////////////////////////////////////////////////////////////////////////////
	// Player keys handling


		if (!g_pMainMenu->GetActive() && !g_pMainMenu->GetChatActive())
		{
			if(st_Input->KeyPressed(DIK_1))
				m_pHumanPlayerEntity->Move_Up_DEBUG(fDeltaTime);
		}


/*	if (st_Input->KeyPressed(DIK_LALT))
	{
	// comment for camera->character
		if(st_Input->KeyPressed(DIK_U))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_Z_FORWARD);

		if(st_Input->KeyPressed(DIK_J))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_Z_BACKWARDS);

		if(st_Input->KeyPressed(DIK_H))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_X_LEFT);

		if(st_Input->KeyPressed(DIK_K))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_X_RIGHT);

		if(st_Input->KeyPressed(DIK_O))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_Y_UP);

		if(st_Input->KeyPressed(DIK_L))
			g_Camera.AddMoveRotateFlags(CAMERA_ACTION_MOVE_Y_DOWN);
	}

*/	
	


	// z axis rotation doesn't work :/

	/*if(st_Input->KeyPressed(DIK_Z))
		g_Camera.AddMoveRotateFlags(CAMERA_ACTION_ROT_Z_LEFT);

	if(st_Input->KeyPressed(DIK_C))
		g_Camera.AddMoveRotateFlags(CAMERA_ACTION_ROT_Z_RIGHT);*/

	


	m_pActiveCamera->UpdatePositionAndMatrices(fDeltaTime);
	g_pGraphicsAPI->SetViewMatrix(m_pActiveCamera->ReturnCameraMatrixfa());


	tUniform* cameraPos = g_pAutoUniforms->Get("cameraPos");
	
	cameraPos->m_pfValue[0] = m_pActiveCamera->GetPosition()->v[0];
	cameraPos->m_pfValue[1] = m_pActiveCamera->GetPosition()->v[1];
	cameraPos->m_pfValue[2] = m_pActiveCamera->GetPosition()->v[2];

	
	/*if(st_Input->KeyPressed(DIK_C))
	{
		char temp[100];
		sprintf_s(temp,"Camera Pos: %.2f,%.2f,%.2f",
			cameraPos->m_pfValue[0],
			cameraPos->m_pfValue[1],
			cameraPos->m_pfValue[2]);
		g_TextMessages.AddMessage(temp,1.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);
	}*/



	/*g_pGraphicsAPI->AddDebugText(100,100,"Camera Pos:%f,%f,%f\n",
		g_Camera.GetPosition()->x,
		g_Camera.GetPosition()->y,
		g_Camera.GetPosition()->z);*/

	/*g_pGraphicsAPI->AddDebugText("Entities Drawn: %d\n",g_idbgEntitiesDrawnNum);*/

/*
	m_pCharacter->SetPosition(CVec3f(180.f,0.f,260.f));
	m_pCharacter->SetAngles(CVec3f(0.f,180.f,0.f));	

	*/

	ILight_Export** ppLights = g_pQ3Map->GetLights();
	int iLightsNum           = g_pQ3Map->GetLightsNum();


	/*if(st_Input->KeyPressed(DIK_0))
	{
		m_pCharacter3->SetPosition(CVec3f(550.f,0.f,160.f));		
	}
	else
		m_pCharacter3->SetPosition(m_pCharacter3->GetPosition()
								-CVec3f(g_pTimer->GetDeltaTimeFAKE()*0.04f,0.f,0.f));*/


	// make light follow mancubus
	/*ppLights[0]->SetPos(m_pCharacter3->GetPosition()+CVec3f(60.f,200.f,0.f));
	ppLights[0]->SetLookAt(CVec3f(-1.f,-1.f,0.f).Normalize());*/



	

	static float fDbgRotY = 0.f;




	// ENEMY CONTROL
	
	//pEnemyEntity->GetRenderModel()->RequestMoveState(ANI_TYPE_IDLE);
/*
	// enemy controls
	if (!st_Input->KeyPressed(DIK_LALT))
	{
		if (st_Input->KeyPressed(DIK_H))
			//pEnemyEntity->Move_RotateLeft(fDeltaTime,false);
			pEnemyEntity->Move_RotateLeft(fDeltaTime,false);

		if (st_Input->KeyPressed(DIK_K))
			//pEnemyEntity->Move_RotateRight(fDeltaTime,false);
			pEnemyEntity->Move_RotateRight(fDeltaTime,false);

		if (st_Input->KeyPressed(DIK_U))
			pEnemyEntity->Move_RunForward(fDeltaTime);

		if (st_Input->KeyPressed(DIK_J))
			pEnemyEntity->Move_RunBackwards(fDeltaTime);


		if (st_Input->KeyPressed(DIK_P))
			pEnemyEntity->Attack1();

		
	}


*/	



	///////////////////////////////////////////////////////////////////////
	// debugging

	tUniform* tweak0 = g_pAutoUniforms->Get("tweakable0");
	tUniform* tweak1 = g_pAutoUniforms->Get("tweakable1");

	static bool bInitedTweakables = false;

	if (!bInitedTweakables)
	{
		bInitedTweakables = true;

		// d3d bias/offset
		//tweak0->m_pfValue[0] = 0.995f;
		//tweak1->m_pfValue[0] = 0.001550f;
		 


		// gl bias/offset
		//bias:0.998279, offset:0.003860
		//tweak0->m_pfValue[0] = 0.998279;
		//tweak1->m_pfValue[0] = 0.003860;

		//bias:0.024000, offset:0.000000
		//tweak0->m_pfValue[0] = 0.024;

		//bias:1.225999, offset:0.000000

		//tweak0->m_pfValue[0] = 0.046;
		tweak0->m_pfValue[0] = 0.f;
		tweak1->m_pfValue[0] = 0.f;


	}
	
	float fSlowSpeed = 0.0001f;
	


	if(st_Input->KeyPressed(DIK_7))
		tweak0->m_pfValue[0]+=fSlowSpeed;

	if(st_Input->KeyPressed(DIK_8))
		tweak0->m_pfValue[0]-=fSlowSpeed;

	if(st_Input->KeyPressed(DIK_9))
		tweak1->m_pfValue[0]+=fSlowSpeed;

	if(st_Input->KeyPressed(DIK_0))
		tweak1->m_pfValue[0]-=fSlowSpeed;



	static int s_TessLevel=5;

	
	static bool s_bPressedIncreseTessAlready = false;	

	

	if (g_pEngineOptions->GetRendRTTBox() && st_Input->KeyPressed(DIK_T))
	{
		if (!s_bPressedIncreseTessAlready)
		{
			s_bPressedIncreseTessAlready = true;
			s_TessLevel++;

			if (s_TessLevel<2)
			s_TessLevel=2;
			char temp[300];
			char temp2[10];

			itoa(s_TessLevel,temp2,10);

			sprintf(temp,"$%s",temp2);
			m_pBox->SetShared(st_StaticModelManager->Load(temp,0.5f,1.0f,0.5f),RENDERABLE_GENERATE_PPL);		
			m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/box-rtt-effect-flagged.mat"));

			float pfWorld[16];
			CMatrix world;
			CMatrix trans;		
			trans.MatMakeMove(495.50f,70.f,113.80f);	
			trans.ReturnArray(pfWorld);
		
			m_pBox->SetTransformForAll(pfWorld);
		}
	}
	else
		s_bPressedIncreseTessAlready = false;


	static bool s_bPressedDecreaseTessAlready = false;

	if (g_pEngineOptions->GetRendRTTBox() && st_Input->KeyPressed(DIK_Y))
	{
		if (!s_bPressedDecreaseTessAlready)
		{
			s_TessLevel--;

			if (s_TessLevel<2)
				s_TessLevel=2;

			char temp[300];
			char temp2[10];

			itoa(s_TessLevel,temp2,10);

			sprintf(temp,"$%s",temp2);
			m_pBox->SetShared(st_StaticModelManager->Load(temp,0.5f,1.0f,0.5f),RENDERABLE_GENERATE_PPL);		
			m_pBox->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/box-rtt-effect-flagged.mat"));
			float pfWorld[16];
			CMatrix world;
			CMatrix trans;		
			trans.MatMakeMove(495.50f,70.f,113.80f);	
			trans.ReturnArray(pfWorld);
			
			m_pBox->SetTransformForAll(pfWorld);
		}
	}
	else
		s_bPressedDecreaseTessAlready = false;




	static bool s_bPressedDeadReckoningAlready = false;

	if (st_Input->KeyPressed(DIK_N))
	{
		if (!s_bPressedDeadReckoningAlready)
		{
			s_bPressedDeadReckoningAlready = true;
			g_bDeadReckoning = !g_bDeadReckoning;
		}

	}
	else
		s_bPressedDeadReckoningAlready = false;



	if(st_Input->KeyPressed(DIK_F7))
	{
		g_iLightSourceEntityUID = 34;
	}

	if(st_Input->KeyPressed(DIK_F8))
	{
		g_iLightSourceEntityUID = 35;
	}

	/*if(st_Input->KeyPressed(DIK_F9))
	{
		g_iLightSourceEntityUID = 36;
	}*/
	
	///////////////////////////////////////////////////////////////////////
	
	if(st_Input->KeyPressed(DIK_F2))
	{
		st_Input->SetCooperativeLevel(DISCL_NONEXCLUSIVE);
	}

	
	static bool bLightIsCam = true;
	static bool bPressedLightModeKey = false;
	if(st_Input->KeyPressed(DIK_F5))
	{
		if (!bPressedLightModeKey)
		{
			bPressedLightModeKey = true;
			bLightIsCam=!bLightIsCam;

			if (bLightIsCam)
				g_pRendererBack->DisableLightScissorRectTest();
			else
				g_pRendererBack->EnableLightScissorRectTest();
		}
	}
	else
		bPressedLightModeKey = false;

	//if(st_Input->KeyPressed(DIK_1))
	if (bLightIsCam)
	{
		ILight_Export** ppLights = g_pQ3Map->GetLights();
		
		////// player=light version

		if (g_iLightSourceEntityUID==34)
		{
			ppLights[0]->SetPos((*m_pActiveCamera->GetPosition())-(m_pActiveCamera->GetLookAt()*10.f) +
				CVec3f(0.f,20.f,0.f)
				);

			ppLights[0]->SetLookAt(m_pActiveCamera->GetLookAt()+CVec3f(0.f,-0.4f,0.f));
		}
		else
		{
			ppLights[0]->SetPos((*g_Camera_FlashLight.GetPosition())-(g_Camera_FlashLight.GetLookAt()*10.f) +
			CVec3f(0.f,50.f,0.f)
			);
		
			ppLights[0]->SetLookAt(g_Camera_FlashLight.GetLookAt()+CVec3f(0.f,-0.4f,0.f));

		}
			


		
		
		ppLights[0]->Update();
		g_pQ3Map->LinkRenderablesToLights();
		g_pQ3Map->LinkLevelEntitiesToLights(true);

		// temp
		//g_pQ3Map->DebugApplyForceOnAllNonStaticEntities(CVec3f(-1.f,0.f,0.f));
	}
	

	
	g_pQ3Map->UpdateLevelEntitiesPhysics(g_pTimer->GetDeltaTimeFAKE());
	
	// FIXME: don't do this all the time! only for things that move
	g_pQ3Map->ReLinkMapEntitiesToLeafs(false);


	g_pQ3Map->LinkRenderablesToLights();
	g_pQ3Map->LinkLevelEntitiesToLights(false);

	//g_TextMessages.UpdateMessages();

	char playerStatus[100];
	sprintf_s(playerStatus,100,"Life:%d Ammo:%d",
		m_pHumanPlayerEntity->GetLife(),
		m_pHumanPlayerEntity->GetAmmo(1));


	g_TextMessages.SetPlayerInfo(playerStatus);

	ShowScore();


	// Game Modes:

	// Look for match end related event

	float fPassedTime = g_pTimer->GetTime() - m_fTimeLimit_TimeStarted;

	if (fPassedTime >= m_fTimeLimit_MatchTime) // game time limit reached
	{
		//////////////////////////////////////////////////////////////////////////
		// Match End - search for winner

		int iWinnerID = NO_LEADER;

		iWinnerID = FindWinner();

		if (NO_LEADER != iWinnerID)
		{
			// We have found a winner !
			char str[255];		
			
			g_TextMessages.AddMessage("---=====================================---",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_GAME);
			sprintf_s(str,255,"Victory ! %s Won.",
				(*m_pPlayersGameState)[iWinnerID]->pName);	
			g_TextMessages.AddMessage(str,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);
			g_TextMessages.AddMessage("---=====================================---",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_GAME);

			if ((*m_pPlayersGameState)[iWinnerID]->uiEntityID == g_uiControlledHumanPlayer)
			{
				CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/player/announce/youwin.wav");						
					spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);
			} else
			{
				CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/world/buzzer.wav");						
					spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);		

			}

			RestartMatch();

			m_uiLeaderID = NO_LEADER;
			return;
		} else // there is a tie
		{
			if (!m_bMatchPoint)
			{
				m_bMatchPoint = true;
				CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/feedback/tiedlead.wav");						
					spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);
				

			}
			
		}

	} else if ( (m_fTimeLimit_MatchTime-fPassedTime <= 1000.f*60.f*5.f) && !m_bAnnounced_5_minutes_left)
	{
		CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/feedback/5_minute.wav");						
			spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);

		m_bAnnounced_5_minutes_left = true;
	} else if ( (m_fTimeLimit_MatchTime-fPassedTime <= 1000.f*60.f*1.0f) && !m_bAnnounced_1_minute_left)
	{

		CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/feedback/1_minute.wav");						
			spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);

		m_bAnnounced_1_minute_left = true;
	}

}

void CGame::Render(void)
{

	/*char msg[150];
	sprintf(msg,"*****************************************************Game Render() Started.");
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/

	/*static CProfileStep sleepStep("sleep");	
	sleepStep.Begin();
	Sleep(1000);
	sleepStep.End();*/

	float fDeltaTime = g_pTimer->GetDeltaTimeFAKE();


	/*static float fRotY = 0;
	fRotY += fDeltaTime / 10.f;*/



	/// Rendering the shadow maps into the lights
	


	

	ILight_Export** ppLights = g_pQ3Map->GetLights();
	int iLightsNum = g_pQ3Map->GetLightsNum();
	
	float pfTempLightView[16];
	float pfTempLightProjection[16];



	////////////////////////////////////////////////////////////////////
	// Render DepthMaps from lights point-of-view


if (!g_pRendererFront->GetRoughFallBack())
{

		for (int i=0;i<iLightsNum;i++)
		{


			// DEBUG
			// for now re-enable it, since there still isn't a mechanism
			// that tags it dirty in case of update needed to the shadow map
			if (i==0 || i==1) // only for lights with potential shadow map change...
				ppLights[i]->SetDirty(true);



			if (!ppLights[i]->GetActive())
				continue;

			if (!ppLights[i]->GetDirty())
				continue;

			ppLights[i]->SetDirty(false);
							
			g_pGraphicsAPI->SetProjectionMatrix(ppLights[i]->GetProjectionMatrix());
			g_pGraphicsAPI->SetViewMatrix(ppLights[i]->GetViewMatrix());


			g_pRendererFront->Clean();
			
			//g_pLogFile->OutPutPlainText("CGame::Render binding render to depth map",LOG_MESSAGE_INFORMATIVE);
			ppLights[i]->BindRenderToDepthMap();
			//g_pLogFile->OutPutPlainText("CGame::Render binding render to depth map /DONE",LOG_MESSAGE_INFORMATIVE);


			// restore restore restore
			g_pQ3Map->SetFrustum(ppLights[i]->GetFrustum());	
			g_pQ3Map->SetPrebatching(false);
			g_pQ3Map->SetUpdateEntitiesRenderModels(false);

			// force a render of yourself in order to cast a shadow
			//g_pQ3Map->RenderEntity(g_iHumanPlayerNum,fDeltaTime*0.001f);

			//g_pQ3Map->SetRenderHumanPlayer(true);
			
			
			//ORIGINAL - 11.02.2011
			//g_pQ3Map->SetRenderHumanPlayer(false);
			
			g_pQ3Map->RenderLevel_ShadowCasters(ppLights[i]->GetPos());
			
			

			//m_pBox2->Render(0);

			//g_pLogFile->OutPutPlainText("CGame::rendering front-end depthmap rendering",LOG_MESSAGE_INFORMATIVE);
			g_pRendererFront->RenderDepthMap(ppLights[i]);
			//g_pLogFile->OutPutPlainText("CGame::rendering front-end depthmap rendering /DONE",LOG_MESSAGE_INFORMATIVE);
		
			//g_pLogFile->OutPutPlainText("CGame::rendering unbind render to depth map",LOG_MESSAGE_INFORMATIVE);
			ppLights[i]->UnbindRenderToDepthMap();
			//g_pLogFile->OutPutPlainText("CGame::rendering unbind render to depth map /DONE",LOG_MESSAGE_INFORMATIVE);

		}				
		

		g_pRendererBack->DisableForceCulling();
}
////////////////////////////////////////////////////////////////////	

////////////////////////////////////////////////////////////////////
// Render RTT box

	if (g_pEngineOptions->GetRendRTTBox())
	//if (0)
	{
		m_pActiveCamera->UpdatePositionAndMatrices(fDeltaTime);
		g_Camera_FlashLight.UpdatePositionAndMatrices(fDeltaTime);

		g_pGraphicsAPI->SetViewMatrix(g_Camera_FlashLight.ReturnCameraMatrixfa());		
		//g_pGraphicsAPI->SetViewMatrix(g_Camera_FlashLight.ReturnMIRROREDCameraMatrixfa());		

		//TODO: change this, and grab projection matrix from camera!
		g_pGraphicsAPI->SetProjectionMatrix(90.0f, (float)(g_pGraphicsAPI->GetResX())/(float)(g_pGraphicsAPI->GetResY()),CR_NEAR_Z, CR_FAR_Z);
		
		g_Frustum.CalculateFrustum(g_pGraphicsAPI->GetViewMatrix(),g_pGraphicsAPI->GetProjectionMatrix());

		g_pRendererFront->Clean();

		g_pRendererBack->StartCamera(RENDER_CAMERA_0,0.f,0.f,0.f,1.0f);	

		g_pQ3Map->SetFrustum(&g_Frustum);

		/*if (g_pRendererFront->GetRoughFallBack())*/
			//g_pQ3Map->SetPrebatching(true);
		/*else
			g_pQ3Map->SetPrebatching(false);*/


		g_pQ3Map->SetUpdateEntitiesRenderModels(false);

		//Original 11.02.2011
		//g_pQ3Map->SetRenderHumanPlayer(true);
		g_pQ3Map->RenderLevel((*g_Camera_FlashLight.GetPosition()));	

		


		g_pRendererFront->Render(&g_Frustum);

		g_pRendererBack->EndCamera();

	}
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
// Render main scene


	// debug debug debug
	//g_pRendererBack->SetForceCulling(CR_FRONT);

	//// Rendering into PRE_PROCESSING_FINAL

		m_pActiveCamera->UpdatePositionAndMatrices(fDeltaTime);
		g_pGraphicsAPI->SetViewMatrix(m_pActiveCamera->ReturnCameraMatrixfa());		

		//TODO: change this, and grab projection matrix from camera!
		g_pGraphicsAPI->SetProjectionMatrix(90.0f, (float)(g_pGraphicsAPI->GetResX())/(float)(g_pGraphicsAPI->GetResY()),CR_NEAR_Z, CR_FAR_Z);
		
		g_Frustum.CalculateFrustum(g_pGraphicsAPI->GetViewMatrix(),g_pGraphicsAPI->GetProjectionMatrix());

		g_pRendererFront->Clean();

		
		g_pRendererBack->StartCamera(PRE_PROCESSING_FINAL,0.f,0.f,0.f,1.0f);	
		
		//g_pRendererBack->StartCamera(RENDER_CAMERA_DEPTH,0.f,0.f,0.f,1.0f);	
		//g_pRendererBack->StartCamera(RENDER_CAMERA_0,0.f,0.f,0.f,1.0f);	
		// Rendering the Quake3 Map
		// RESTORE RESTORE RESTORE
		
		g_pQ3Map->SetFrustum(&g_Frustum);
		//g_pQ3Map->SetPrebatching(false);

		/*if (g_pRendererFront->GetRoughFallBack())*/
			//g_pQ3Map->SetPrebatching(true);
		/*else
			g_pQ3Map->SetPrebatching(false);*/

		g_pQ3Map->SetUpdateEntitiesRenderModels(true);
		//g_idbgEntitiesDrawnNum = 0;
		/*g_pGraphicsAPI->AddDebugText(100,100,"Camera is in leaf:%d",
			g_pQ3Map->FindLeaf((*g_Camera.GetPosition())));*/
		
		//ORIGINAL 11.02.2011
		//g_pQ3Map->SetRenderHumanPlayer(false);
		//g_pQ3Map->SetRenderHumanPlayer(true);


		// render depth first

		g_pQ3Map->RenderLevel((*m_pActiveCamera->GetPosition()));		
		
		if (g_pEngineOptions->GetRendRTTBox())
		{
			m_pBox->Render(0);	
		}		

		// render parallax mapping test
		//m_pBox2->Render(0);

		//g_pGraphicsAPI->UnBindVertexShader();


		//g_pGraphicsAPI->SetBlendFunc(CR_BF_ONE,CR_BF_ONE);


		// render depth first
		//g_pRendererFront->RenderDepthFirst();
		

		g_pRendererBack->EndCamera();

		// Render main scene
		g_pRendererFront->Render(&g_Frustum);

////////////////////////////////////////////////////////////////////////////////////



		//g_pRendererBack->DisableForceCulling();

		// debug render
		//g_pGraphicsAPI->DrawDebugLine(0.f,0.f,0.f,0.f,2500.f,0.f);

				
		

		//g_pQ3Map->RenderFaceBBox(iDbgFaceNum);


		// return to fixed pipeline

		g_pGraphicsAPI->UnBindVertexShader();
		g_pGraphicsAPI->UnBindFragmentShader();

		
		//g_pQ3Map->RenderAllFacesBBox();


// DEBUG RENDERING

	// debug render entities

	g_pQ3Map->RenderAllEntitiesBBoxes();

	/*g_pQ3Map->DrawMapLeafsBBoxes();
	
	//////////////////////////////////////////////////////////////////////////////////////
	// debug rendering of frustum

	CVector4f* pCorners4d = ppLights[0]->GetFrustum()->GetCorners();

	CVec3f  pCorners[8];

	for (int i=0;i<8;i++)
	{
		pCorners[i].x = pCorners4d[i].x;
		pCorners[i].y = pCorners4d[i].y;
		pCorners[i].z = pCorners4d[i].z;
	}

	CVec3f green(0.f,1.f,0.f);

	// back square		
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_DOWN_LEFT_CORNER],pCorners[FRUSTUM_BACK_UP_LEFT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_UP_LEFT_CORNER],pCorners[FRUSTUM_BACK_UP_RIGHT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_UP_RIGHT_CORNER],pCorners[FRUSTUM_BACK_DOWN_RIGHT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_DOWN_RIGHT_CORNER],pCorners[FRUSTUM_BACK_DOWN_LEFT_CORNER],green);

	// front square	
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_FRONT_DOWN_LEFT_CORNER],pCorners[FRUSTUM_FRONT_UP_LEFT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_FRONT_UP_LEFT_CORNER],pCorners[FRUSTUM_FRONT_UP_RIGHT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_FRONT_UP_RIGHT_CORNER],pCorners[FRUSTUM_FRONT_DOWN_RIGHT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_FRONT_DOWN_RIGHT_CORNER],pCorners[FRUSTUM_FRONT_DOWN_LEFT_CORNER],green);

	// rendering lines connecting back with far
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_DOWN_LEFT_CORNER],pCorners[FRUSTUM_FRONT_DOWN_LEFT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_UP_LEFT_CORNER],pCorners[FRUSTUM_FRONT_UP_LEFT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_UP_RIGHT_CORNER],pCorners[FRUSTUM_FRONT_UP_RIGHT_CORNER],green);
	g_pGraphicsAPI->DrawDebugLine(pCorners[FRUSTUM_BACK_DOWN_RIGHT_CORNER],pCorners[FRUSTUM_FRONT_DOWN_RIGHT_CORNER],green);
	//////////////////////////////////////////////////////////////////////////////////////	
	

		
		//////////////////////////////////////////////////////////////////////////////////////////
		// debug rendering of first light scissor rect
		float fXmin,fXmax,fYmin,fYmax;
					
		ppLights[0]->GetFrustum()->GetProjectedCornersBoundingSquare(fXmin,fXmax,fYmin,fYmax);

				
		fXmin = (fXmin-0.5f)*2.f;
		fXmax = (fXmax-0.5f)*2.f;
		fYmin = (fYmin-0.5f)*2.f;
		fYmax = (fYmax-0.5f)*2.f;

		float fPixelSizeX = 1.f / (float) g_pGraphicsAPI->GetResX();
		float fPixelSizeY = 1.f / (float) g_pGraphicsAPI->GetResY();

		fXmin+= fPixelSizeX;
		fXmax-= fPixelSizeX;

		fYmin+= fPixelSizeY;
		fYmax-= fPixelSizeY;


		// top
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmin,fYmin),CVec2f(fXmax,fYmin));
		// bottom
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmin,fYmax),CVec2f(fXmax,fYmax));
		// left
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmin,fYmin),CVec2f(fXmin,fYmax));
		// right
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmax,fYmin),CVec2f(fXmax,fYmax));

		// cross 1
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmin,fYmin),CVec2f(fXmax,fYmax));

		// cross 2
		g_pGraphicsAPI->DrawDebugLine2D(CVec2f(fXmin,fYmax),CVec2f(fXmax,fYmin));

		//////////////////////////////////////////////////////////////////////////////////////////
		*/
		
		
		//g_TextMessages.DrawMessages();
		
				
		//if (g_pEngineOptions->GetDepthOfField())
		
		if (g_bRenderableRequirePostProcessing)
		{
			//g_pRendererBack->PerformPostProcess();
			g_pMainMenu->Update();
			g_pMainMenu->Render();
			g_pRendererBack->EndCamera();	
			//g_pGraphicsAPI->EndScene();

			//g_pRendererBack->EndCamera();	

			/*g_pMainMenu->Update();
			g_pMainMenu->Render();*/
		}
		else		
		{
			g_pMainMenu->Update();
			g_pMainMenu->Render();
			g_pRendererBack->EndCamera();		
		}
		
	

			
	/*sprintf(msg,"**************************Game Render() Ended.");
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/

	//BREAKPOINT(1);
}

int CGame::FindWinner()
{
	/*// name
	char str_curr[256];
	char str[512];
	str[0] = NULL;*/

	int iBestScore = -9999;
	
	bool bFoundWinner = false;
	bool iWinnerID = -1;

	for (int i=0;i<m_pPlayersGameState->size();i++)
	{		
		// should remove the output to text from here to per score changing event
		/*sprintf_s(str_curr,255,"%s:%d  ",
			(*m_pPlayersGameState)[i]->pName,
			(*m_pPlayersGameState)[i]->iScore);
		strcat_s(str,512,str_curr);		*/

		if ((*m_pPlayersGameState)[i]->iScore == iBestScore)
		{
			bFoundWinner = false;
		} else
		if ((*m_pPlayersGameState)[i]->iScore > iBestScore)
		{
			iBestScore = (*m_pPlayersGameState)[i]->iScore;
			bFoundWinner = true;
			iWinnerID = i;
		}
	}

	//g_TextMessages.AddMessage(str,1.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);

	if (bFoundWinner)
	{
		return iWinnerID;
	}

	return NO_WINNER;
}

unsigned int CGame::FindLeaderID(int& iLeaderScore)
{
	unsigned int uiLeaderID = NO_LEADER;
	iLeaderScore = -9999;

	unsigned int uiHowManyWinners = 0;

	for (int i=0;i<m_pPlayersGameState->size();i++)
	{		
		/*sprintf_s(str_curr,255,"%s:%d  ",
			(*m_pPlayersGameState)[i]->pName,
			(*m_pPlayersGameState)[i]->iScore);
		strcat_s(str,512,str_curr);		*/

		if ( ((*m_pPlayersGameState)[i]->iScore) > iLeaderScore)
		{
			iLeaderScore = ((*m_pPlayersGameState)[i]->iScore);
			uiLeaderID = ((*m_pPlayersGameState)[i])->uiEntityID;		
		}

		if ( ((*m_pPlayersGameState)[i]->iScore) == iLeaderScore)
		{
			uiHowManyWinners++;
		}
	}

	if (uiHowManyWinners>1) // multiple winners
	{
		uiLeaderID = NO_LEADER;
	}

	return uiLeaderID;
}

void CGame::OnEvent(DWORD dwGameEvent, void* pEventData)
{
	char temp[300];
	char temp2[100];

	//int iWinnerID = NO_LEADER;


	if (dwGameEvent == COMBAT_EVENT__SERVER_UPDATE__PROJECTILE_SHOOT)
	{
		tCombatEvent_ServerUpdate_ProjectileShoot* pData = (tCombatEvent_ServerUpdate_ProjectileShoot*) pEventData;

		// handle the event
		CControllableEntity* pShooter = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		BREAKPOINT(!pShooter);
		
		//pShooter->GetRenderModel()->PlayAction(ANI_TYPE_ATTACK_1);
		
		switch(pData->ucProjectileType)
		{
		case WEAPON_TYPE_ROCKET:
			g_pQ3Map->SpawnProjectile(WEAPON_TYPE_ROCKET, CVec3f(pData->fStartPosition) , CVec3f(pData->fDirection)*WEAPON_ROCKET_SPEED,pData->uiPlayerEntityID);
			break;
		case WEAPON_TYPE_FIREBALL:
			g_pQ3Map->SpawnProjectile(WEAPON_TYPE_FIREBALL, CVec3f(pData->fStartPosition) , CVec3f(pData->fDirection)*WEAPON_FIREBALL_SPEED,pData->uiPlayerEntityID);
			break;
		case WEAPON_TYPE_BLUE:
			g_pQ3Map->SpawnProjectile(WEAPON_TYPE_BLUE, CVec3f(pData->fStartPosition) , CVec3f(pData->fDirection)*WEAPON_BLUE_SPEED,pData->uiPlayerEntityID);
			break;
		case WEAPON_TYPE_WHITE:
			g_pQ3Map->SpawnProjectile(WEAPON_TYPE_WHITE, CVec3f(pData->fStartPosition) , CVec3f(pData->fDirection)*WEAPON_WHITE_SPEED,pData->uiPlayerEntityID);
			break;		
		default:
			{
				char msg[500];
				sprintf(msg,"Unfamiliar projectile type! [%d]",pData->ucProjectileType);
				g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);
				BREAKPOINT(1);
				break;
			}
		}
	}
	else if (dwGameEvent == GAME_EVENT_RESPAWN_PLAYER)
	{
		// interpret incoming data
		tRespawnPlayer_Game* pData = (tRespawnPlayer_Game*) pEventData;

		// handle the event
		g_pQ3Map->RespawnPlayer(pData->uiPlayerEntityID,(int) pData->cStartPosition);

	}
	else if (dwGameEvent == GAME_EVENT_PLAYER_DIED_BY_LEVEL)
	{
		// interpret incoming data
		tPlayerDiedByLevel_Game* pData = (tPlayerDiedByLevel_Game*) pEventData;	

		// handle the event
		CControllableEntity* pContEnt_suicider = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		pContEnt_suicider->SetDying();

		tPlayerGameState* pPlayerGS = g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID);

		sprintf_s(temp,300,"%s just killed self.",
			pPlayerGS->pName);
		g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);

		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		st_SoundManager->Load("sound/player/bitterman/falling1.wav")->Play(pContEnt->GetPhysicsModel()->GetPosition(),200);

		

		pPlayerGS->iScore--;

		//iWinnerID = FindWinner();
		
	} 
	else if (dwGameEvent == GAME_EVENT_PLAYER_DIED_BY_WEAPON)
	{
		// interpret incoming data
		tPlayerDiedByWeapon_Game* pData = (tPlayerDiedByWeapon_Game*) pEventData;	

		// handle the event
		CControllableEntity* pContEnt_killer = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID_killer);
		CControllableEntity* pContEnt_victom = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID_victom);
		pContEnt_victom->SetDying();
		
		if (pData->uiPlayerEntityID_killer == pData->uiPlayerEntityID_victom)
		{
			sprintf_s(temp,300,"%s died from his own weapon.",
				g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_victom)->pName);

			g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_victom)->iScore--;

			g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);
		}
		else
		{
			sprintf_s(temp,300,"%s was fragged by %s.",
				g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_victom)->pName,
				g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_killer)->pName);		
			g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_killer)->iScore++;

			g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);
		}
				
		//iWinnerID = FindWinner();
				
		// here i should trigger some death animation in the victem
		// and change scores to victem/killer
		

	}
	else if (dwGameEvent==GAME_EVENT_PLAYER_DAMAGE)
	{
		// interpert incoming data
		tPlayerDamageEvent_Game* pData = (tPlayerDamageEvent_Game*) pEventData;	

		// handle the event
		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);
		pContEnt->SetLife(pContEnt->GetLife()-pData->iDamage);
		pContEnt->GetPhysicsModel()->m_vVelocity+=pData->vApplyForce;

		BREAKPOINT(   *((unsigned long*)&pContEnt->GetPhysicsModel()->m_vVelocity.x) == 0xffc00000 );

		((CCharacter*)pContEnt->GetRenderModel())->PlayAction(ANI_TYPE_PAIN);


	}
	else if (dwGameEvent==COMBAT_EVENT_COLLECTABLE_PICKUP_UPDATE)
	{
		// interpert incoming data
		tCombatEventCollectablePickupUpdate* pData = (tCombatEventCollectablePickupUpdate*) pEventData;	

		// handle the event
		CCollectableEntity* pCollEnt = (CCollectableEntity*) g_pQ3Map->GetEntity(pData->uiItemID);

		// inform the collectable entity that it was picked up
		pCollEnt->PickedUp();

		// inform player that 
		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		pContEnt->PickUp(pCollEnt->GetItemType());

		

		switch(pCollEnt->GetItemType())
		{
		case COLLECTABLE_ITEM_TYPE_HEALTH:
			sprintf_s(temp2,100,"Health Small");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);

			//g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID)->m_i
			break;
		case COLLECTABLE_ITEM_TYPE_ARMOR_SHARD:
			sprintf_s(temp2,100,"Armor Shard");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;
		case COLLECTABLE_ITEM_TYPE_ARMOR_BODY:
			sprintf_s(temp2,100,"Armor Body");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;
		case COLLECTABLE_ITEM_TYPE_ARMOR_COMBAT:
			sprintf_s(temp2,100,"Armor Combat");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;
		case COLLECTABLE_ITEM_TYPE_AMMO_1:
			sprintf_s(temp2,100,"Ammo 1");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;
		case COLLECTABLE_ITEM_TYPE_AMMO_2:
			sprintf_s(temp2,100,"Ammo 2");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;
		case COLLECTABLE_ITEM_TYPE_AMMO_3:
			sprintf_s(temp2,100,"Ammo 2");
			//st_SoundManager->PlaySound("Sound/Items/l_health.wav",pCollEnt->GetPhysicsModel()->GetPosition(),150);
			st_SoundManager->Load("Sound/Items/l_health.wav")->Play(pCollEnt->GetPhysicsModel()->GetPosition(),150);
			break;

		default:
			BREAKPOINT(1); // unrecognized item!
			break;
		};					
		
		sprintf(temp,"%s picked up %s.",g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID)->pName,temp2);
		g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);


	}
	else if (dwGameEvent==GAME_EVENT_PLAYER_TELEPORTED)
	{
		
		// interpert incoming data
		tTeleportedEvent_Game* pData = (tTeleportedEvent_Game*) pEventData;	

		// handle the event
		CTeleporterEntity* pTeleportEnt = (CTeleporterEntity*) g_pQ3Map->GetEntity(pData->uiTeleporterID);
		// do sth with it - trigger sound and maybe animation as well

		// affect player
		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);

		pContEnt->GetPhysicsModel()->SetPosition(pData->vTeleportTarget + CVec3f(0.f,10.f,0.f));

		float fYAng = pTeleportEnt->GetRotYAngle();

		pContEnt->SetAngles(CVec3f(0.f,fYAng,0.f));
		


	} else if (dwGameEvent==GAME_EVENT_PLAYER_LEVEL_DAMAGE)
	{
		// interpret incoming data
		tLevelDamageEvent_Game* pData = (tLevelDamageEvent_Game*) pEventData;	

		// handle the event

		// affect player
		CControllableEntity* pContEnt = (CControllableEntity*) g_pQ3Map->GetEntity(pData->uiPlayerEntityID);
		pContEnt->SetLife(pContEnt->GetLife()-pData->iDamage);

		// if it was networked, here i should update other client as well

	}else
	{
		BREAKPOINT(1); // ERROR! unrecognized event !!!!
	}


	//////////////////////////////////////////////////////////////////////////
	// Handle leading

	int iLeaderScore = -9999;
	unsigned int uiLeaderID = FindLeaderID(iLeaderScore);

	//if (g_pServer->GetPlayerStateFromFinalVector(pData->uiPlayerEntityID_killer)->iScore > iLeaderScore)
	if (m_uiLeaderID != uiLeaderID) // if leader changed
	{
		if (!m_bMatchPoint)
		{
			if (g_uiControlledHumanPlayer == uiLeaderID) // The human player became the leader
			{
				g_TextMessages.AddMessage("You have taken the lead!",0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);

				CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/feedback/takenlead.wav");						
				spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);
				
			} else if (g_uiControlledHumanPlayer == m_uiLeaderID) //The human player was the leader, and now lost it
			{
				g_TextMessages.AddMessage("You have lost the lead!",0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);

					CSmartPointer<CSound> spSound = st_SoundManager->Load("sound/feedback/lostlead.wav");						
				spSound->Play(m_pHumanPlayerEntity->GetPhysicsModel()->GetPosition(),255);			

			} else 
			{
				if (NO_LEADER!=uiLeaderID)
				{
					sprintf_s(temp,300,"%s has taken the lead!",g_pServer->GetPlayerStateFromFinalVector(uiLeaderID)->pName);
					g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_GAME);
				}
				
			}
		}
		
		m_uiLeaderID = uiLeaderID;			
	}

	//////////////////////////////////////////////////////////////////////////
}


void CGame::RestartMatch()
{
	// Frag Limit

	m_uiFragLimit_FragsLimit = 15;
	m_uiFragLimit_RemainingFrags = m_uiFragLimit_FragsLimit;

	// Time Limit

	m_fTimeLimit_MatchTime = 1000.f*60.f*8.f;
	m_fTimeLimit_TimeStarted = g_pTimer->GetTime();

	m_bAnnounced_5_minutes_left	= false;
	m_bAnnounced_1_minute_left	= false;

	m_bMatchPoint = false;

	for (int i=0;i<m_pPlayersGameState->size();i++)
		{
			(*m_pPlayersGameState)[i]->iScore = 0;			
		}

	/////////////////////////////////////////////////////////////////////
}