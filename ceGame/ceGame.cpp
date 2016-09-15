// ceGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ceGame.h"

#include "ceLibrary.h"

#define MAX_LOADSTRING 100

#include "Window3D.h"
CWindow3D g_Window;

#include "ShaderParser.h"
#include "Q3A_ShaderManager.h"

//#include "ceLibraries.h"

#include "stdio.h"

#include "GameApplication.h"

#include "EngineOptions.h"
IEngineOptions* g_pEngineOptions;

// debug debug debug
bool g_bBlendingIn = false;
bool g_bBlendingOut = false;

bool g_bDeadReckoning = true;

struct tEngineExports
{
	 class ILogFile*                pLogFile;
	 class IGraphicsAPI_Export*     pGraphicsAPI;
	 class IRendererBackend_Export* pRendererBack;
	 class CShaderUniforms*         pAutoUniforms; 
	 class ITimer*                  pTimer;
	 class IProfiler*               pProfiler;
	 IEngineOptions*                pEngineOptions; 
};


#include "ce.h"

#include "LogFile_I.h"
ILogFile* g_pLogFile;

#include "GraphicsAPI_IE.h"
IGraphicsAPI_Export* g_pGraphicsAPI=NULL;

#include "Frustum.h"
CFrustum g_Frustum;

#include "RendererBackend_IE.h"
IRendererBackend_Export* g_pRendererBack=NULL;

#include "ShaderUniforms.h"
CShaderUniforms* g_pAutoUniforms=NULL;


#include "StaticModel.h"
#include "StaticModelManager.h"
#include "ParticleSystemManager.h"
#include "MD5Manager.h"
#include "Character.h"


typedef tEngineExports* (WINAPI*GETENGINEEXPORTS)();
GETENGINEEXPORTS GetEngineExports;

typedef void* (WINAPI*ENGINESHUTDOWN)();
ENGINESHUTDOWN EngineShutdown;

#include "Game.h"
CGame* g_pGame = NULL;

#include "Q3AMap.h"
CQuake3BSP* g_pQ3Map = NULL;

#include "RendererFrontEnd.h"
CRendererFrontEnd* g_pRendererFront;

int g_idbgEntitiesDrawnNum = 0;
int g_idbgEntitiesOnlyNecessaryRMUpdate = 0;

int g_idbgJumppadAddForceCount = 0;


/*float        g_fDEBUGRotJointX=0.f;
float        g_fDEBUGRotJointY=0.f;
float        g_fDEBUGRotJointZ=0.f;
int          g_iDEBUGJoint=24;
float        g_fDebugJoint=24.f;*/

#include "Camera.h"
CCamera g_Camera_FirstPerson;
CCamera g_Camera_FlashLight;
CCamera g_Camera_FreeFly;




#include "Timer_IE.h"
ITimer* g_pTimer;

#include "Profiler.h"
IProfiler* g_pProfiler;

#include "Input.h"

#include "TagLogics.h"
CTagLogics g_TagLogics;

#include "Buttons.h"
CButtons g_Buttons;

#include "TextMessages.h"
CTextMessages g_TextMessages;

#include "Server_Local.h"
IServer*    g_pServer = NULL;

#include "MainMenu.h"
CMainMenu* g_pMainMenu = NULL;


#include "SoundManager.h"

#include "SoundAPI.h"
CSoundAPI g_SoundAPI;




// debug
//float g_


bool g_bPostProcessing = true;
//bool g_bPostProcessing = false;


bool g_bRenderableRequirePostProcessing=false;

CGameApplication* g_pGameApplication = NULL;



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{



	// debug

	/*CRingBuffer ring;

	char temp[100];
	

	char* p1 = NULL;

	int iLength;
	int iLength2;

	char buffer[10];

	for (int i=0;i<10;i++)
		buffer[i] = 30+i;

	char buffer2[15];

	for (int i=0;i<15;i++)
		buffer2[i] = i;

	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);
	ring.Push(buffer,10);

	ring.InformDataSent(90);


	ring.GetCurrSend(p1,iLength);

	ring.Push(buffer2,15);

	ring.GetCurrSend(p1,iLength);*/



	


	//// debug debug

	/*char blah[1000];
	sprintf(blah,"%d , %c , %s",0x11, 0x3F , "test");*/

	HMODULE module;

	module = LoadLibrary("ceEngine.dll");

	DWORD dwError = GetLastError();

	/*char pPath[400];
	GetCurrentDirectory(400,pPath);*/

	GetEngineExports =  (GETENGINEEXPORTS)GetProcAddress(module,"GetEngineExports");

	EngineShutdown   =   (ENGINESHUTDOWN) GetProcAddress(module,"EngineShutdown");



	tEngineExports* pEngineExports  = GetEngineExports();



	g_pLogFile = pEngineExports->pLogFile;
	g_pGraphicsAPI = pEngineExports->pGraphicsAPI;
	g_pRendererBack = pEngineExports->pRendererBack;
	g_pAutoUniforms = pEngineExports->pAutoUniforms;
	g_pTimer = pEngineExports->pTimer;
	g_pProfiler = pEngineExports->pProfiler;
	g_pEngineOptions = pEngineExports->pEngineOptions;

	//// debug debug


	tWinProperties windowProps;
	windowProps.iXPos = 0;
	windowProps.iYPos = 0;


	windowProps.iClientWidth  = g_pEngineOptions->GetScreenWidth();
	windowProps.iClientHeight = g_pEngineOptions->GetScreenHeight();



	windowProps.pCaption = new char[50];
	//sprintf(windowProps.pCaption,"Crimson Engine");
	sprintf_s(windowProps.pCaption,50,"Crimson Engine");

	windowProps.pClassName = new char[50];
	sprintf_s(windowProps.pClassName,50,"Crimson Engine");

	
	g_Window.Create(hInstance, windowProps );
	g_Window.SetWindowText("Crimson Engine");

	//CGameApplication app;
	g_pGameApplication = new CGameApplication();
	

	g_pGameApplication->Init(g_Window.GetHWND());


	// from here

	


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Direct Input...",LOG_MESSAGE_INFORMATIVE);
	CInput::CreateInstance();
	st_Input->Init((unsigned long)windowProps.iClientWidth,(unsigned long)windowProps.iClientHeight,g_Window.GetHWND());
	st_Input->SetCursorPosition((unsigned long)windowProps.iClientWidth / 2,(unsigned long)windowProps.iClientHeight / 2);
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Direct Input Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


	st_Input->UnAquireKBandMouse();

	if (!g_pGraphicsAPI->Init(windowProps.iClientWidth, windowProps.iClientHeight, g_Window.GetHWND()))
	{
		CR_ERROR(1,"Failed to init Graphics API!");
		return false;
	}



	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Particle System Manager...",LOG_MESSAGE_INFORMATIVE);
	CParticleSystemManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Particle System Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Static Model Manager...",LOG_MESSAGE_INFORMATIVE);
	CStaticModelManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Static Model Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);

	g_pLogFile->OutPutPlainText("CEngine::Init::Initing MD5 Manager...",LOG_MESSAGE_INFORMATIVE);
	CMD5Manager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of MD5 Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);
	


/*
	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Timer...",LOG_MESSAGE_INFORMATIVE);
	ITimer::CreateInstance();
	g_pTimer->TimerInit();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Timer Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);*/

	

	///////////////////////
	// Init USB Device
/*
	g_pLogFile->OutPutPlainText("CEngine::Init::Initing USB Input Device...",LOG_MESSAGE_INFORMATIVE);

	int iPort = g_TagLogics.InitSerialCommunication();

	if (iPort==-1)
	{
		g_pLogFile->OutPutPlainText("CEngine::Init::Initing USB Input Device Failed!",LOG_MESSAGE_ERROR);
		return false;
	}
	
	char comInitedCorrectlyMSG[100];
	sprintf(comInitedCorrectlyMSG,"CEngine::Init::USB Input Initialization done. Using Port: %u",iPort);
	g_pLogFile->OutPutPlainText(comInitedCorrectlyMSG,"4","009900",true,LOG_MESSAGE_INFORMATIVE);
*/
	

	g_TagLogics.ResetTags();
	////////////////////////////////////

	g_pLogFile->OutPutPlainText("CEngine::Init::Setting Config...",LOG_MESSAGE_INFORMATIVE);
	g_Buttons.SetConfig("Tags.txt");
	g_pLogFile->OutPutPlainText("////CEngine::Init::Setting Config Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);



	



	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Renderer-FrontEnd...",LOG_MESSAGE_INFORMATIVE);	
	g_pRendererFront = new CRendererFrontEnd();
	if (g_pEngineOptions->GetFullFallback())
		g_pRendererFront->SetRoughFallBack(true);
	else
		g_pRendererFront->SetRoughFallBack(false);
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Renderer-FrontEnd Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing FMod based Sound API",LOG_MESSAGE_INFORMATIVE);	
	bool bRes = g_SoundAPI.Init();

	if (!bRes)
		g_pLogFile->OutPutPlainText("CEngine::Init::Initing Failed to init FMod based Sound API!",LOG_MESSAGE_INFORMATIVE);	
	else
		g_pLogFile->OutPutPlainText("CEngine::Init::FMod based Sound API initialization done.",LOG_MESSAGE_INFORMATIVE);	


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Sound Manager...",LOG_MESSAGE_INFORMATIVE);
	CSoundManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Sound Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);



	// create GUI menus

	g_pMainMenu = new CMainMenu();
	g_pMainMenu->Init();
	

	// debug
	/*float fTest = 0.f;
	for (int i=0;i<1000000;i++)
		fTest+=0.1f;

	char temp[400];
	sprintf(temp,"Result = %f",fTest);

	g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_INFORMATIVE);*/

	/////////////////////////////////////////////////////
	// Init camera

	//(477.28f,923.44f,292.39f) (-0.10f,-0.87f,0.47f) (-60.90f,-192.40f,0.00f)

	g_Camera_FirstPerson.SetPosition(477.28f,923.44f,292.39f);
	g_Camera_FirstPerson.SetAngles(-60.90f,-192.40f,0.00f);
	

	



	// q3dm17 map
	//(106.38f,170.52f,33.58f) (-0.66f,-0.60f,0.45f) (-36.70f,-235.90f,0.00f)
	//g_Camera_FlashLight.SetPosition(106.38f,170.52f,33.58f);
	//g_Camera_FlashLight.SetAngles (-36.70f,-235.90f,0.00f);
	
	//tutorial map
	//(331.71f,277.08f,-872.02f) (-0.21f,-0.34f,-0.92f) (-19.80f,-346.90f,0.00f)

	//(165.38f,1146.83f,-55.41f) (-0.04f,-1.00f,0.00f) (-87.80f,-269.40f,0.00f)

	
	g_Camera_FlashLight.SetPosition(165.38f,1146.83f,-55.41f);
	g_Camera_FlashLight.SetAngles(-87.80f,-269.40f,0.00f);

	//(333.93f,532.00f,-874.00f) (0.48f,-0.37f,0.79f) (-22.00f,211.40f,0.00f)

	/*g_Camera_FlashLight.SetPosition(333.93f,532.00f,-874.00f);
	g_Camera_FlashLight.SetAngles(-22.00f,211.40f,0.00f);*/

	float fDeltaTime = g_pTimer->GetTime();

	g_Camera_FirstPerson.UpdatePositionAndMatrices(fDeltaTime);
	g_Camera_FlashLight.UpdatePositionAndMatrices(fDeltaTime);
	g_pGraphicsAPI->SetViewMatrix(g_Camera_FirstPerson.ReturnCameraMatrixfa());



	//////////////////////////////////
	// Q3A Shader Scripts

	CShaderParser::CreateInstance();
	st_ShaderParser->BuildFileList("scripts/","*.shader");
	st_ShaderParser->BuildCacheMap();


	CQ3A_ShaderManager::CreateInstance();
	//st_Q3A_ShaderManager->

	
	////////////////////////////////////
	// get into menu screen




	st_Input->AquireKBandMouse();


	////////////////////////////////////////////////////////////////////////////
	// Main Game Loop
	g_pGameApplication->MessageLoop();
	////////////////////////////////////////////////////////////////////////////


	SafeDelete(g_pGame);
	SafeDelete(g_pQ3Map);

	EngineShutdown();
	
	return 1;
	
}


