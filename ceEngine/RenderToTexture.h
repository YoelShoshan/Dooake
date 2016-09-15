#ifndef CRIMSON_RENDER_TO_TEXTURE_INTERFACE_H
#define CRIMSON_RENDER_TO_TEXTURE_INTERFACE_H

#pragma  once

#include "ceLibrary.h"
#include "Texture.h"

class IRenderToTexture : public ITexture
{
	public:
	IRenderToTexture() {};
	~IRenderToTexture() {};


	virtual void Init(const char* pName, DWORD dwType,int iWidth, int iHeight) = 0;
	virtual void BindRenderTo(/*int iSamplerNum*/) const  = 0;
	virtual void UnbindRenderTo() const = 0;
	virtual DWORD GetType() const = 0;


};


#endif