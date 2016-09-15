#include "stdafx.h"
#include "ShaderFragmentProgram_GL.h"
#include "Defines.h"

#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "io.h"

#include "ShaderProgramManager.h"
#include "ce.h"
#include "Profiler.h"

CShaderFragmentProgram_GL::CShaderFragmentProgram_GL()
{
	m_Program=NULL;
	m_pcName=NULL;
	
}

CShaderFragmentProgram_GL::~CShaderFragmentProgram_GL()
{
	char message[400];
	sprintf_s(message,400,"Deleting Fragment Program - %s...",m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
	sprintf_s(message,400,"Deleting Fragment Program - %s DONE!",m_pcName);

		st_ShaderProgramManager->Unregister(m_pcName);

	if (m_Program)
		cgDestroyProgram(m_Program);

	SafeDeleteArray(m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

void CShaderFragmentProgram_GL::Reload(void)
{
	assert(0); // since load from text was added, this is no longer valid, since shader text was lost
	if (m_Program)
		cgDestroyProgram(m_Program);

	Load(m_pcName);

}


bool CShaderFragmentProgram_GL::LoadFromText(const char* pchShaderName,const char* pchShaderText)
{
	m_Program = cgCreateProgram(g_cgContext,CG_SOURCE,pchShaderText,g_FragmentProfile,"main",NULL);

	/*m_Program = cgCreateProgramFromFile(g_cgContext, CG_SOURCE,pchShaderName,
	g_FragmentProfile, "main", 0);*/

	if (!m_Program)
	{
		//TODO: make this a single line
		g_pLogFile->OutPutPlainText("CShaderFragmentProgram_GL::Load() Creation of Fragment Program Failed!",LOG_MESSAGE_ERROR);
		g_pLogFile->OutPutPlainText(pchShaderName,LOG_MESSAGE_ERROR);

		return false;
	}

	char message[400];
	sprintf(message,"Loading Fragment Program: %s",pchShaderName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);


	m_pcName = new char[strlen(pchShaderName)+1];
	strcpy(m_pcName,pchShaderName);

	if (!cgIsProgramCompiled(m_Program))
		cgCompileProgram(m_Program);

	cgGLEnableProfile(g_FragmentProfile); 
	cgGLLoadProgram(m_Program);

	const char *pchListing = cgGetLastListing(g_cgContext);  

	ParseParameters();

	return true;
}

bool CShaderFragmentProgram_GL::Load(const char* pchShaderName)
{

	FILE* pFile;
	if ((pFile=fopen (pchShaderName, "rb"))== NULL)
	{
		g_pLogFile->OutPutPlainText("CShaderFragmentProgram_GL::Load() Fragment Program File Not Found!",LOG_MESSAGE_ERROR);
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
	return true;

}

bool CShaderFragmentProgram_GL::RebindParameters(CShaderUniforms* pShaderUniforms) const
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
			sprintf(message,"CShaderFragmentProgram_GL::cgParameter %s wasn't found in fargment program!",pUniform->m_pchName);
			g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_WARNING_LVL_1);
		}
		else // if it WAS found
		{
			//cgD3D9SetUniform(cgParam,pUniform->m_pfValue);			

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
				g_pLogFile->OutPutPlainText("CShaderFragmentProgram_GL::Uniform was of unrecognized type",LOG_MESSAGE_WARNING_LVL_1);		
		}

		pShaderUniforms->Next();
	}

	

	//const char *pchListing = cgGetLastListing(g_cgContext);  
	return true;


}

bool CShaderFragmentProgram_GL::Bind(CShaderUniforms* pShaderUniforms) const
{	

	static CProfileStep step("FS:");	
	step.Begin();

	cgGLEnableProfile(g_FragmentProfile); 
	cgGLBindProgram(m_Program);
	RebindParameters(pShaderUniforms);
		

	step.Begin();
	
	return true;

}

void CShaderFragmentProgram_GL::Unbind(void) const
{
	cgGLDisableProfile(g_FragmentProfile);
}