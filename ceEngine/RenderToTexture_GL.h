#ifndef CRIMSON_RENDER_TO_TEXTURE_GL_H
#define CRIMSON_RENDER_TO_TEXTURE_GL_H

#pragma once

#include "RenderToTexture.h"

#include "ce.h"

class CRenderToTexture_GL : public IRenderToTexture
{
public:
	void BindRenderTo(/*int iSamplerNum*/) const;
	void UnbindRenderTo() const;

	// Todo: make this do sth or change structre ...
	bool Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress)
	{
		// perhaps create an fbo and load into it the image
		//(for example, if someone wants a background for a render target)

		BREAKPOINT(1);
		return false;
	};

	DWORD GetType() const { return m_dwType;};

	void Init(const char* pName,  DWORD dwType,int iWidth, int iHeight);

	void Bind(int iSamplerNum) const;
	const char* GetName(void) const;


	bool GetForceReBind() const
	 {
		 return false;
	 }
	


	 int GetWidth() const
	 {
		 return m_iWidth;
	 }
	 int GetHeight() const
	 {
		 return m_iHeight;
	 }
 
private:
	 char* m_pTexName;
     int m_iWidth; 
     int m_iHeight; 

	 unsigned int m_uiFboID;
	 unsigned int m_uiTexID;
	 unsigned int m_uiDepthRBID;

	 DWORD m_dwType;
};



#endif