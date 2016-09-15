#ifndef CRIMSON_OPENGL_EXTENSIONS_H
#define CRIMSON_OPENGL_EXTENSIONS_H

#pragma once

#include "Defines.h"
#include "windows.h"
#include <gl\gl.h>	
#include <gl\glu.h>	
#include "glext.h"


class COpenGLExtensions
{
public:
	COpenGLExtensions();
	~COpenGLExtensions();

	bool Init(void);

	bool MultiTexturingPresent(void) { return m_bMultiTexturing;};
	int  GetTexUnitsNum(void) { return m_iTexUnitsNum;};
	bool VBOPresent(void) { return m_bVBO;};

private:
	bool m_bMultiTexturing;
	int  m_iTexUnitsNum;
	bool m_bVBO;
};
	


#endif