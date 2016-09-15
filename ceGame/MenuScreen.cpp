#include "stdafx.h"
#include "MenuScreen.h"

#include "ce.h"

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;

#include "Input.h"

CMenuScreen::CMenuScreen(std::string strName)
{
	m_Name = strName;
	m_ParentScreenName = "0";
	m_bEscapeKeyActive = true;
}

CMenuScreen::~CMenuScreen()
{

}

void CMenuScreen::AddItem(IGuiItem* pItem,std::string strName)
{
	m_Items[strName] = pItem;	
}

void CMenuScreen::UnFocusAll(void)
{
	for (ITEMS_MAP_IT it = m_Items.begin();it!=m_Items.end();++it)
		it->second->SetFocus(false);

}

IGuiItem* CMenuScreen::GetItem(std::string& strName)
{
	ITEMS_MAP_IT it = m_Items.find(strName);

	if (it == m_Items.end())
	{
		BREAKPOINT(1); // item wasn't found!
	}

	return it->second;

}

void CMenuScreen::Update(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left)
{
	if (!g_pMainMenu->GetChatActive())
	{
		if (m_bEscapeKeyActive)
			if (st_Input->KeyPressed(DIK_ESCAPE))
			{
				if (strcmp("0",m_ParentScreenName.c_str()))
				{
					g_pMainMenu->SetActiveScreen(m_ParentScreenName);
				}

			}
	}
	
	// find if mouse is on top, find clicks etc.
	for (ITEMS_MAP_IT it=m_Items.begin();it!=m_Items.end();++it)
	{
		it->second->RunLogics(fMouseX_Perc,fMouseY_Perc,bMousePressed_Left);			
	}
}

void CMenuScreen::Render(void)
{
	for (ITEMS_MAP_IT it=m_Items.begin();it!=m_Items.end();++it)
	{
		it->second->Render();
	}
}
