#ifndef CRIMSON_POST_PROCESS_TEXTURE_H
#define CRIMSON_POST_PROCESS_TEXTURE_H

#pragma  once

#include "ceLibrary.h"
#include "RenderToTexture.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

class CRenderToTextureProxy : public IRenderToTexture
{
public:
	CRenderToTextureProxy() 
	{
		m_pRenderToTexture = NULL;
	};
	~CRenderToTextureProxy() {};

	// sets render to texture and returns previous
	void SetRenderToTexture(IRenderToTexture* pRenderToTex)
	{
		m_pRenderToTexture = pRenderToTex;
	}

	DWORD GetType() const { return m_pRenderToTexture->GetType();};

	IRenderToTexture* GetRenderToTexture()
	{
		return m_pRenderToTexture;
	}

	bool GetForceReBind() const
	 {
		 return true;
	 }
		
	bool Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress)
	{ 
		// this should never happen !
		BREAKPOINT(1);
	}

	void Bind(int iSamplerNum) const
	{
		m_pRenderToTexture->Bind(iSamplerNum);
	}

	const char* GetName(void) const
	{
		return m_pRenderToTexture->GetName();
	}

	void Init(const char* pName, DWORD dwType,int iWidth, int iHeight)
	{
		m_pRenderToTexture = g_pGraphicsAPI->GenerateRenderToTexture();
		m_pRenderToTexture->Init(pName,dwType,iWidth,iHeight);
	}

	void BindRenderTo(/*int iSamplerNum*/) const
	{
		m_pRenderToTexture->BindRenderTo(/*iSamplerNum*/);
	}
	
	void UnbindRenderTo() const
	{
		m_pRenderToTexture->UnbindRenderTo();
	}

	int GetWidth() const
	 {
		 return m_pRenderToTexture->GetWidth();
	 }
	 int GetHeight() const
	 {
		 return m_pRenderToTexture->GetHeight();
	 }

private:

	IRenderToTexture* m_pRenderToTexture;
};


#endif