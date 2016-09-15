#ifndef CRIMSON_GUI_BUTTON_H
#define CRIMSON_GUI_BUTTON_H

#pragma once

#include "guiItem.h"
#include "SmartPointer.h"
#include "Material_IE.h"

#define CE_GUI_BUTTON_UP     0
#define CE_GUI_BUTTON_HOVER  1
#define CE_GUI_BUTTON_DOWN   2

class CGuiButton : public IGuiItem
{
public:

	CGuiButton();
	~CGuiButton();
	
	void Render(void);
	void RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left);
	

	void* GetInfo(void) {return NULL;};



	void Load(DWORD dwAction,const char *pchMatUP, const char *pchMatHOVER, const char*pchMatDOWN);

private:

	CSmartPointer<IMaterial_Export> m_spMatUP;
	CSmartPointer<IMaterial_Export> m_spMatHOVER;
	CSmartPointer<IMaterial_Export> m_spMatDOWN;


	CSmartPointer<IMaterial_Export> m_spMat_Cross;

	int m_iState;
	DWORD m_dwAction;
};
















#endif