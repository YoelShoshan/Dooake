#include "stdafx.h"
#include "ShaderVertexProgram_GL.h"
#include "Defines.h"

#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;

#include "LogFile.h"
#include "GMath.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "ShaderProgramManager.h"

#include "Profiler.h"

#include "ce.h"

#include "io.h"

CShaderVertexProgram_GL::CShaderVertexProgram_GL()
{
	m_Program=NULL;
	m_pcName = NULL;
}

CShaderVertexProgram_GL::~CShaderVertexProgram_GL()
{
	char message[400];
	sprintf_s(message,400,"Deleting Vertex Program - %s...",m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
	sprintf_s(message,400,"Deleting Vertex Program - %s DONE!",m_pcName);

		st_ShaderProgramManager->Unregister(m_pcName);

	if (m_Program)
		cgDestroyProgram(m_Program);

	SafeDeleteArray(m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

void CShaderVertexProgram_GL::Reload(void)
{
	assert(0); // since load from text was added, this is no longer valid, since shader text was lost
	if (m_Program)
		cgDestroyProgram(m_Program);

	Load(m_pcName);
}

bool CShaderVertexProgram_GL::LoadFromText(const char* pchShaderName,const char* pchShaderText)
{
	m_Program = cgCreateProgram(g_cgContext,CG_SOURCE,pchShaderText,g_VertexProfile,"main",NULL);

	/*m_Program = cgCreateProgramFromFile(g_cgContext, CG_SOURCE,pchShaderName,
	g_VertexProfile, "main", 0);*/

	if (!m_Program)
	{
		//TODO: make this a single line
		g_pLogFile->OutPutPlainText("CShaderVertexProgram_GL::Load() Creation of Vertex Program Failed!",LOG_MESSAGE_ERROR);
		g_pLogFile->OutPutPlainText(pchShaderName,LOG_MESSAGE_ERROR);

		return false;
	}

	char message[400];
	sprintf(message,"Loading Vertex Program: %s",pchShaderName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);


	m_pcName = new char[strlen(pchShaderName)+1];
	strcpy(m_pcName,pchShaderName);

	if (!cgIsProgramCompiled(m_Program))
		cgCompileProgram(m_Program);

	cgGLEnableProfile(g_VertexProfile); 
	cgGLLoadProgram(m_Program);

	const char *pchListing = cgGetLastListing(g_cgContext);  

	ParseParameters();

	return true;
}

bool CShaderVertexProgram_GL::Load(const char* pchShaderName)
{
	FILE* pFile;
	if ((pFile=fopen (pchShaderName, "rb"))== NULL)
	{
		g_pLogFile->OutPutPlainText("CShaderVertexProgram_GL::Load() Vertex Program File Not Found!",LOG_MESSAGE_ERROR);
		return false;		
	}

	char tempDefine[500];	
	sprintf(tempDefine,"#define CE_TEX_2D(a,b) tex2D(a,float2((b).x,1-(b).y))\n");
	int iDefineLength = strlen(tempDefine);

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iDefineLength+iFileLength+1];
	strcpy(pText,tempDefine);
	fread(pText+iDefineLength,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength+iDefineLength] = NULL;


	bool bRet = LoadFromText(pchShaderName,pText);

	SafeDeleteArray(pText);
	return bRet;
}

bool CShaderVertexProgram_GL::RebindParameters(CShaderUniforms* pShaderUniforms) const
{
	if (!pShaderUniforms)
		return true;
	
	pShaderUniforms->SetStart();

	CGparameter cgParam;
	tUniform*   pUniform=NULL;

	while (!pShaderUniforms->IsEnd())
	{		
		pUniform = pShaderUniforms->GetCurrent();
		BREAKPOINT(!pUniform);
		cgParam = cgGetNamedParameter(m_Program, pUniform->m_pchName);
		
		if (!cgParam) // didn't find the param in the program
		{
			char message[200];
			sprintf_s(message,200,"CShaderVertexProgram_GL::cgParameter %s wasn't found in fargment program!",pUniform->m_pchName);
			g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_WARNING_LVL_1);
		}
		else // if it WAS found
		{
			if ( pUniform->m_dwType == CR_FLOAT1 )
				cgGLSetParameter1fv(cgParam,pUniform->m_pfValue);
			else
			if ( pUniform->m_dwType == CR_FLOAT2 )
				cgGLSetParameter2fv(cgParam,pUniform->m_pfValue);
			else
			if ( pUniform->m_dwType == CR_FLOAT3 )
				cgGLSetParameter3fv(cgParam,pUniform->m_pfValue);
			else
			if ( pUniform->m_dwType == CR_FLOAT4 )
				cgGLSetParameter4fv(cgParam,pUniform->m_pfValue);
			else
			if ( pUniform->m_dwType == CR_FLOAT4X4 )
				cgGLSetMatrixParameterfr(cgParam,pUniform->m_pfValue);
			else
				g_pLogFile->OutPutPlainText("CShaderVertexProgram_GL::Uniform was of unrecognized type",LOG_MESSAGE_WARNING_LVL_1);		
		}

		pShaderUniforms->Next();
	}


	

	//const char *pchListing = cgGetLastListing(g_cgContext);  

/*	 CGerror err = cgGetError();

  if (err != CG_NO_ERROR)
   {
     printf("CG error: %s\n", cgGetErrorString(err));
     exit(1);
   }*/

	return true;

}

bool CShaderVertexProgram_GL::Bind(CShaderUniforms* pShaderUniforms) const
{	
	static CProfileStep step("VS:");	
	step.Begin();


	cgGLBindProgram(m_Program);
	cgGLEnableProfile(g_VertexProfile); 

	
	RebindParameters(pShaderUniforms);

	step.End();
	
	return true;
}

void CShaderVertexProgram_GL::Unbind(void) const
{
	cgGLDisableProfile(g_VertexProfile);
}