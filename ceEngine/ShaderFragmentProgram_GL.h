#ifndef CRIMSON_SHADER_FRAGMENT_PROGRAM_GL_H
#define CRIMSON_SHADER_FRAGMENT_PROGRAM_GL_H

#pragma once

#include "cgShaderProgram.h"

#include <windows.h>
#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_Context;

#include "ShaderUniforms.h"

class CShaderFragmentProgram_GL : public CCGShaderProgram
{
friend class CGraphicsAPI_GL;
public:
	
	~CShaderFragmentProgram_GL();

	bool LoadFromText(const char* pchShaderName,const char* pchShaderText);

	bool Load(const char* pchShaderName);
	bool Bind(CShaderUniforms* pShaderUniforms) const;
	bool RebindParameters(CShaderUniforms* pShaderUniforms) const;

	void Unbind(void) const;

	void Reload(void);

	
	const char* GetName(void) const { return m_pcName;};

protected:
	CShaderFragmentProgram_GL();
	char* m_pcName;
};

#endif