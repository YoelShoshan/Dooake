#ifndef CRIMSON_AUTO_UNIFORMS_H
#define CRIMSON_AUTO_UNIFORMS_H

#pragma once

#include "ce.h"
#include <map>
#include <string>

#include "Uniform.h"

typedef std::map<std::string,tUniform*> UNIFORMS_MAP;
typedef UNIFORMS_MAP::iterator     UNIFORMS_MAP_IT;

class CShaderUniforms
{
public:
	CShaderUniforms();
	~CShaderUniforms();

	bool Add(const char* pchName,tUniform* pAU); 
	tUniform* Get(const char* pchName);

	void     SetStart(void);
	bool     IsEnd(void);
	void     Next(void);
	tUniform* GetCurrent(void);
	

private:
	

	UNIFORMS_MAP    m_Uniforms;
	UNIFORMS_MAP_IT m_Uniforms_it;
};




#endif