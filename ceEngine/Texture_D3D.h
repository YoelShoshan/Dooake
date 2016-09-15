#ifndef CRIMSON_TEXTURE_D3D_H
#define CRIMSON_TEXTURE_D3D_H

#pragma once

#include "d3d9.h"
#include "d3dx9.h"

#include "Texture.h"

class CTexture_D3D : public ITexture
{
friend class CGraphicsAPI_D3D;
public:	
	~CTexture_D3D();

	bool Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress);
	void Bind(int iSamplerNum) const;
	inline const char* GetName() const { return m_pcTexName; }; 

	int GetWidth() const
	 {
		 return m_iWidth;
	 }
	 int GetHeight() const
	 {
		 return m_iHeight;
	 }

	 bool GetForceReBind() const
	 {
		 return false;
	 }

protected:
	CTexture_D3D();

	bool FindTextureExtension(char *strFileName);

	char* m_pcTexName;
	LPDIRECT3DTEXTURE9 m_pD3DTexture;

	int m_iWidth;
	int m_iHeight;
};










#endif