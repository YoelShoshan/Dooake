#include "stdafx.h"
#include "GameApplication.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "StaticModel.h"

#include "ClipBoard.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "RingBuffer.h"


#include "Game.h"
extern CGame* g_pGame;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Character.h"

#include "Camera.h"
extern CCamera g_Camera_FirstPerson;
extern CCamera g_Camera_FlashLight;

#include "Input.h"
#include "Timer_IE.h"
extern ITimer* g_pTimer;
#include "Profiler.h"
extern IProfiler* g_pProfiler;


#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "TagLogics.h"
extern CTagLogics g_TagLogics;

#include "Window3D.h"
extern CWindow3D g_Window;

#include "Frustum.h"
extern CFrustum g_Frustum;

#include "Server.h"
extern IServer*    g_pServer;

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;



extern bool g_bBlendingIn;
extern bool g_bBlendingOut;

CGameApplication::CGameApplication()
{
	//m_dwState = GAME_STATE_BATTLE;
	//m_dwState = GAME_STATE_MAIN_MENU;
}

CGameApplication::~CGameApplication()
{

}

/*void CGameApplication::SetState(DWORD dwState)
{
	m_dwState = dwState;

}*/


void CGameApplication::Menu(void)
{
	/*if (!g_pMainMenu->GetActive())
		return;*/
	
	//ShowCursor(true);


	if (!g_pGame)
	{
		g_pMainMenu->Update();
		// last par is true to force render into framebuffer (and ignore the fact that we might have post processing on)
		//g_pRendererBack->DeclareNonPPCamera();
		g_pRendererBack->StartCamera(PRE_PROCESSING_FINAL_DIDNT_FIND_PP,0.f,0.f,0.f,1.0f);	
		g_pMainMenu->Render();
		g_pRendererBack->EndCamera();

		// debug
		//g_pMainMenu->Action(MENU_ACTION_GOTO_LOCAL_GAME_SCREEN);
	}


	g_pGraphicsAPI->BeginScene();
	g_TextMessages.DrawMessages();
	g_pGraphicsAPI->EndScene();


}

void CGameApplication::Game(void)
{
	if (!g_pGame)
		return;

	if (!g_pServer)
		return;

/*	if (!g_pServer->IsHandShakeDone())
		return;*/

	/*if (g_pServer->m_dwGameType!=SERVER_GAME_TYPE_LOCAL)
		if (g_pServer->m_dwConnectStage!=CONNECTION_IN_GAME)
			return;*/

	g_pServer->Think();

	static CProfileStep EntireFrameStep("Frame:");	
	EntireFrameStep.Begin();

	
	// remember to restore this!
//	g_TagLogics.PerfromReadParse();


	//g_pTimer->SetFakeSlowMotion(1.f);

	if(st_Input->KeyPressed(DIK_LSHIFT))
		g_pTimer->SetFakeSlowMotion(0.2f);
		//g_pTimer->SetFakeSlowMotion(0.003f);
	else
		g_pTimer->SetFakeSlowMotion(1.f);

	
	


	g_pGame->UpdateLogics();


	

	//TODO: this part should be in the rendererfrontend

	
	
			
	g_pGame->Render();
	
	

		

	EntireFrameStep.End();






	

	




	
	// copy camera pos/angles/lookat vector into clipboard
	if(st_Input->KeyPressed(DIK_F5))
	{
		char temp[200];

		sprintf(temp,"(%.2ff,%.2ff,%.2ff) (%.2ff,%.2ff,%.2ff) (%.2ff,%.2ff,%.2ff)",
			g_Camera_FirstPerson.GetPosition()->v[0],
			g_Camera_FirstPerson.GetPosition()->v[1],
			g_Camera_FirstPerson.GetPosition()->v[2],
			g_Camera_FirstPerson.GetLookAt().v[0],
			g_Camera_FirstPerson.GetLookAt().v[1],
			g_Camera_FirstPerson.GetLookAt().v[2],
			g_Camera_FirstPerson.GetXAng(),
			g_Camera_FirstPerson.GetYAng(),
			g_Camera_FirstPerson.GetZAng());

		CClipBoard::Copy(temp);
	}

	if(st_Input->KeyPressed(DIK_F6))
	{
		char temp[200];

		sprintf(temp,"offsetX:%f, offsetY:%f",
			g_pAutoUniforms->Get("tweakable0")->m_pfValue[0],
			g_pAutoUniforms->Get("tweakable1")->m_pfValue[0]);
			

		CClipBoard::Copy(temp);
	}

	

}

void CGameApplication::Connecting(void)
{
	if (!g_pServer)
		return;
	

	g_pServer->SendPendingMessages_TCP();
	g_pServer->RecievePendingMessages_TCP();



	if ( (SERVER_GAME_TYPE_NETWORKED_SERVER_AND_CLIENT == g_pServer->m_dwGameType) ||
		 (SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT == g_pServer->m_dwGameType) )

	{
		if (CONNECTION_NOT_INIT != g_pServer->m_dwConnectStage)
		{
			g_pServer->HandShake();
		}

	} else if ( SERVER_GAME_TYPE_LOCAL == g_pServer->m_dwGameType)
	{
		if (CONNECTION_HANDSHAKING == g_pServer->m_dwConnectStage)
		{
			g_pServer->HandShake();
		}
	}

	/*if (g_pServer->m_dwConnectStage == CONNECTION_IN_GAME)
		return;

	if (g_pServer->m_dwConnectStage == CONNECTION_HANDSHAKING)
	{
		g_pServer->HandShake();
	}*/
}

void  CGameApplication::CombatNetworking(void)
{
	g_pServer->SendPendingMessages_TCP();
	g_pServer->RecievePendingMessages_TCP();

	g_pServer->SendPendingMessages_UDP();
	g_pServer->RecievePendingMessages_UDP();
}



void CGameApplication::Frame(void)
{	
	//TODO: this part should be in the logics

	g_pTimer->Update();
	st_Input->Update();

	assert(_CrtCheckMemory());
	tUniform* time = g_pAutoUniforms->Get("time");
	assert(_CrtCheckMemory());
	if (time)
		//time->m_pfValue[0] = g_pTimer->GetTimeSecondsFAKE()*0.3;
		time->m_pfValue[0] = g_pTimer->GetTimeSeconds();

	g_TextMessages.UpdateMessages();

	g_pGraphicsAPI->BeginFrame();
		
	Game();
	
	Menu();
	
	g_pGraphicsAPI->UnBindVertexShader();
	g_pGraphicsAPI->UnBindFragmentShader();


	if (g_pServer && (CONNECTION_NOT_INIT != g_pServer->m_dwConnectStage))
	{
		Connecting();
	}

	if (g_pServer && (CONNECTION_IN_GAME == g_pServer->m_dwConnectStage))
	{
		CombatNetworking();
	}

	/*if (g_pServer && (g_pServer->m_dwConnectStage == CONNECTION_IN_GAME))
		CombatNetworking();
	else
		Connecting();*/
	

	g_pGraphicsAPI->Present();	


	// update window title

	char Title[400];
	char Title2[400];
	char Title3[400];
	char API[30];
	if (g_pGraphicsAPI->GetHandness()==API_LEFT_HANDED)
		sprintf(API,"Direct3D");
	else
		sprintf(API,"OpenGL");

	sprintf(Title,"API: %s,FPS: %f",API,g_pTimer->GetFPS());

	//if (rand()%5==0)
	g_Window.SetWindowText(Title);
	
}