#include "stdafx.h"
#include "Shader.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "ShaderProgramManager.h"


bool CShader::LoadVPFromText(const char* pchShaderName,const char* pchShaderText)
{
	m_spShaderProgram = st_ShaderProgramManager->LoadFromText(pchShaderName,pchShaderText,VERTEX_SHADER_PROGRAM);

	if (m_spShaderProgram)
		return true;

	return false;

}

bool CShader::LoadFPFromText(const char* pchShaderName,const char* pchShaderText)
{
	m_spShaderProgram = st_ShaderProgramManager->LoadFromText(pchShaderName,pchShaderText,FRAGMENT_SHADER_PROGRAM);

	if (m_spShaderProgram)
		return true;

	return false;

}

bool CShader::LoadVP(const char* pchShaderName)
{
	m_spShaderProgram = st_ShaderProgramManager->Load(pchShaderName,VERTEX_SHADER_PROGRAM);

	if (m_spShaderProgram)
		return true;

	return false;
}

bool CShader::LoadFP(const char* pchShaderName)
{
	m_spShaderProgram = st_ShaderProgramManager->Load(pchShaderName,FRAGMENT_SHADER_PROGRAM);

	if (m_spShaderProgram)
		return true;

	return false;
}

bool CShader::Bind(DWORD dwProgType) const
{
	if (dwProgType==VERTEX_SHADER_PROGRAM)
		g_pGraphicsAPI->BindVertexShader(m_spShaderProgram,m_pShaderUniforms);
	else if (dwProgType==FRAGMENT_SHADER_PROGRAM)
		g_pGraphicsAPI->BindFragmentShader(m_spShaderProgram,m_pShaderUniforms);
	else
		BREAKPOINT(1);
	
	return true;
}

void CShader::Unbind(void) const
{
	m_spShaderProgram->Unbind();
}

bool CShader::AddUniform(const char* pchName,tUniform* pAU)
{

	if (!m_pShaderUniforms)
		m_pShaderUniforms = new CShaderUniforms();

	return (m_pShaderUniforms->Add(pchName,pAU));
}

void CShader::operator= (CShader& other)
{
	m_spShaderProgram = other.m_spShaderProgram;

	delete m_pShaderUniforms;
	m_pShaderUniforms = new CShaderUniforms();

	/*other.m_pShaderUniforms->SetStart();

	tUniform* pUniform = other.m_pShaderUniforms->GetCurrent();

	m_pShaderUniforms->Add(pUniform->m_pchName,pUniform);*/

	////

	tUniform*   pUniform=NULL;

	while (!other.m_pShaderUniforms->IsEnd())
	{		
		pUniform = other.m_pShaderUniforms->GetCurrent();
		BREAKPOINT(!pUniform);

		m_pShaderUniforms->Add(pUniform->m_pchName,pUniform);
	}

	//other.m_pShaderUniforms->SetStart();
}