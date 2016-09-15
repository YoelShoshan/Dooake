#ifndef CRIMSON_SHADER_FRAGMENT_PROGRAM_D3D_H
#define CRIMSON_SHADER_FRAGMENT_PROGRAM_D3D_H

#pragma once

#include "cgShaderProgram.h"

#include <windows.h>
#include "Cg/cg.h"
#include "Cg/cgD3D9.h"
extern CGcontext g_Context;

#include "ShaderUniforms.h"

class CShaderFragmentProgram_D3D : public CCGShaderProgram
{
friend class CGraphicsAPI_D3D;
public:
	
	~CShaderFragmentProgram_D3D();

	bool Load(const char* pchShaderName);

	bool LoadFromText(const char* pchShaderName,const char* pchShaderText);

	bool Bind(CShaderUniforms* pShaderUniforms) const;	

	bool RebindParameters(CShaderUniforms* pShaderUniforms) const;

	void Unbind(void) const;

	void Reload(void);

	


	const char* GetName(void) const { return m_pcName;};

protected:
	CShaderFragmentProgram_D3D();
	char* m_pcName;

};

#endif