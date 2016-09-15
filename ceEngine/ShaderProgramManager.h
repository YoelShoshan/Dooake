///////////////////////////////////////////
//
// Shader Program Manager
//
///////////////////////////////////////////

#ifndef CRIMSON_SHADER_PROGRAM_MANAGER_H
#define CRIMSON_SHADER_PROGRAM_MANAGER_H

#pragma once


#define st_ShaderProgramManager CShaderProgramManager::Inst()

#include "ShaderProgram.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"


class CShaderProgramManager : public CManager<IShaderProgram>,public CSingleton<CShaderProgramManager>	
{
public:

	CShaderProgramManager() {};
	~CShaderProgramManager() {};
	
	CSmartPointer<IShaderProgram> Load(const char *FileName, DWORD dwType);
	CSmartPointer<IShaderProgram> LoadFromText(const char *FileName, const char *pchShaderText,DWORD dwType);

	void ReloadAllShaderPrograms(void) const;

private:

	CSmartPointer<IShaderProgram>  LoadHelper(const char *FileName, const char *pchShaderText,DWORD dwType);

};

#endif
