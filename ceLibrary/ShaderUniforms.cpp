#include "stdafx.h"
#include "ShaderUniforms.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include <assert.h>



CShaderUniforms::CShaderUniforms()
{
}

CShaderUniforms::~CShaderUniforms()
{
}
bool CShaderUniforms::Add(const char* pchName,tUniform* pAU)
{
	UNIFORMS_MAP_IT it;
	std::string stdName(pchName);

	it = m_Uniforms.find(stdName);

	if (it!=m_Uniforms.end()) // it's already in the map!
	{
		g_pLogFile->OutPutPlainText("CShaderUniforms::Add::unifrom already exists in map!",LOG_MESSAGE_ERROR);
		return false;
	}


	m_Uniforms[stdName] = pAU;

	return true;
}

tUniform* CShaderUniforms::Get(const char* pchName)
{

	assert(_CrtCheckMemory());

	UNIFORMS_MAP_IT it;
	std::string stdName(pchName);

	//assert(_CrtCheckMemory());

	it = m_Uniforms.find(stdName);

	//assert(_CrtCheckMemory());

	if (it!=m_Uniforms.end()) // it's already in the map!
		return it->second;
	else // it wasn't found in the map
	{
		//assert(_CrtCheckMemory());

		char temp[500];
		sprintf(temp,"CShaderUniforms::Get::uniform [%s] doesn't exist!",pchName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);

		//assert(_CrtCheckMemory());

		return NULL;
	}
}

void CShaderUniforms::SetStart(void)
{
	m_Uniforms_it = m_Uniforms.begin();
}

void CShaderUniforms::Next(void)
{
	m_Uniforms_it++;
}

tUniform* CShaderUniforms::GetCurrent(void)
{
	if (m_Uniforms_it == m_Uniforms.end() ) // we reached the end
		return NULL;
	else
		return m_Uniforms_it->second;
}

bool CShaderUniforms::IsEnd(void)
{
	return (m_Uniforms_it == m_Uniforms.end());
}