#include "stdafx.h"
#include "guiButton.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "Input.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;


CGuiButton::CGuiButton()
{
	m_iState = CE_GUI_BUTTON_UP;

	m_dwAction = 0xFFFFFFFF;

}

CGuiButton::~CGuiButton()
{

}

void CGuiButton::RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left)
{
	if (!m_bActive)
		return;

	if (CursorOnMe(fMouseX_Perc,fMouseY_Perc))
	{
		m_iState = CE_GUI_BUTTON_HOVER;

		if (bMousePressed_Left)
		{
			m_iState = CE_GUI_BUTTON_DOWN;
			g_pMainMenu->Action(m_dwAction);
			return;
		}
	}
	else
		m_iState=CE_GUI_BUTTON_UP;

	return;
}
	
void CGuiButton::Render(void)
{
	if (!m_bRender)
		return;

	switch(m_iState)
	{
	case CE_GUI_BUTTON_UP:
		g_pRendererBack->Render2DQuad_Perc(m_spMatUP,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
		break;

	case CE_GUI_BUTTON_HOVER:
		g_pRendererBack->Render2DQuad_Perc(m_spMatHOVER,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
		break;

	case CE_GUI_BUTTON_DOWN:
		g_pRendererBack->Render2DQuad_Perc(m_spMatDOWN,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
		break;

	default:
		BREAKPOINT(1); // unrecognized button state!!
		break;
	};

	if (!m_bActive)
		g_pRendererBack->Render2DQuad_Perc(m_spMat_Cross,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
		
		

	//g_pRendererBack->Render2DQuad(m_spMat,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
	

	//g_pRendererBack->RenderFullScreenQuad(m_spMat);


}

void CGuiButton::Load(DWORD dwAction,const char *pchMatUP, const char *pchMatHOVER, const char*pchMatDOWN)
{
	m_dwAction = dwAction;
	m_spMatUP = g_pRendererBack->LoadMaterial(pchMatUP);
	m_spMatHOVER = g_pRendererBack->LoadMaterial(pchMatHOVER);
	m_spMatDOWN = g_pRendererBack->LoadMaterial(pchMatDOWN);


	m_spMat_Cross = g_pRendererBack->LoadMaterial("!GUI/Cross.tga");
}