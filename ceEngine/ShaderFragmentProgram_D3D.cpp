#include "stdafx.h"
#include "ShaderFragmentProgram_D3D.h"
#include "Defines.h"

#include "Cg/cg.h"
#include "Cg/cgD3D9.h"
extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;


#include <io.h>

#include <assert.h>
#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "ShaderProgramManager.h"

#include "ce.h"

CShaderFragmentProgram_D3D::CShaderFragmentProgram_D3D()
{
	m_Program=NULL;
	m_pcName = NULL;
}

CShaderFragmentProgram_D3D::~CShaderFragmentProgram_D3D()
{
	char message[400];
	sprintf(message,"Deleting Fragment Program - %s...",m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
	sprintf(message,"Deleting Fragment Program - %s DONE!",m_pcName);

	st_ShaderProgramManager->Unregister(m_pcName);

	if (m_Program)
		cgDestroyProgram(m_Program);

	SafeDeleteArray(m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

void CShaderFragmentProgram_D3D::Reload(void)
{
	assert(0); // since load from text was added, this is no longer valid, since shader text was lost
	if (m_Program)
		cgDestroyProgram(m_Program);

	Load(m_pcName);
}

bool CShaderFragmentProgram_D3D::LoadFromText(const char* pchShaderName, const char* pchShaderText)
{
	//m_Program = cgCreateProgram(g_cgContext,CG_SOURCE,pchShaderText,g_FragmentProfile,"main",NULL);

	m_Program = cgCreateProgram(g_cgContext,CG_SOURCE,pchShaderText,g_FragmentProfile,"main",NULL);

	/*m_Program = cgCreateProgramFromFile( g_cgContext,
	CG_SOURCE,
	pchShaderName,
	g_FragmentProfile,
	"main",
	NULL );*/

	//debug - remove
	//if (!cgIsProgramCompiled(m_Program))

	CGbool res = cgIsProgramCompiled(m_Program);

	//cgSetProgramProfile(m_Program,

	//if (CG_TRUE!=res)
		cgCompileProgram(m_Program);

	const char* pCompiledProgStr = cgGetProgramString(m_Program,CG_COMPILED_PROGRAM);

	//OutputDebugStringA("Compiled:\n");
	//OutputDebugStringA(pCompiledProgStr);
	//OutputDebugStringA("---Compiled Program End---\n");

	char message[400];
	sprintf(message,"Loading Fragment Program: %s",pchShaderName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);

	HRESULT hr=cgD3D9LoadProgram(m_Program,false,0);
	if (!SUCCEEDED(hr))
	{
		g_pLogFile->OutPutPlainText("CShaderFragmentProgram_D3D::Load() Creation of Fragment Program Failed!",LOG_MESSAGE_ERROR);
		return false;
	}

	m_pcName = new char[strlen(pchShaderName)+1];
	strcpy(m_pcName,pchShaderName);


	const char *pchListing = cgGetLastListing(g_cgContext);  

	ParseParameters();

	return true;
}

bool CShaderFragmentProgram_D3D::Load(const char* pchShaderName)
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
		g_pLogFile->OutPutPlainText("CShaderFragmentProgram_D3D::Load() Fragment Program File Not Found!",LOG_MESSAGE_ERROR);
		return false;		
	}

	//////////////////////////////////////////////////
	// Add a define of CE_TEX_2D for d3d vs. opengl //
	char tempDefine[75];
	sprintf(tempDefine,"#define CE_TEX_2D(x,y) tex2D(x,y)\n");
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

bool CShaderFragmentProgram_D3D::RebindParameters(CShaderUniforms* pShaderUniforms) const
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
			sprintf(message,"CShaderFragmentProgram_D3D::cgParameter %s wasn't found in fragment program!",pUniform->m_pchName);
			g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_WARNING_LVL_1);
		}
		else // if it WAS found
			cgD3D9SetUniform(cgParam,pUniform->m_pfValue);			

		pShaderUniforms->Next();
	}

	return true;

}

bool CShaderFragmentProgram_D3D::Bind(CShaderUniforms* pShaderUniforms) const
{
	cgD3D9BindProgram( m_Program );

	RebindParameters(pShaderUniforms);
	


	//const char *pchListing = cgGetLastListing(g_cgContext);  


	
			
	return true;

}

void CShaderFragmentProgram_D3D::Unbind(void) const
{
	// should i do something here?
}