#ifndef CRIMSON_BUTTONS_H
#define CRIMSON_BUTTONS_H

#pragma once

#include <map>

#include "windows.h"

struct tButtonBind
{
	__int64 i64TagID;
	DWORD key;
};

typedef std::map<int,tButtonBind*>                     BUTTON_BINDS_MAP;
typedef std::map<int,tButtonBind*>::iterator           BUTTON_BINDS_MAP_IT;

#define BUTTON_DOWN            0
#define BUTTON_FORWARD         1
#define BUTTON_FORWARD_RIGHT   2
#define BUTTON_RIGHT           3
#define BUTTON_RIGHT_BACKWARDS 4
#define BUTTON_BACKWARDS       5
#define BUTTON_BACKWARDS_LEFT  6
#define BUTTON_LEFT            7
#define BUTTON_LEFT_FORWARD    8

class CButtons
{
public:
	CButtons();
	~CButtons();

	bool SetConfig(const char* pcFileName);

	bool Add(int iButtonNum,tButtonBind* pAddMe);

	bool IsActive(int iButtonNum);
private:

	int m_iButtonsNum;
	BUTTON_BINDS_MAP  m_ButtonBinds;

};

#endif