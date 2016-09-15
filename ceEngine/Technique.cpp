#include "stdafx.h"
#include "Technique.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "RendererBackend.h"
extern CRendererBackend* g_RendererBackend;

#include <assert.h>

extern DWORD g_dwForcedCullingMethod;

CTechnique::CTechnique()
{ 
	m_iPassesNum =0; 
	
	for (int i=0;i<TECHNIQUE_PASSES_MAX;i++) 
		m_Passes[i] = NULL;            
};

CTechnique::~CTechnique() 
{
	for (int i=0;i<m_iPassesNum;i++)
		SafeDelete(m_Passes[i]);
};

CPass* CTechnique::GetPass(int iIndex) const
{
	if (iIndex>=TECHNIQUE_PASSES_MAX)
	{
		g_pLogFile->OutPutPlainText("CTechnique::GetPass::Trying to get a pass beyond TECHNIQUE_PASSES_MAX",LOG_MESSAGE_ERROR);
		return NULL;
	}

	if (iIndex>=m_iPassesNum)
	{
		g_pLogFile->OutPutPlainText("CTechnique::GetPass::Trying to get a pass beyond technique passes num",LOG_MESSAGE_ERROR);
		return NULL;
	}

	return m_Passes[iIndex];
}

CPass* CTechnique::AddPass(void)
{
	/*if (!pPass)
	{
		g_pLogFile->OutPutPlainText("CTechnique::AddPass::Trying to add a NULL pass",LOG_MESSAGE_ERROR);
		return false;

	}*/

	if (m_iPassesNum>=TECHNIQUE_PASSES_MAX)
	{
		g_pLogFile->OutPutPlainText("CTechnique::AddPass::Trying to add a pass beyond TECHNIQUE_PASSES_MAX",LOG_MESSAGE_ERROR);
		return NULL;
	}

	CPass* pPass = new CPass();

	m_Passes[m_iPassesNum++] = pPass;

	return pPass;
}

int CTechnique::GetPassesNum(void) const
{
	return m_iPassesNum;
}

// if first pass is opaque, then the entire technique is opaque
bool	CTechnique::IsOpaque()
{
	assert(m_iPassesNum);
	bool bOpaque = m_Passes[0]->IsOpaque();
	return bOpaque;
}


bool CTechnique::BindPass(int iPassNum) const
{
	if (iPassNum>=m_iPassesNum)
	{
		g_pLogFile->OutPutPlainText("CTechnique::BindPass::Trying to bind a pass beyond technique passes num",LOG_MESSAGE_ERROR);

		return false;
	}

	CPass* pPass = m_Passes[iPassNum];


	if (g_dwForcedCullingMethod!=CE_DONT_FORCE_CULLING_METHOD)
		g_pGraphicsAPI->SetCullFunc(g_dwForcedCullingMethod);
	else
		g_pGraphicsAPI->SetCullFunc(pPass->m_RendererState.m_dwCullFunc);


	g_pGraphicsAPI->SetDepthTest(pPass->m_RendererState.m_bDepthTest);
	g_pGraphicsAPI->SetDepthWrite(pPass->m_RendererState.m_bDepthWrite);
	g_pGraphicsAPI->SetDepthFunc(pPass->m_RendererState.m_dwDepthFunc);
	g_pGraphicsAPI->SetBlendFunc(pPass->m_RendererState.m_cBlendSource,
		pPass->m_RendererState.m_cBlendTarget);
	g_pGraphicsAPI->SetAlphaFunc(pPass->m_RendererState.m_dwAlphaFunc);
	g_pGraphicsAPI->SetAlphaReferenceValue(pPass->m_RendererState.m_dwAlphaRefValue);

	//CTechnique* pTech = m_pMaterial->GetTechnique(0);

	
	for (int i=0;i<API_TEX_UNITS_NUM;i++)
			if (pPass->SamplerStates[i].GetTexture())
			{
				g_pGraphicsAPI->BindTexture(i,pPass->SamplerStates[i].GetTexture());		
				g_pGraphicsAPI->SetClamping(i,pPass->SamplerStates[i].cClamping);
				g_pGraphicsAPI->SetFiltering(i,pPass->SamplerStates[i].cFiltering,pPass->SamplerStates[i].cAnisotropicLevel);
			}
			else
				g_pGraphicsAPI->UnBindTexUnit(i); //why this causes problems ??


	pPass->vertexProgram.Bind(VERTEX_SHADER_PROGRAM);
	pPass->fragmentProgram.Bind(FRAGMENT_SHADER_PROGRAM);

	return true;
}








