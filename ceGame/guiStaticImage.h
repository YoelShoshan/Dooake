#ifndef CRIMSON_GUI_STATIC_IMAGE_H
#define CRIMSON_GUI_STATIC_IMAGE_H

#pragma once

#include "guiItem.h"
#include "SmartPointer.h"
#include "Material_IE.h"


class CGuiStaticImage : public IGuiItem
{
public:

	CGuiStaticImage();
	~CGuiStaticImage();
	
	void Render(void);
	void RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left) {};
	
	void* GetInfo(void) {return NULL;};



	void Load(const char *pchMaterialName);

private:

	CSmartPointer<IMaterial_Export> m_spMat;

};
















#endif