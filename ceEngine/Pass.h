#ifndef CRIMSON_PASS_H
#define CRIMSON_PASS_H

#pragma once

#define SS_FILTERING_BILINEAR            1		
#define SS_FILTERING_TRILINEAR           2		
#define SS_FILTERING_ANISOTROPIC         3
#define SS_FILTERING_NEAREST             4
#define SS_FILTERING_NEAREST_NO_MIPMAPS  5

#define SS_CLAMPING_REPEAT 0
#define SS_CLAMPING_CLAMP  1

#include "SmartPointer.h"
#include "Texture.h"
#include "Shader.h"
#include "RendererState.h"
#include "GraphicsAPI.h"

#include "RTTProxy.h"

struct tSamplerState
{	
	tSamplerState():iTextureType(CR_TEXTURE_OFF),
		cClamping(SS_CLAMPING_REPEAT/*SS_CLAMPING_CLAMP*/),
		cFiltering(SS_FILTERING_TRILINEAR),		
		cAnisotropicLevel(1){};
	~tSamplerState();

	char cClamping;
	char cFiltering;
	char cAnisotropicLevel;
	
	bool SetTexture(CSmartPointer<ITexture> setspTex);
	CSmartPointer<ITexture> GetTexture(void) { return spTex;};

	int iTextureType;

	CSmartPointer<ITexture> spTex;
};


class CPass
{
friend class CTechnique;
public:
	~CPass();

	bool AddTexture(CSmartPointer<ITexture> spTex,int iTexUnitNum);
	
	void operator=(CPass& other);

	void SetShadowMapRTTProxy(CRenderToTextureProxy* pRTTProxy);
	CRenderToTextureProxy* GetShadowMapRTTProxy();

	void InjectShadowMap(IRenderToTexture* pRTT);
		
	bool IsOpaque()
	{
		return m_RendererState.IsOpaque();
	}

	CShader vertexProgram;
	CShader fragmentProgram;

	tRendererState m_RendererState;
	tSamplerState  SamplerStates[API_TEX_UNITS_NUM];

	// for lighting

	DWORD m_dwLighting;

protected:
	CPass();

private:

	CRenderToTextureProxy* m_pShadowMapRTTProxy;

};







#endif