#include "stdafx.h"
#include "StaticModelManager.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "ce.h"

CSmartPointer<CStaticModel_Shared> CStaticModelManager::Load(const char *FileName, float fScaleX, float fScaleY, float fScaleZ)
{	
//	assert(_CrtCheckMemory());
	CStaticModel_Shared* pAlreadyInMan= AlreadyInManager(FileName);
	if (!pAlreadyInMan || FileName[0]=='$')
	{
		CStaticModel_Shared* pSharedModel;
		pSharedModel = new CStaticModel_Shared();

		pSharedModel->Inc();
		
		if (FileName[0]=='$' || FileName[0]=='%') // a request for generation of quad
		{
			int iTessLevel = 1;
			
			if (FileName[1]!=NULL)
				iTessLevel = atoi(&FileName[1]);

			if (FileName[0]=='$')
				pSharedModel->Generate(GENERATE_QUAD,iTessLevel,fScaleX,fScaleY,fScaleZ,RENDERABLE_GENERATE_PPL);
			else //if (FileName[0]=='%')
				pSharedModel->Generate(GENERATE_QUAD,iTessLevel,fScaleX,fScaleY,fScaleZ,RENDERABLE_GENERATE_NONE);

			m_ResourcesMap[std::string(FileName)] = pSharedModel; // add to map

			m_iResourcesNum++;
			return pSharedModel;
		}
		else
		if (pSharedModel->Load3DS(FileName,fScaleX, fScaleY, fScaleZ))  // load was successfull
		{
			m_ResourcesMap[std::string(FileName)] = pSharedModel; // add to map

			m_iResourcesNum++;
			return pSharedModel;
		}
		else  // load FAILED
		{
			//

			CStaticModel_Shared* pErrorImageInMan = AlreadyInManager("ceData/error.3ds");
			if (!pErrorImageInMan)	// wasn't found		
			{
				pSharedModel->Load3DS("ceData/error.3ds",0.1f);
				m_ResourcesMap[std::string("ceData/error.3ds")] = pSharedModel; // add to map
				m_iResourcesNum++;
				return pSharedModel;
			}
			else
			{
				pSharedModel->Dec();
				//SafeDelete(pSharedModel);
				return pErrorImageInMan;
			}
		}
	}
	else
	{
		return pAlreadyInMan;
	}
}




