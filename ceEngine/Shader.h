#ifndef CRIMSON_SHADER_INTERFACE_H
#define CRIMSON_SHADER_INTERFACE_H

#pragma once

#include "ShaderProgram.h"

#include "Uniform.h"
#include "ShaderUniforms.h"
#include "SmartPointer.h"

class CShader
{
friend class CPass;
public:
	~CShader() {};

	void operator= (CShader& other);

	bool LoadVP(const char* pchShaderName);
	bool LoadVPFromText(const char* pchShaderName,const char* pchShaderText);

	bool LoadFP(const char* pchShaderName);
	bool LoadFPFromText(const char* pchShaderName,const char* pchShaderText);

	CSmartPointer<IShaderProgram> GetProgram(void) const { return m_spShaderProgram; };

	bool Bind(DWORD dwProgType) const;
	void Unbind(void) const;

	bool AddUniform(const char* pchName,tUniform* pAU); 

protected:
	CShader() {m_pShaderUniforms = NULL;};
	CSmartPointer<IShaderProgram> m_spShaderProgram;
	CShaderUniforms* m_pShaderUniforms;

	
};


#endif