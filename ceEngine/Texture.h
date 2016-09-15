#ifndef CRIMSON_TEXTURE_INTERFACE_H
#define CRIMSON_TEXTURE_INTERFACE_H

#pragma once

#include "ceLibrary.h"

class ITexture : public CReferenceCount
{
public:
	ITexture() {};
	virtual ~ITexture() {};

	//todo: remove mipmaping/compress etc. !!
	// since manager won't load different textures
	virtual bool Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress) = 0;
	virtual void Bind(int iSamplerNum) const = 0;
	virtual const char* GetName(void) const = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual bool GetForceReBind() const = 0;

	

};


#endif