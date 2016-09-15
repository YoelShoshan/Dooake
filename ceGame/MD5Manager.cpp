#include "stdafx.h"
#include "MD5Manager.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

CSmartPointer<CMD5_Shared> CMD5Manager::Load(const char *FileName)
{	
//	assert(_CrtCheckMemory());
	CMD5_Shared* pAlreadyInMan= AlreadyInManager(FileName);
	if (!pAlreadyInMan)
	{
		CMD5_Shared* pSharedModel;
		pSharedModel = new CMD5_Shared();

		// hacked touching
		pSharedModel->Inc();
		
		//assert(_CrtCheckMemory());

		if (pSharedModel->LoadCharacterFile(FileName))  // load was successfull
		{

			//assert(_CrtCheckMemory());

			m_ResourcesMap[std::string(FileName)] = pSharedModel; // add to map

			m_iResourcesNum++;
			return pSharedModel;
		}
		else  // load FAILED
		{
			pSharedModel->Dec();

			SafeDelete(pSharedModel);
			return NULL;

			/*CMD5_Shared* pErrorImageInMan = AlreadyInManager("ceImages/TexNotFound.bmp");
			if (!pErrorImageInMan)	// wasn't found		
			{
				pSharedModel->Load3DS("ceModels/error.3ds",0.1f);
				m_ResourcesMap[std::string(FileName)] = pSharedModel; // add to map
				m_iResourcesNum++;
				return pSharedModel;
			}
			else
			{
				SafeDelete(pSharedModel);
				return pErrorImageInMan;
			}*/
		}
	}
	else
	{
		return pAlreadyInMan;
	}
}




