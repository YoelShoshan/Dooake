#ifndef CRIMSON_SHADER_PROGRAM_INTERFACE_H
#define CRIMSON_SHADER_PROGRAM_INTERFACE_H

#pragma once

#include "Cg/cg.h"

#include "ShaderUniforms.h"

#include "ReferenceCount.h"

#include "VertexBuffer.h"

#define VERTEX_SHADER_PROGRAM 0
#define FRAGMENT_SHADER_PROGRAM 1

#include "VertexDecleration.h"

#include "ShaderProgram_IE.h"


/////////////////////////////////////////////////////////////////
//
// Important TODO:
//
// Create a REAL vd, that will be used later in the pass to bind
//////////////////////////////////////////////////////////////////
class IShaderProgram : public CReferenceCount, public IShaderProgram_Export
{
public:
	IShaderProgram() {m_pVD = NULL;};
	virtual ~IShaderProgram() {};

	

	virtual void ParseParameters(void) = 0;
	virtual void ParseParameter(CGparameter param) = 0;

	

	virtual const char* GetName(void) const = 0;

	const CVertDecl* GetVD(void) const { return m_pVD;};

	// remove this!
	//const tMaxVD* GetMaxVD(void) const { return &m_MaxVD;};

protected:
	CGprogram m_Program;	
	// remove this!
	//tMaxVD m_MaxVD;
	CVertDecl* m_pVD;
};




#endif
