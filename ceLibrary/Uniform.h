#ifndef CRIMSON_UNIFORM_H
#define CRIMSON_UNIFORM_H

#pragma once

#include "ce.h"
#include <Windows.h>

struct tUniform
{
private:
	tUniform() {};
public:
	tUniform(DWORD dwType);
	
	char* m_pchName;
	DWORD m_dwType;
	float *m_pfValue;
};

#endif