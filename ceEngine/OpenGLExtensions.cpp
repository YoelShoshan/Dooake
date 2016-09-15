#include "stdafx.h"
#include "OpenGLExtensions.h"

#include "ce.h"

COpenGLExtensions::COpenGLExtensions()
{
	m_bMultiTexturing=false;
	m_bVBO=false;
	m_iTexUnitsNum=0;
}

COpenGLExtensions::~COpenGLExtensions()
{

}

bool COpenGLExtensions::Init(void)
{
	#include "glext_load.inl" 
	
	

	//wglGetExtensionsStringARB(extensions);

	char* extensionsList = (char*) glGetString(GL_EXTENSIONS);

	// Here we make sure that the functions were loaded properly
	if(!glActiveTextureARB || !glClientActiveTextureARB)
	{
		m_bMultiTexturing = false;
		m_iTexUnitsNum = 1;
		CR_ERROR(1,"Card not supporting MultiTexturing!");
		return false;
	}
	else
	{		
		m_bMultiTexturing = true;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_iTexUnitsNum);
	}

		
	if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB || !glDeleteBuffersARB)
	{
		m_bVBO = false;
		CR_ERROR(1,"Card not supporting VBO!");
		return false;
	}
	else
		m_bVBO = true;


	//glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	
	/*glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFrameBufferEXT");

	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");

	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");

	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");

	glBindRenderbufferEXT =  (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");

	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");

	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");


	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");*/



	return true;

}