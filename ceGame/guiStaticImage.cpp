#include "stdafx.h"
#include "guiStaticImage.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "Timer_IE.h"
extern ITimer* g_pTimer;


CGuiStaticImage::CGuiStaticImage()
{

}

CGuiStaticImage::~CGuiStaticImage()
{

}


	
void CGuiStaticImage::Render(void)
{
	if (!m_bRender)
		return;

	//g_pRendererBack->Render2DQuad(m_spMat,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
	g_pRendererBack->Render2DQuad_Perc(m_spMat,m_fPosX,m_fPosY,m_fWidth,m_fHeight);

	//g_pRendererBack->RenderFullScreenQuad(m_spMat);


}

void CGuiStaticImage::Load(const char *pchMaterialName)
{
	m_spMat = g_pRendererBack->LoadMaterial(pchMaterialName);

}