#ifndef CRIMSON_SHADER_VERTEX_PROGRAM_GL_H
#define CRIMSON_SHADER_VERTEX_PROGRAM_GL_H

#pragma once

#include "cgShaderProgram.h"

#include <windows.h>
#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_cgContext;

#include "ShaderUniforms.h"

class CShaderVertexProgram_GL : public CCGShaderProgram
{
friend class CGraphicsAPI_GL;
public:
	
	~CShaderVertexProgram_GL();


	bool LoadFromText(const char* pchShaderName,const char* pchShaderText);
	bool Load(const char* pchShaderName);
	bool Bind(CShaderUniforms* pShaderUniforms) const;

	bool RebindParameters(CShaderUniforms* pShaderUniforms) const;

	void Unbind() const;

	void Reload(void);



	const char* GetName(void) const { return m_pcName;};

private:
	CShaderVertexProgram_GL();
	char* m_pcName;
};

#endif