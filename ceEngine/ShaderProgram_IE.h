#ifndef CRIMSON_SHADER_PROGRAM_INTERFACE_EXPORT_H
#define CRIMSON_SHADER_PROGRAM_INTERFACE_EXPORT_H

#pragma once


#include "ShaderUniforms.h"

/////////////////////////////////////////////////////////////////
//
// Important TODO:
//
// Create a REAL vd, that will be used later in the pass to bind
//////////////////////////////////////////////////////////////////
class IShaderProgram_Export
{
public:
	IShaderProgram_Export() {};
	virtual ~IShaderProgram_Export() {};

	virtual bool LoadFromText(const char* pchShaderName, const char* pchShaderText) = 0;

	virtual bool Load(const char* pchShaderName) = 0;
	virtual bool Bind(CShaderUniforms* pShaderUniforms) const = 0;

	virtual bool RebindParameters(CShaderUniforms* pShaderUniforms) const = 0;

	virtual void Unbind(void) const = 0 ;
	
	virtual void Reload(void) = 0 ;

	virtual const char* GetName(void) const = 0;
};




#endif
