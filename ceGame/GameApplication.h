#ifndef CRIMSON_GAME_APPLICATION_H
#define CRIMSON_GAME_APPLICATION_H

#pragma once

#include "ceLibrary.h"

/*#define GAME_STATE_MAIN_MENU 0
#define GAME_STATE_BATTLE    1*/

class CGameApplication : public CApplication
{
public:
	CGameApplication();
	~CGameApplication();

	void Frame(void);

	//void SetState(DWORD dwState);

private:

	void Connecting(void);
	void CombatNetworking(void);
	void Game(void);
	void Menu(void);
	

	//DWORD m_dwState;
};


#endif