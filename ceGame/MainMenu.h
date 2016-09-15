#ifndef CRIMSON_MAIN_MENU_H
#define CRIMSON_MAIN_MENU_H

#pragma once

#include "MenuScreen.h"

#include <map>
#include <string>

#include "SmartPointer.h"
#include "Material_IE.h"

#include "guiEditBox.h"

#include "ce.h"

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;

typedef std::map<const std::string,CMenuScreen*> MENU_SCREENS_MAP;
typedef std::map<const std::string,CMenuScreen*>::iterator MENU_SCREENS_MAP_IT;


#define MENU_ACTION_GOTO_LOCAL_GAME_SCREEN   0
#define MENU_ACTION_GOTO_HOST_GAME_SCREEN    1
#define MENU_ACTION_GOTO_JOIN_GAME_SCREEN    2
#define MENU_ACTION_QUIT                     3
#define MENU_ACTION_JOIN_GAME                4
#define MENU_ACTION_CONNECTION_ABORTED       5
#define MENU_ACTION_END_GAME                 6
#define MENU_ACTION_HOST_START_GAME          7
#define MENU_ACTION_PURE_CLIENT_GAME_STARTED 8
#define MENU_ACTION_STOP_HOSTING             9
#define MENU_ACTION_RE_HOST                 10



#define MENU_GAME_STATE_NO_GAME               0
#define MENU_GAME_STATE_INSIDE_LOCAL_GAME     1
#define MENU_GAME_STATE_INSIDE_NETWORKED_GAME 2

class CMainMenu
{
public:
	CMainMenu();
	~CMainMenu();

	void Init(void);

	void SetActiveScreen(std::string& strActiveScreenName);

	bool GetIsDeepInMenus(void);
		
	void Update(void);

	void Render(void);

	bool Action(DWORD dwAction);

	CMenuScreen* GetScreen(std::string& strScreenName);

	void SetActive(bool bActive); 
	bool GetActive(void) const { return m_bActive;};

	bool GetChatActive(void) const { return m_ChatEditBox.m_bActive;};

private:

	void AddScreen(std::string strScreenName,CMenuScreen* pScreen);

	
	MENU_SCREENS_MAP m_Screens;
	CMenuScreen*     m_pActiveScreen;
	std::string      m_strActiveScreenName;

	CSmartPointer<IMaterial_Export> m_spMat_Cursor;
	
	CSmartPointer<IMaterial_Export> m_spMat_Cross;


	CGuiEditBox m_ChatEditBox;

	

	float m_fMouseX,m_fMouseY;
	float m_fMouseX_Perc,m_fMouseY_Perc;

	DWORD m_dwGameState;
	bool m_bActive;


	FSOUND_STREAM* m_pMenuMusic;
};

#endif