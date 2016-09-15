///////////////////////////////////////////
//
// Texture Manager
//
///////////////////////////////////////////

#ifndef CRIMSON_TEXTURE_MANAGER_H
#define CRIMSON_TEXTURE_MANAGER_H

#pragma once


#define st_TextureManager CTextureManager::Inst()

#include "Texture.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"




class CTextureManager : public CManager<ITexture>,public CSingleton<CTextureManager>	
{
public:

	CTextureManager() {};
	~CTextureManager() {};
	
	CSmartPointer<ITexture> Load(const char *FileName);


};

#endif
