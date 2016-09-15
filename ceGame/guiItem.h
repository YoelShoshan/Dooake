#ifndef CRIMSON_GUI_ITEM_INTERFACE_H
#define CRIMSON_GUI_ITEM_INTERFACE_H

#pragma once

#include "MenuScreen.h"

class CMenuScreen;
class IGuiItem
{
public:
	IGuiItem();
	virtual ~IGuiItem() {};


	virtual void Render(void)       = 0;
	// if true is returned, then an action occured due to mouse click
	virtual void RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left)    = 0;
	virtual void* GetInfo(void) = 0;
	
	
	bool CursorOnMe(float fMouseX_Perc,float fMouseY_Perc)
	{
		if (fMouseX_Perc >= m_fPosX && fMouseX_Perc<= m_fPosX+m_fWidth &&
			fMouseY_Perc >= m_fPosY-m_fHeight && fMouseY_Perc<= m_fPosY)
			return true;

		return false;

	};

	void SetLocation(float fPosX,float fPosY,float fWidth,float fHeight) 
	{
		m_fPosX   = fPosX;
		m_fPosY   = fPosY;
		m_fWidth  = fWidth;
		m_fHeight = fHeight;
	};

	void SetParent(CMenuScreen* pParent);
	

	void SetFocus(void);
	
	float m_fPosX,m_fPosY,m_fWidth,m_fHeight;

	CMenuScreen* m_pParentScreen;


	void SetActive(bool bActive) { m_bActive = bActive;};
	void SetRender(bool bRender) { m_bRender = bRender;};
	void SetFocus(bool bFocus)   { m_bFocus = bFocus;};

	bool m_bActive;
	bool m_bFocus;
	bool m_bRender;

};




#endif