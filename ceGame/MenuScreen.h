#ifndef CRIMSON_MENU_SCREEN_H
#define CRIMSON_MENU_SCREEN_H

#pragma once

#include <map>
#include <string>

#include "guiItem.h"

class IGuiItem;
typedef std::map<const std::string,IGuiItem*>           ITEMS_MAP;
typedef std::map<const std::string,IGuiItem*>::iterator ITEMS_MAP_IT;


class CMenuScreen
{
public:
	CMenuScreen(std::string strName);
	~CMenuScreen();

	void AddItem(IGuiItem* pItem,std::string strName);

	IGuiItem* GetItem(std::string& strName);


	void Update(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left);
	void Render(void);

	void UnFocusAll(void);

	void SetParentName(std::string strParName) { m_ParentScreenName = strParName;};

	void SetEscapeActive(bool bActive) { m_bEscapeKeyActive = bActive; };
private:

	ITEMS_MAP m_Items;

	std::string m_Name;

	std::string m_ParentScreenName;

	bool m_bEscapeKeyActive;
};




#endif