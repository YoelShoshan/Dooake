#ifndef CRIMSON_SHADER_VERTEX_PROGRAM_D3D_H
#define CRIMSON_SHADER_VERTEX_PROGRAM_D3D_H

#pragma once

#include "cgShaderProgram.h"

#include <windows.h>
#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_cgContext;

#include "ShaderUniforms.h"


class CShaderVertexProgram_D3D : public CCGShaderProgram
{
friend class CGraphicsAPI_D3D;
public:
	
	~CShaderVertexProgram_D3D();

	bool LoadFromText(const char* pchShaderName,const char* pchShaderText);
	bool Load(const char* pchShaderName);
	bool Bind(CShaderUniforms* pShaderUniforms) const;
	bool RebindParameters(CShaderUniforms* pShaderUniforms) const;

	void Unbind(void) const;


	void Reload(void);

	const char* GetName(void) const { return m_pcName;};

protected:
	CShaderVertexProgram_D3D();
	char* m_pcName;

};

#endif