#ifndef CRIMSON_RENDER_TO_TEXTURE_D3D_H
#define CRIMSON_RENDER_TO_TEXTURE_D3D_H

#pragma once

#include "RenderToTexture.h"

#include "ce.h"

#include "d3d9.h"
#include "d3dx9.h"

class CRenderToTexture_D3D : public IRenderToTexture
{
public:

	CRenderToTexture_D3D();
	~CRenderToTexture_D3D();


	void BindRenderTo(/*int iSamplerNum*/) const;
	void UnbindRenderTo() const;

	// Todo: make this do sth or change structre ...
	bool Load(const char* strFileName, bool bMipMap,bool bClamp, bool bCompress)
	{
		// perhaps create an fbo and load into it the image
		//(for example, if someone wants a background for a render target)

		BREAKPOINT(1);
		return false;
	};

	bool GetForceReBind() const
	 {
		 return false;
	 }

	void Init(const char* pName, DWORD dwType,int iWidth, int iHeight);

	void Bind(int iSamplerNum) const;
	const char* GetName(void) const;


	DWORD GetType() const { return m_dwType;};


	 int GetWidth() const
	 {
		 return m_iWidth;
	 }
	 int GetHeight() const
	 {
		 return m_iHeight;
	 }
 
private:

	struct DXUTDeviceSettings
	{
		UINT AdapterOrdinal;
		D3DDEVTYPE DeviceType;
		D3DFORMAT AdapterFormat;
		DWORD BehaviorFlags;
		D3DPRESENT_PARAMETERS pp;
	};

	 char* m_pTexName;
     int m_iWidth; 
     int m_iHeight; 

	 LPD3DXRENDERTOSURFACE   m_pRenderToSurface;
	 LPDIRECT3DTEXTURE9      m_pDynamicTexture;

	 LPDIRECT3DSURFACE9      m_pTextureSurface;
	 LPDIRECT3DSURFACE9      m_pTextureSurface_RT;



	 static LPDIRECT3DSURFACE9	m_pBack;



	 DWORD m_dwType;

	 bool m_bDepth;
};



#endif