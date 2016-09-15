#include "stdafx.h"
#include "TextureManager.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "ce.h"

CSmartPointer<ITexture> CTextureManager::Load(const char *FileName)
{	
	assert(_CrtCheckMemory());
	ITexture* pAlreadyInMan= AlreadyInManager(FileName);
	if (!pAlreadyInMan)
	{
		ITexture* pTempTex;
		pTempTex = g_pGraphicsAPI->GenerateTexture();		
		

		if (pTempTex->Load(FileName,true,false,false))  // load was successfull
		{
			m_ResourcesMap[std::string(FileName)] = pTempTex; // add to map

			m_iResourcesNum++;
			return pTempTex;
		}
		else  // load FAILED
		{
			char Name[300];
			sprintf(Name,"CTextureManager::Load() Didn't find Texture [%s]!",FileName);
			g_pLogFile->OutPutPlainText(Name,LOG_MESSAGE_INFORMATIVE);


			// someone wanted a normal map
			if (strstr(FileName,"_local")!=0)
			{
				char* pNormalMapName = "ceData/default_local.tga";

				ITexture* pErrorImageInMan_Local = AlreadyInManager(pNormalMapName);
				if (!pErrorImageInMan_Local)	// wasn't found		
				{
					bool bRes = pTempTex->Load(pNormalMapName,true,false,false);
					if (!bRes)
					{
						char Name[300];
						sprintf(Name,"CTextureManager::Load() Didn't find default normal map [%s]!",pNormalMapName);
						g_pLogFile->OutPutPlainText(Name,LOG_MESSAGE_ERROR);
					}

					m_ResourcesMap[std::string(pNormalMapName)] = pTempTex; // add to map
					m_iResourcesNum++;
					return pTempTex;
				}
				else
				{
					return pErrorImageInMan_Local;
				}

			} else // not a normal map
			{
				ITexture* pErrorImageInMan = AlreadyInManager("ceData/default.jpg");
				if (!pErrorImageInMan)	// wasn't found		
				{
					pTempTex->Load("ceData/default.jpg",true,false,false);
					m_ResourcesMap[std::string("ceData/default.jpg")] = pTempTex; // add to map
					m_iResourcesNum++;
					pTempTex->Inc();
					return pTempTex;
				}
				else
				{
					//TODO - fix this leak !!! 
					// the problem was that the texture tries to unregister itself ...
					// perhaps add a flag DO_NOT_UNREGISTER or sth ...
					//SafeDelete(pTempTex);

					return pErrorImageInMan;
				}
			}
		}
	}
	else
	{
		return pAlreadyInMan;
	}
}




