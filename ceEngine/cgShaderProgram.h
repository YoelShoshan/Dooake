#ifndef CRIMSON_CG_SHADER_PROGRAM_H
#define CRIMSON_CG_SHADER_PROGRAM_H

#pragma once

#include "ShaderProgram.h"

class CCGShaderProgram : public IShaderProgram
{
public:
	CCGShaderProgram() {};
	~CCGShaderProgram() {};


	void ParseParameters(void);
	void ParseParameter(CGparameter param);

	//const tMaxVD* GetMaxVD(void) const { return &m_MaxVD;};

};













#endif