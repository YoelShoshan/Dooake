#include "stdafx.h"
#include "ShaderVertexProgram_D3D.h"
#include "Defines.h"

#include <cg/cg.h>
#include <cg/cgD3D9.h>

#include "LogFile.h"

#include <io.h>


extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;

extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;

#include <assert.h>

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "ShaderProgramManager.h"

#include "ce.h"

CShaderVertexProgram_D3D::CShaderVertexProgram_D3D()
{
	m_Program=NULL;
	m_pcName = NULL;
}

CShaderVertexProgram_D3D::~CShaderVertexProgram_D3D()
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



void CShaderVertexProgram_D3D::Reload(void)
{
	assert(0); // since load from text was added, this is no longer valid, since shader text was lost
	if (m_Program)
		cgDestroyProgram(m_Program);

	Load(m_pcName);
}



bool CShaderVertexProgram_D3D::LoadFromText(const char* pchShaderName,const char* pchShaderText)
{
	m_Program = cgCreateProgram(g_cgContext,CG_SOURCE,pchShaderText,g_VertexProfile,"main",NULL);


	if (!cgIsProgramCompiled(m_Program))
		cgCompileProgram(m_Program);

	char message[400];
	sprintf(message,"Loading Vertex Program: %s",pchShaderName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);

	HRESULT hr=cgD3D9LoadProgram(m_Program,false,0);
	if (!SUCCEEDED(hr))
	{
		g_pLogFile->OutPutPlainText("CShaderVertexProgram_D3D::Load() Creation of Vertex Program Failed!",LOG_MESSAGE_ERROR);
		return false;
	}

	m_pcName = new char[strlen(pchShaderName)+1];
	strcpy(m_pcName,pchShaderName);


	const char *pchListing = cgGetLastListing(g_cgContext);  

	ParseParameters();

	return true;
}

bool CShaderVertexProgram_D3D::Load(const char* pchShaderName)
{

	/*const char* profileOpts[] = 
	{
	"-profileopts", "dcls", NULL,
	};*/

	/*
	CGDLL_API CGprogram cgCreateProgram(CGcontext ctx, 
	CGenum program_type,
	const char *program,
	CGprofile profile,
	const char *entry,
	const char **args);
	*/

	FILE* pFile;
	if ((pFile=fopen (pchShaderName, "rb"))== NULL)
	{
		g_pLogFile->OutPutPlainText("CShaderVertexProgram_D3D::Load() Vertex Program File Not Found!",LOG_MESSAGE_ERROR);
		return false;		
	}

	//////////////////////////////////////////////////
	// Add a define of CE_TEX_2D for d3d vs. opengl //
	char tempDefine[75];
	sprintf(tempDefine,"#define PLACE_HOLDER(x,y) tex2D(x,y)\n");
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

bool CShaderVertexProgram_D3D::RebindParameters(CShaderUniforms* pShaderUniforms) const
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
			sprintf(message,"CShaderVertexProgram_D3D::cgParameter %s wasn't found in vertex program!",pUniform->m_pchName);
			g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_WARNING_LVL_1);
		}
		else // if it WAS found
			cgD3D9SetUniform(cgParam,pUniform->m_pfValue);			

		pShaderUniforms->Next();
	}


	//const char *pchListing = cgGetLastListing(g_cgContext);  


	return true;

}

bool CShaderVertexProgram_D3D::Bind(CShaderUniforms* pShaderUniforms) const
{

	 cgD3D9BindProgram( m_Program );

	 
	 RebindParameters(pShaderUniforms);
	 
		

	return true;

}

void CShaderVertexProgram_D3D::Unbind(void) const
{
	// should i do something here?
}