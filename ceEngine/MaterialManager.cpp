#include "stdafx.h"
#include "MaterialManager.h"

#include "Material.h"
#include "Q3AShader_Data.h"
//#include "Logics.h"
#include <assert.h>

CSmartPointer<IMaterial_Export> CMaterialManager::LoadQ3AShader(const CQ3A_Shader_Data* pQ3AShaderData,bool& bScripted)
{
	assert(pQ3AShaderData);

	IMaterial_Export* pAlreadyInMan= AlreadyInManager(pQ3AShaderData->m_cpShaderName);

	if (!pAlreadyInMan)
	{
		// convert q3ashader into Crimson Engine material

		IMaterial_Export* pSharedMaterial;
		pSharedMaterial = new CMaterial();

		// hacked TOUCHING
		pSharedMaterial->Inc();

		bool bRes = pSharedMaterial->GenerateFromQ3AShader(pQ3AShaderData);

		if (!bRes) //
		{
		
			//pSharedMaterial->Dec();
			IMaterial_Export* pErrorImageInMan = AlreadyInManager("ceData/error.utm");
			if (!pErrorImageInMan)	// wasn't found		
			{
				pSharedMaterial->Load("ceData/error.utm");
				m_ResourcesMap[std::string("ceData/error.utm")] = pSharedMaterial; // add to map
				m_iResourcesNum++;
				return pSharedMaterial;
			}
			else
			{
				pSharedMaterial->Dec();
				//SafeDelete(pSharedMaterial);
				return pErrorImageInMan;
			}		
		}

		m_ResourcesMap[std::string(pQ3AShaderData->m_cpShaderName)] = pSharedMaterial; // add to map

		m_iResourcesNum++;
		return pSharedMaterial;


	} else
	{
		return pAlreadyInMan;
	}

}

CSmartPointer<IMaterial_Export> CMaterialManager::Load(const char *FileName)
{	
//	assert(_CrtCheckMemory());
	IMaterial_Export* pAlreadyInMan= AlreadyInManager(FileName);
	if (!pAlreadyInMan)
	{
		IMaterial_Export* pSharedMaterial;
		pSharedMaterial = new CMaterial();

		// hacked TOUCHING
		pSharedMaterial->Inc();
		
		if (FileName[0]=='@') // means generate shadow mapped,light passes based material
		{
			//pSharedMaterial->GenerateTextureMaterial(&FileName[1]);
			pSharedMaterial->GeneratePerPixelLitMaterial(&FileName[1]);

			m_ResourcesMap[std::string(FileName)] = pSharedMaterial; // add to map

			m_iResourcesNum++;
			return pSharedMaterial;
		} else
		if (FileName[0]=='#') // means generate simple basic texture material
		{
			//pSharedMaterial->GenerateTextureMaterial(&FileName[1]);
			pSharedMaterial->GenerateSimpleTextureMaterial(&FileName[1]);

			m_ResourcesMap[std::string(FileName)] = pSharedMaterial; // add to map

			m_iResourcesNum++;
			return pSharedMaterial;
		} else
		if (FileName[0]=='!') // means generate alpha blended basic texture material
		{
			//pSharedMaterial->GenerateTextureMaterial(&FileName[1]);
			pSharedMaterial->GenerateAlphaBlendedAlwaysDrawTextureMaterial(&FileName[1]);

			m_ResourcesMap[std::string(FileName)] = pSharedMaterial; // add to map

			m_iResourcesNum++;
			return pSharedMaterial;
		} else
		if (pSharedMaterial->Load(FileName))  // load was successfull
		{
			m_ResourcesMap[std::string(FileName)] = pSharedMaterial; // add to map

			m_iResourcesNum++;
			return pSharedMaterial;
		}
		else  // load FAILED
		{
			pSharedMaterial->Dec();
			IMaterial_Export* pErrorImageInMan = AlreadyInManager("ceData/error.utm");
			if (!pErrorImageInMan)	// wasn't found		
			{
				pSharedMaterial->Load("ceData/error.utm");
				m_ResourcesMap[std::string("ceData/error.utm")] = pSharedMaterial; // add to map
				m_iResourcesNum++;
				return pSharedMaterial;
			}
			else
			{
				SafeDelete(pSharedMaterial);
				return pErrorImageInMan;
			}
		}
	}
	else
	{
		return pAlreadyInMan;
	}
}




