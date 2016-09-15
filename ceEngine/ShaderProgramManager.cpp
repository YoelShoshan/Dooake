#include "stdafx.h"
#include "ShaderProgramManager.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

//#include "Logics.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "ce.h"

void CShaderProgramManager::ReloadAllShaderPrograms(void) const
{
	std::map<std::string,IShaderProgram*>::const_iterator it = m_ResourcesMap.begin();

	while (it!=m_ResourcesMap.end())
	{
		it->second->Reload();
		it++;
	}

}


CSmartPointer<IShaderProgram>  CShaderProgramManager::LoadHelper(const char *FileName, const char *pchShaderText,DWORD dwType)
{
	if ((dwType!=VERTEX_SHADER_PROGRAM) && (dwType!=FRAGMENT_SHADER_PROGRAM))
	{
		char error[400];
		sprintf_s(error,400,"CShaderProgramManager::Load ""%s "": unrecognized shaderprogram type!",FileName);
		g_pLogFile->OutPutPlainText(error,LOG_MESSAGE_ERROR);
		return NULL;
	}


	IShaderProgram* pAlreadyInMan= AlreadyInManager(FileName);
	if (!pAlreadyInMan)
	{		
		IShaderProgram* pSharedProgram;

		if (dwType==VERTEX_SHADER_PROGRAM)
			pSharedProgram = (IShaderProgram*)g_pGraphicsAPI->GenerateShaderVertexProgram();
		else //if (dwType==FRAGMENT_SHADER_PROGRAM)
			pSharedProgram = (IShaderProgram*)g_pGraphicsAPI->GenerateShaderFragmentProgram();

		bool bLoadSuccessful = false;

		if (pchShaderText)
			bLoadSuccessful = pSharedProgram->LoadFromText(FileName,pchShaderText);
		else
			bLoadSuccessful = pSharedProgram->Load(FileName);

		if (bLoadSuccessful)  // load was successfull
		{
			m_ResourcesMap[std::string(FileName)] = pSharedProgram; // add to map

			m_iResourcesNum++;
			return pSharedProgram;
		}
		else  // load FAILED
		{
			IShaderProgram* pErrorImageInMan;

			if (dwType==VERTEX_SHADER_PROGRAM)
				pErrorImageInMan = AlreadyInManager("ceData/errorVP.cg");
			else
				pErrorImageInMan = AlreadyInManager("ceData/errorFP.cg");


			if (!pErrorImageInMan)	// wasn't found		
			{
				if (dwType==VERTEX_SHADER_PROGRAM)
					pSharedProgram->Load("ceData/errorVP.cg");
				else //if (dwType==FRAGMENT_SHADER_PROGRAM)
					pSharedProgram->Load("ceData/errorFP.cg");


				m_ResourcesMap[std::string(FileName)] = pSharedProgram; // add to map
				m_iResourcesNum++;
				return pSharedProgram;
			}
			else
			{
				SafeDelete(pSharedProgram);
				return pErrorImageInMan;
			}
		}
	}
	else
	{
		return pAlreadyInMan;
	}

}

CSmartPointer<IShaderProgram> CShaderProgramManager::LoadFromText(const char *FileName, const char *pchShaderText,DWORD dwType)
{
	return LoadHelper(FileName,pchShaderText,dwType);
}

CSmartPointer<IShaderProgram> CShaderProgramManager::Load(const char *FileName, DWORD dwType)
{	
	return LoadHelper(FileName,NULL,dwType);	
}




