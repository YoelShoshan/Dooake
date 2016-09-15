#include "stdafx.h"
#include "RenderToTexture_GL.h"

#include "Singleton.h"
#include <gl/GL.h>
//#include <gl/GLAux.h>

#include "glext.h"
#include <string>
#include <assert.h>

#include "ce.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;



///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////////////////////////////////////////
// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
#include "TextureManager.h"



void CRenderToTexture_GL::Init(const char* pName,  DWORD dwType,int iWidth, int iHeight)
{

	// debug
	GLenum t_OpenGLError = glGetError();
	char* cpTempError = (char*) gluErrorString(t_OpenGLError);

	m_dwType = dwType;

	

	m_iWidth=iWidth;
	m_iHeight=iHeight;
	
	m_pTexName = new char[strlen(pName)+1];
	strcpy(m_pTexName,pName);



	

	// color texture / renderbuffer
	

	if (m_dwType==CR_RTT_SHADOW_MAP)
	{	
		char* cpTempError = NULL;
				
		//g_pLogFile->OutPutPlainText("generating tex",LOG_MESSAGE_INFORMATIVE);
		glGenTextures(1, &m_uiDepthRBID);
		GLenum error = glGetError();
		if (error!=GL_NO_ERROR)
		{
			cpTempError = (char*) gluErrorString(error);
			g_pLogFile->OutPutPlainText(cpTempError,LOG_MESSAGE_INFORMATIVE);
		}

		//g_pLogFile->OutPutPlainText("binding tex",LOG_MESSAGE_INFORMATIVE);
		glBindTexture(GL_TEXTURE_2D, m_uiDepthRBID);
		error = glGetError();
		if (error!=GL_NO_ERROR)
		{
			cpTempError = (char*) gluErrorString(error);
			g_pLogFile->OutPutPlainText(cpTempError,LOG_MESSAGE_INFORMATIVE);
		}


		//g_pLogFile->OutPutPlainText("glTexImage2D - depth component tex",LOG_MESSAGE_INFORMATIVE);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_iWidth, m_iHeight, 0,
					GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_iWidth, m_iHeight, 0,
					GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		

		error = glGetError();
		if (error!=GL_NO_ERROR)
		{
			cpTempError = (char*) gluErrorString(error);
			g_pLogFile->OutPutPlainText(cpTempError,LOG_MESSAGE_INFORMATIVE);
		}

		//g_pLogFile->OutPutPlainText("setting tex parameters",LOG_MESSAGE_INFORMATIVE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		error = glGetError();
		if (error!=GL_NO_ERROR)
		{
			cpTempError = (char*) gluErrorString(t_OpenGLError);
			g_pLogFile->OutPutPlainText(cpTempError,LOG_MESSAGE_INFORMATIVE);
		}
		
		
	}
	else	//if (m_dwType==CR_RTT_COLOR_AND_DEPTH)  // default
	{
		glGenFramebuffersEXT(1, &m_uiFboID);
		glGenTextures(1, &m_uiTexID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_uiFboID);

		glBindTexture(GL_TEXTURE_2D, m_uiTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


		// restore for no mip maps
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glGenerateMipmapEXT(GL_TEXTURE_2D);


		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,	GL_TEXTURE_2D, m_uiTexID, 0);
	
		/*glGenRenderbuffersEXT(1, &m_uiDepthRBID);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_uiDepthRBID);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_iWidth, m_iHeight);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
		GL_RENDERBUFFER_EXT, m_uiDepthRBID);*/


		GLenum status=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


		switch(status)
		{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;                            

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			BREAKPOINT(1);
			break;                            

		default:                            
			/* programming error; will fail on all hardware */   
			BREAKPOINT(1);
		}
	}
	
	
			

	


	

}



void CRenderToTexture_GL::BindRenderTo(/*int iSamplerNum*/) const
{
	if (m_dwType==CR_RTT_SHADOW_MAP)
	{		
		//g_pLogFile->OutPutPlainText("Shadow::BindRenderTo:: enter",LOG_MESSAGE_INFORMATIVE);
		
		// restore restore 
		glDrawBuffer(GL_NONE);	
		glReadBuffer(GL_NONE);			

		g_pGraphicsAPI->SetViewport(0,0,m_iWidth,m_iHeight);

		//g_pLogFile->OutPutPlainText("Shadow::BindRenderTo:: clearing depth",LOG_MESSAGE_INFORMATIVE);
		g_pGraphicsAPI->Clear(CR_CLEAR_DEPTH);
		//g_pLogFile->OutPutPlainText("Shadow::BindRenderTo:: exit",LOG_MESSAGE_INFORMATIVE);

		

	}
	else //if (m_dwType==CR_RTT_COLOR_AND_DEPTH)  // default
	{
		glBindTexture(GL_TEXTURE_2D,0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_uiFboID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		
		g_pGraphicsAPI->SetViewport(0,0,m_iWidth,m_iHeight);
	
		g_pGraphicsAPI->SetClearColor(0.f,0.f,0.f,0.f);
		g_pGraphicsAPI->Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);
	}

	
	

}

void CRenderToTexture_GL::UnbindRenderTo() const
{

	if (m_dwType==CR_RTT_SHADOW_MAP)
	{

		g_pGraphicsAPI->SetViewport(0,0,m_iWidth,m_iHeight);
		glBindTexture(GL_TEXTURE_2D, m_uiDepthRBID);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_iWidth, m_iHeight);
		g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
		// restore default render target
		glDrawBuffer(GL_BACK);	
		glReadBuffer(GL_BACK);	
		g_pGraphicsAPI->Clear(CR_CLEAR_DEPTH);
	}
	else//if (m_dwType==CR_RTT_COLOR_AND_DEPTH)  // default
	{
		// unbind current Frame Buffer Object
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		// restore default render target
		glDrawBuffer(GL_BACK);	

		glBindTexture(GL_TEXTURE_2D,m_uiTexID);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	
	}


	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// want depths only


	

}

void CRenderToTexture_GL::Bind(int iSamplerNum) const
{
	if (m_dwType==CR_RTT_SHADOW_MAP) 
	{
		
		//g_pLogFile->OutPutPlainText("Shadow::Bind:: enter",LOG_MESSAGE_INFORMATIVE);

		//g_pLogFile->OutPutPlainText("Shadow::Bind:: binding depth texture",LOG_MESSAGE_INFORMATIVE);
		glBindTexture(GL_TEXTURE_2D,m_uiDepthRBID);

		//g_pLogFile->OutPutPlainText("Shadow::Bind:: exit",LOG_MESSAGE_INFORMATIVE);
	}
	else
		glBindTexture(GL_TEXTURE_2D,m_uiTexID);

	
}

/*void CRenderToTexture_GL::Unbind(int iSamplerNum) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB+iTexUnit);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(m_iTextureType,0);
}*/

const char* CRenderToTexture_GL::GetName(void) const
{
	return m_pTexName;
}
