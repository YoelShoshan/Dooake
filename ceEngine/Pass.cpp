#include "stdafx.h"
#include "Pass.h"

#include "Logfile.h"
extern ILogFile* g_pLogFile;




bool tSamplerState::SetTexture(CSmartPointer<ITexture> setspTex)
{
	if (!setspTex)
	{
		g_pLogFile->OutPutPlainText("tSamplerState::SetTexture trying to set a NULL texture!",LOG_MESSAGE_ERROR);
		return false;
	}

	spTex = setspTex;
	return true;
}

 tSamplerState::~tSamplerState()
 {
	 spTex = NULL;
 }




void CPass::operator=(CPass& other)
{
	vertexProgram		= other.vertexProgram;
	fragmentProgram		= other.fragmentProgram;

	m_RendererState		= other.m_RendererState;

	for (int i=0;i<API_TEX_UNITS_NUM;i++)
		SamplerStates[i] = other.SamplerStates[i];

	m_dwLighting		= other.m_dwLighting;

	CRenderToTextureProxy* m_pShadowMapRTTProxy;
}

CPass::CPass()
{
	m_dwLighting = MAT_PASS_LIGHTING_AMBIENT;

	m_pShadowMapRTTProxy = NULL;
}

CPass::~CPass()
{
	

/*	for (int i=0;i<API_TEX_UNITS_NUM;i++)
		if (SamplerStates[i].spTex)
			SamplerStates[i].spTex = NULL;*/
	
}

bool CPass::AddTexture(CSmartPointer<ITexture> spTex,int iTexUnitNum)
{
	if (iTexUnitNum>API_TEX_UNITS_NUM)
		g_pLogFile->OutPutPlainText("CPass::AddTexture:: trying to add a texture to a tex unit beyond PASS_TEX_UNITS_MAX",LOG_MESSAGE_ERROR);

	SamplerStates[iTexUnitNum].SetTexture(spTex);
	return true;
}

void CPass::SetShadowMapRTTProxy(CRenderToTextureProxy* pRTTProxy)
{
	m_pShadowMapRTTProxy = pRTTProxy;
}

 CRenderToTextureProxy* CPass::GetShadowMapRTTProxy()
{
	return m_pShadowMapRTTProxy; 
}

void   CPass::InjectShadowMap(IRenderToTexture* pRTT)
{
	assert(pRTT);

	m_pShadowMapRTTProxy->SetRenderToTexture(pRTT);
}


