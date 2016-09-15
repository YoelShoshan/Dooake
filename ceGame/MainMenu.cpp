#include "stdafx.h"
#include "MainMenu.h"

#include "ce.h"

#include <assert.h>

#include "guiStaticImage.h"
#include "guiButton.h"
#include "guiEditBox.h"

#include "Input.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Game.h"
extern CGame* g_pGame;

#include "Server.h"
#include "Server_Local.h"
#include "Server_Networking_Master.h"
#include "Server_Networking_Client.h"
#include "Server_Local.h"
extern IServer*    g_pServer;

#include "GameApplication.h"
extern CGameApplication* g_pGameApplication;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <sys/stat.h> 
#include <direct.h>


string GetCurrentPath() 
{
	char cCurrentPath[FILENAME_MAX];

	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		return "";
	}

	string ans = cCurrentPath;
	return ans;
}



CMainMenu::CMainMenu()
{
	m_pActiveScreen = NULL;
	m_dwGameState   = MENU_GAME_STATE_NO_GAME;
	m_bActive = true;
	m_pMenuMusic = NULL;
}


CMainMenu::~CMainMenu()
{
	/*if (m_pMenuMusic)
	{
		g_SoundAPI.FreeStream(m_pMenuMusic);
		SafeDelete(m_pMenuMusic);
	}*/

}

void CMainMenu::Init(void)
{

	// start music
	g_SoundAPI.LoadStream("Music/menu.mp3",m_pMenuMusic);
	g_SoundAPI.PlayStream(m_pMenuMusic,150);

	//g_SoundManager.PlayLoopedMusic("Music/menu.mp3",m_pMenuMusic,0);


	// add cursor
	m_spMat_Cursor = g_pRendererBack->LoadMaterial("!GUI/cursor2.tga");
	m_spMat_Cross = g_pRendererBack->LoadMaterial("!GUI/crosshair1.tga");

	////////////////////////////////////////////////////////////////////////////////////////
	// Add first menu
	CMenuScreen* pFirstScreen = new CMenuScreen("FIRST_SCREEN");
	AddScreen(std::string("FIRST_SCREEN"),pFirstScreen);

	// TITLE static image
	CGuiStaticImage* pTitle = new CGuiStaticImage();
	pTitle->SetLocation(25.f,80.f,50.f,20.f);
	//pTitle->SetLocation(0.f,0.f,1024.f,768.f);
	pTitle->Load("!GUI/dooake1.jpg");
	pTitle->SetParent(pFirstScreen);
	std::string strTitle("FIRST_MENU_ITEM_TITLE");
	pFirstScreen->AddItem(pTitle,strTitle);

	// HOST GAME button
	CGuiButton* pHostButton = new CGuiButton();
	pHostButton->SetLocation(37.5f,65.f,27.1484375f,9.375f);
	pHostButton->Load(MENU_ACTION_GOTO_HOST_GAME_SCREEN,"!GUI/HostGameON.tga","!GUI/HostGameOFF.tga","!GUI/HostGameOFF.tga");
	pHostButton->SetParent(pFirstScreen);
	strTitle = "ITEM_HOST_GAME_BUTTON";
	pFirstScreen->AddItem(pHostButton,strTitle);

	
	// JOIN GAME button
	CGuiButton* pJoinButton = new CGuiButton();
	pJoinButton->SetLocation(37.5f,50.f,27.1484375f,9.375f);
	pJoinButton->Load(MENU_ACTION_GOTO_JOIN_GAME_SCREEN,"!GUI/JoinGameON.tga","!GUI/JoinGameOFF.tga","!GUI/JoinGameOFF.tga");
	pJoinButton->SetParent(pFirstScreen);
	strTitle = "ITEM_JOIN_GAME_BUTTON";
	pFirstScreen->AddItem(pJoinButton,strTitle);


	// LOCAL GAME button
	CGuiButton* pLocalButton = new CGuiButton();
	pLocalButton->SetLocation(37.5f,35.f,27.1484375f,9.375f);
	pLocalButton->Load(MENU_ACTION_GOTO_LOCAL_GAME_SCREEN,"!GUI/LocalGameON.tga","!GUI/LocalGameOFF.tga","!GUI/LOcalGameOFF.tga");
	pLocalButton->SetParent(pFirstScreen);
	strTitle = "ITEM_LOCAL_GAME_BUTTON";
	pFirstScreen->AddItem(pLocalButton,strTitle);


	// QUIT button
	CGuiButton* pQuit = new CGuiButton();
	//pQuit->SetLocation(37.5f,40.f,11.328125f,0.0520833f);
	pQuit->SetLocation(45.f,20.f,11.328125f,5.20833f);
	pQuit->Load(MENU_ACTION_QUIT,"!GUI/quitDOWN.tga","!GUI/quitHOVER.tga","!GUI/quitUP.tga");	
	pQuit->SetParent(pFirstScreen);
	strTitle = "ITEM_QUIT_BUTTON";
	pFirstScreen->AddItem(pQuit,strTitle);



	// END GAME button
	CGuiButton* pEndGameButton = new CGuiButton();
	pEndGameButton->SetLocation(5.f,35.f,27.1484375f,9.375f);
	pEndGameButton->Load(MENU_ACTION_END_GAME,"!GUI/EndGame.tga","!GUI/EndGame.tga","!GUI/EndGame.tga");
	pEndGameButton->SetParent(pFirstScreen);
	pEndGameButton->SetActive(false);
	pEndGameButton->SetRender(false);
	strTitle = "ITEM_END_GAME_BUTTON";
	pFirstScreen->AddItem(pEndGameButton,strTitle);



	////////////////////////////////////////////////////////////////////////////////////////
	// Add host game menu
	CMenuScreen* pHostGameScreen = new CMenuScreen("HOST_GAME_SCREEN");
	AddScreen(std::string("HOST_GAME_SCREEN"),pHostGameScreen);

	// TITLE static image
	CGuiStaticImage* pTitleHost = new CGuiStaticImage();
	pTitleHost->SetLocation(37.5f,80.f,27.1484375f,9.375f);
	//pTitle->SetLocation(0.f,0.f,1024.f,768.f);
	pTitleHost->Load("!GUI/HostMenuHead.tga");
	pTitleHost->SetParent(pHostGameScreen);
	strTitle = "HOST_GAME_ITEM_TITLE";
	pHostGameScreen->AddItem(pTitleHost,strTitle);




	// Start! button
	CGuiButton* pStopHostGameButton = new CGuiButton();
	pStopHostGameButton->SetLocation(37.5f,10.f,27.1484375f,9.375f);
	pStopHostGameButton->Load(MENU_ACTION_HOST_START_GAME,"!GUI/Start!.tga","!GUI/Start!-hover.tga","!GUI/Start!.tga");
	pStopHostGameButton->SetParent(pHostGameScreen);
	/*pStartHostGameButton->SetActive(true);
	pStartHostGameButton->SetRender(false);*/
	strTitle = "ITEM_START_HOST_GAME_BUTTON";
	pHostGameScreen->AddItem(pStopHostGameButton,strTitle);

	
	pHostGameScreen->SetParentName("FIRST_SCREEN");
	/////////////////////////////////////////////////////////////////////////////////////////


	// Stop Hosting! button
	CGuiButton* pStartHostGameButton = new CGuiButton();
	pStartHostGameButton->SetLocation(10.f,45.f,27.1484375f,9.375f);
	pStartHostGameButton->Load(MENU_ACTION_STOP_HOSTING,"!GUI/Stop-host!.tga","!GUI/Stop-host!-hover.tga","!GUI/Stop-host!.tga");
	pStartHostGameButton->SetParent(pHostGameScreen);
	/*pStartHostGameButton->SetActive(true);
	pStartHostGameButton->SetRender(false);*/
	strTitle = "ITEM_STOP_HOST_GAME_BUTTON";
	pHostGameScreen->AddItem(pStartHostGameButton,strTitle);


	// Re-Host! button
	CGuiButton* pReHostButton = new CGuiButton();
	pReHostButton->SetLocation(60.f,45.f,27.1484375f,9.375f);
	pReHostButton->Load(MENU_ACTION_RE_HOST,"!GUI/re-host!.tga","!GUI/re-host!-hover.tga","!GUI/re-host!.tga");
	pReHostButton->SetParent(pHostGameScreen);
	/*pStartHostGameButton->SetActive(true);
	pStartHostGameButton->SetRender(false);*/
	strTitle = "ITEM_RE_HOST_BUTTON";
	pHostGameScreen->AddItem(pReHostButton,strTitle);
	
	pHostGameScreen->SetParentName("FIRST_SCREEN");
	/////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Add join game menu
	CMenuScreen* pJoinGameScreen = new CMenuScreen("JOIN_GAME_SCREEN");
	AddScreen(std::string("JOIN_GAME_SCREEN"),pJoinGameScreen);

	/*// TITLE static image
	CGuiStaticImage* pTitleJoin = new CGuiStaticImage();
	pTitleJoin->SetLocation(37.5f,80.f,27.1484375f,9.375f);
	pTitleJoin->Load("!GUI/JoinGameON.tga");
	strTitle = "JOIN_GAME_ITEM_TITLE";
	pJoinGameScreen->AddItem(pTitleJoin,strTitle);*/

	// PleaseTypeServerIP static image
	CGuiStaticImage* pType = new CGuiStaticImage();
	pType->SetLocation(10.f,50.f,27.1484375f,9.375f);
	pType->Load("!GUI/PleaseTypeServerIP.tga");
	pType->SetParent(pJoinGameScreen);
	strTitle = "PLEASE_TYPE_SERVER_IP";
	pJoinGameScreen->AddItem(pType,strTitle);

	// IP edit box
	CGuiEditBox* pIPEditBox = new CGuiEditBox();
	pIPEditBox->SetLocation(50.f,50.f,27.1484375f,9.375f);
	pIPEditBox->Load(EDIT_BOX_TYPE_IP_ADDRESS,"!GUI/ChatMenu.tga","Materials/OnFocusEditBox.mat",-1);
	pIPEditBox->SetParent(pJoinGameScreen);
	//pIPEditBox->SetFocus(false);
	strTitle = "TYPE_IP_EDIT_BOX";
	pJoinGameScreen->AddItem(pIPEditBox,strTitle);


	char server_ip_str[256];
	DWORD dwStrSize = 0;

	string ini_file_path = GetCurrentPath()+string("\\ceConfig.ini");

	struct stat stFileInfo;
	bool blnReturn;
	int intStat; 
	intStat = stat(ini_file_path.c_str(),&stFileInfo); 

	IGuiItem* pEditBox = pJoinGameScreen->GetItem(std::string("TYPE_IP_EDIT_BOX"));
	if (pEditBox)
	{		
		/*if (intStat!=0) //file doesn't exist
		{
			// If the file doesn't exist, let's create it and set default values into it.

			ofstream myfile;
			myfile.open ("ceConfig.ini");
			myfile << ";Dooake Configuration File\n";
			myfile.close();
		}*/

			

		if (intStat==0)
		{
			DWORD dwSize = GetPrivateProfileString("CONNECTION_SETTINGS","SERVER_IP",NULL,server_ip_str,255,ini_file_path.c_str());
			if (dwSize>0)
			{
				pIPEditBox->SetText(server_ip_str);
			}
		}
	

	}


	// PleaseTypePlayerName static image
	CGuiStaticImage* pTypePlayerName = new CGuiStaticImage();
	pTypePlayerName->SetLocation(10.f,70.f,27.1484375f,9.375f);
	pTypePlayerName->Load("!GUI/PleaseTypePlayerName.tga");
	pTypePlayerName->SetParent(pJoinGameScreen);
	strTitle = "PLEASE_TYPE_PLAYER_NAME";
	pJoinGameScreen->AddItem(pTypePlayerName,strTitle);


	// Player Name edit box
	CGuiEditBox* pPlayerNameEditBox = new CGuiEditBox();
	pPlayerNameEditBox->SetLocation(50.f,70.f,27.1484375f,9.375f);
	pPlayerNameEditBox->Load(EDIT_BOX_TYPE_FREE_TEXT_NO_NEWLINES,"!GUI/ChatMenu.tga","Materials/OnFocusEditBox.mat",28);
	pPlayerNameEditBox->SetParent(pJoinGameScreen);
	//pPlayerNameEditBox->SetActive(false);
	strTitle = "TYPE_PLAYER_NAME_EDIT_BOX";
	pJoinGameScreen->AddItem(pPlayerNameEditBox,strTitle);



	// JOIN GAME button
	CGuiButton* pJoinServerButton = new CGuiButton();
	pJoinServerButton->SetLocation(37.5f,25.f,27.1484375f,9.375f);
	pJoinServerButton->Load(MENU_ACTION_JOIN_GAME,"!GUI/JoinGameON.tga","!GUI/JoinGameOFF.tga","!GUI/JoinGameOFF.tga");
	pJoinServerButton->SetParent(pJoinGameScreen);
	strTitle = "ITEM_JOIN_GAME_NOW_BUTTON";
	pJoinGameScreen->AddItem(pJoinServerButton,strTitle);
	
	pJoinGameScreen->SetParentName("FIRST_SCREEN");
	/////////////////////////////////////////////////////////////////////////////////////////










	///////////////////////////////////////////////////////////////////////////////

	SetActiveScreen(std::string("FIRST_SCREEN"));



	// init chat edit box

	 //Load(DWORD dwType,const char *pchMatBackground,const char *pchMatBackgroundFocus,int iLimit);
	m_ChatEditBox.Load(EDIT_BOX_TYPE_FREE_TEXT_NO_NEWLINES,"!GUI/ChatMenu.tga","Materials/OnFocusEditBox.mat",50);
	m_ChatEditBox.SetLocation(0.f,60.f,60.f,10.f);
	m_ChatEditBox.SetRender(false);
	m_ChatEditBox.SetFocus(false);
	m_ChatEditBox.SetActive(false);


	//pPlayerNameEditBox->SetActive(false);
}

void CMainMenu::AddScreen(std::string strScreenName,CMenuScreen* pScreen)
{
	m_Screens[strScreenName] = pScreen;
}

void CMainMenu::Update(void)
{

	

	m_fMouseX = st_Input->GetAbsoluteX();
	m_fMouseY = st_Input->GetAbsoluteY();

	float fResX = g_pGraphicsAPI->GetResX();
	float fResY = g_pGraphicsAPI->GetResY();


	m_fMouseY = fResY - m_fMouseY;

	m_fMouseX_Perc = (m_fMouseX / fResX)*100.f;
	m_fMouseY_Perc = (m_fMouseY / fResY)*100.f;

	assert(m_pActiveScreen);

	static bool bLeftMousePressed=false;

	bool bLeftClick=false;


	if (st_Input->MouseButtonDown(DIMOUSE_LEFTBUTTON))
	{
		if (!bLeftMousePressed)
		{
			bLeftMousePressed=true;
			bLeftClick=true;		
		}
	}
	else
		bLeftMousePressed=false;

	

	if (m_ChatEditBox.m_bActive)
	{
		if(st_Input->KeyPressed(DIK_ESCAPE))
		{
			m_ChatEditBox.SetRender(false);
			m_ChatEditBox.SetFocus(false);
			m_ChatEditBox.SetActive(false);
		}
	}


	static bool bTPressed=false;
	if(st_Input->KeyPressed(DIK_T))
	{
		if (!bTPressed)
		{
			bTPressed=true;		

			if (!m_ChatEditBox.m_bActive)
			{
				m_ChatEditBox.SetRender(true);
				m_ChatEditBox.SetFocus(true);
				m_ChatEditBox.SetActive(true);
				m_ChatEditBox.s_bTPressed = true;
			}			
		}								
	}
	else
		bTPressed=false;


	static bool bEnterPressed=false;
	if(st_Input->KeyPressed(DIK_RETURN) || st_Input->KeyPressed(DIK_NUMPADENTER))
	{
		if (!bEnterPressed)
		{
			bEnterPressed=true;		

			if (m_ChatEditBox.m_bActive)
			{
				std::string message((char*)m_ChatEditBox.GetInfo());

				if (g_pServer)
				if (message.length()>0)
				{
					g_pServer->SendChatMessage(message.c_str(),g_pServer->m_cHumanClientConnectID);
					m_ChatEditBox.Clear();
				}

				m_ChatEditBox.SetRender(false);
				m_ChatEditBox.SetFocus(false);
				m_ChatEditBox.SetActive(false);
			}
						
		}								
	}
	else
		bEnterPressed=false;

	m_ChatEditBox.RunLogics(m_fMouseX_Perc,m_fMouseY_Perc,bLeftClick);

	if (!m_bActive)
		return;


	m_pActiveScreen->Update(m_fMouseX_Perc,m_fMouseY_Perc,bLeftClick);

	
}


void CMainMenu::Render(void)
{
	if (m_bActive)
		m_pActiveScreen->Render();


	m_ChatEditBox.Render();


	if (m_bActive)
		g_pRendererBack->Render2DQuad(m_spMat_Cursor,m_fMouseX,m_fMouseY,64.f,64.f);
	else
		g_pRendererBack->Render2DQuad_Perc(m_spMat_Cross,44,44,12,12);
		




}

bool CMainMenu::GetIsDeepInMenus(void)
{
	if (m_strActiveScreenName == std::string("FIRST_SCREEN"))
		return false;

	return true;

}

void CMainMenu::SetActive(bool bActive) 
{ 
	//BREAKPOINT(1);
	m_bActive = bActive;

}

CMenuScreen* CMainMenu::GetScreen(std::string& strScreenName)
{
	MENU_SCREENS_MAP_IT it = m_Screens.find(strScreenName);

	if (it==m_Screens.end())
	{
		BREAKPOINT(1) // screen not found
			return NULL;
	}

	// it was found
	return it->second;

}

void CMainMenu::SetActiveScreen(std::string& strActiveScreenName)
{

	m_pActiveScreen = GetScreen(strActiveScreenName);
	m_strActiveScreenName = strActiveScreenName;
}


bool CMainMenu::Action(DWORD dwAction)
{
	int iPlayersNum = 3;
	tConnectData pConData;

	CMenuScreen* pScreen = NULL;
	char* pTemp = NULL;



	switch (dwAction)
	{
	case MENU_ACTION_RE_HOST:
	{
		if (g_pServer)
			g_TextMessages.AddMessage("You are already hosting.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);			
		else
		{
			g_TextMessages.AddMessage("Hosting Started!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			g_TextMessages.AddMessage("Waiting for clients.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			SafeDelete(g_pServer);
			g_pServer = new CNetworking_Master();		
			g_pServer->Connect(NULL);
		}

		break;
	}	
	case MENU_ACTION_STOP_HOSTING:
		if (g_pServer)
		{
			g_TextMessages.AddMessage("Stopped Hosting.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			SafeDelete(g_pServer);
			SafeDelete(g_pGame);

		}
		else
		{
			g_TextMessages.AddMessage("You are not hosting right now.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		}
	break;
	case MENU_ACTION_END_GAME:
		SafeDelete(g_pServer);
		SafeDelete(g_pGame);

		pScreen = GetScreen(std::string("FIRST_SCREEN"));

		///////////////////////////////////////////////////////////////////////////////
		// ENABLE
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetRender(true);

		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetRender(true);

		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetRender(true);

		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetRender(true);

		///////////////////////////////////////////////////////////////////////////////
		// DISABLE	
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetRender(false);

		g_SoundAPI.PlayStream(m_pMenuMusic,150);
		
		break;
	case MENU_ACTION_CONNECTION_ABORTED:
		pScreen = GetScreen(std::string("JOIN_GAME_SCREEN"));
		pScreen->SetEscapeActive(true);
		break;
	case MENU_ACTION_HOST_START_GAME:

		if (!g_pServer)
		{
			g_TextMessages.AddMessage("Can't start game:",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			g_TextMessages.AddMessage("You are not hosting right now.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			break;
		}

		// stop menu music
		if (m_pMenuMusic)
			g_SoundAPI.StopStream(m_pMenuMusic);
		
		//g_SoundManager.PlayStream(m_pMenuMusic);

		// here i should start the game
		m_bActive = false;
		SetActiveScreen(std::string("FIRST_SCREEN"));
		g_pServer->OnEvent(INFORM_START_GAME,NULL);

		// temp try

		pScreen = GetScreen(std::string("FIRST_SCREEN"));

		///////////////////////////////////////////////////////////////////////////////
		// DISABLE
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetRender(false);

		///////////////////////////////////////////////////////////////////////////////
		// ENABLE
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetRender(true);

		break;
	case MENU_ACTION_GOTO_LOCAL_GAME_SCREEN:

		/*if (g_pServer)
			if (g_pServer->m_dwConnectStage!=CONNECTION_NOT_INIT)
			{
				g_TextMessages.AddMessage("You are already connected!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				return;				

			}*/

		m_bActive = false;

		// stop menu music
		if (m_pMenuMusic)
			g_SoundAPI.StopStream(m_pMenuMusic);


		SetActiveScreen(std::string("FIRST_SCREEN"));
		pScreen = GetScreen(std::string("FIRST_SCREEN"));

		///////////////////////////////////////////////////////////////////////////////
		// DISABLE
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetRender(false);

		///////////////////////////////////////////////////////////////////////////////
		// ENABLE
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetRender(true);

		


		st_Input->UnAquireKBandMouse();

		///////////////////////////////////////////////////////////////////////////
		// init server

		g_TextMessages.AddMessage("Hosting Started!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		g_TextMessages.AddMessage("Waiting for clients.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

		SafeDelete(g_pServer);
		g_pServer = new CServer_Local();		
		g_pServer->Connect(NULL);
			

		


		//g_pGameApplication->SetState(GAME_STATE_BATTLE);

		

		st_Input->AquireKBandMouse();

		break;
	case MENU_ACTION_GOTO_HOST_GAME_SCREEN:
		/*if (g_pServer)
			if (g_pServer->m_dwConnectStage!=CONNECTION_NOT_INIT)
			{
				g_TextMessages.AddMessage("You are already hosting!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
				return;

			}*/

		SetActiveScreen(std::string("HOST_GAME_SCREEN"));

		pScreen = GetScreen(std::string("HOST_GAME_SCREEN"));

		//pScreen->SetEscapeActive(false);

		if (!g_pServer)
		{
			g_pServer = new CNetworking_Master();		
			g_pServer->Connect(NULL);
		}
		

		break;
	case MENU_ACTION_GOTO_JOIN_GAME_SCREEN:
		SetActiveScreen(std::string("JOIN_GAME_SCREEN"));

		

		break;

	case MENU_ACTION_PURE_CLIENT_GAME_STARTED:
		SetActiveScreen(std::string("FIRST_SCREEN"));
		pScreen = GetScreen(std::string("FIRST_SCREEN"));

		///////////////////////////////////////////////////////////////////////////////
		// DISABLE
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_HOST_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_JOIN_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_LOCAL_GAME_BUTTON"))->SetRender(false);

		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetActive(false);
		pScreen->GetItem(std::string("ITEM_QUIT_BUTTON"))->SetRender(false);

		///////////////////////////////////////////////////////////////////////////////
		// ENABLE
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetActive(true);
		pScreen->GetItem(std::string("ITEM_END_GAME_BUTTON"))->SetRender(true);

		// stop menu music
		if (m_pMenuMusic)
			g_SoundAPI.StopStream(m_pMenuMusic);

		break;
	case MENU_ACTION_JOIN_GAME:
		{

			if (g_pServer)
				if (g_pServer->m_dwConnectStage!=CONNECTION_NOT_INIT)
				{
					g_TextMessages.AddMessage("You are already connected!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
					return true;

				}

			SafeDelete(g_pServer);
			g_pServer = new CNetworking_Client();		


			
			// here i need to retrieve the ip from the text box

			pScreen = GetScreen(std::string("JOIN_GAME_SCREEN"));

			pScreen->SetEscapeActive(false);
			
			// it was found

			pTemp = (char*) pScreen->GetItem(std::string("TYPE_IP_EDIT_BOX"))->GetInfo();
			strcpy(pConData.ipHostAdress,pTemp);


			// save to ini file if found

			string ini_file_path = GetCurrentPath()+string("\\ceConfig.ini");
			WritePrivateProfileString("CONNECTION_SETTINGS","SERVER_IP",pConData.ipHostAdress,ini_file_path.c_str());



			pTemp = (char*) pScreen->GetItem(std::string("TYPE_PLAYER_NAME_EDIT_BOX"))->GetInfo();
			SafeDeleteArray(pConData.pcPlayerName);
			pConData.pcPlayerName = new char[strlen(((char*)pTemp))+1];
			strcpy(pConData.pcPlayerName,pTemp);
					

			//strcpy(pConData.ipHostAdress,"127.0.0.1");
			//strcpy(pConData.ipHostAdress,"84.174.120.162");
			//strcpy(pConData.ipHostAdress,"85.65.213.5");		 

			g_pServer->Connect(&pConData);
		}
		break;
	case MENU_ACTION_QUIT:
		ExitProcess(0);
		//PostQuitMessage(0);
		return true;
		break;
	default:
		BREAKPOINT(1); // unrecognized action
		break;

	};

	return true;
}
