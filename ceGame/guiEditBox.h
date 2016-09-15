#ifndef CRIMSON_GUI_EDIT_BOX_H
#define CRIMSON_GUI_EDIT_BOX_H

#pragma once

#include "guiItem.h"
#include "SmartPointer.h"
#include "Material_IE.h"

#include <string>

#define EDIT_BOX_TEXT_BUFFER_SIZE 400

#define EDIT_BOX_TYPE_FREE_TEXT_WITH_NEWLINES 0
#define EDIT_BOX_TYPE_FREE_TEXT_NO_NEWLINES   1
#define EDIT_BOX_TYPE_IP_ADDRESS              2


class CGuiEditBox : public IGuiItem
{
public:
	CGuiEditBox();
	~CGuiEditBox();

	void Render(void);
	void RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left);
	void* GetInfo(void);
	void Clear(void);
	

	// if null then no background is used
	void Load(DWORD dwType,const char *pchMatBackground,const char *pchMatBackgroundFocus,int iLimit);

	void SetText(const char* pText);

	static bool s_bTPressed;

private:

	bool IsLegalIPCharacters(const char* pTestMe) const;

	CSmartPointer<IMaterial_Export> m_spMatBackground;
	CSmartPointer<IMaterial_Export> m_spMatBackgroundFocus;

	
	int m_iLimit;

	int  m_iPosInBuffer;
	char m_Text[EDIT_BOX_TEXT_BUFFER_SIZE];

	
	DWORD m_dwType;
};





#endif