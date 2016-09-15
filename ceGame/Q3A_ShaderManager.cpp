#include "stdafx.h"
#include "Q3A_ShaderManager.h"

#include <GL/gl.h>
#include "Singleton.h"
#include "Timer_IE.h"
#include "ShaderParser.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

CShaderParser g_ShaderParser;

/////////////////////////////////////
//
//  CQ3A_Shader class implementation
//
/////////////////////////////////////

CQ3A_Shader::CQ3A_Shader()
{
	
	m_Data.m_dwShaderFlags = FLAG_CULLFRONT; // default flags
	m_Data.m_dwSurfaceParm = SURFACEPARM_NONE;

	m_Data.m_uiShaderSort=SORT_NOT_INIT; //default sort
	//m_cpShaderName="";  // RESTORE THIS RESTORE THIS
	m_Data.m_cpShaderName = NULL;

	m_Data.m_bAllBlended = false;

	m_Data.m_bDVUsed = false;

	m_Data.m_bFarBoxUsed=false;


	m_Data.m_uiPassesNum = 0;
	m_Data.m_ppPasses        = new CQ3A_ShaderPass*[MAX_PASS_COUNT]; // TODO: Maybe make this more dynamic?

	for (int i=0;i<MAX_PASS_COUNT;i++)
		m_Data.m_ppPasses[i] = NULL;
}

CQ3A_Shader::~CQ3A_Shader()
{

	// DEBUG DEBUG DEBUG
	//m_Data.m_cpShaderName = NULL;

	// DEBUG DEBUG DEBUG
	//m_Data.m_ppPasses = NULL;
	//
	st_Q3A_ShaderManager->Unregister(m_Data.m_cpShaderName);
	//
	for (int i=0;i<m_Data.m_uiPassesNum;i++)
		SafeDelete(m_Data.m_ppPasses[i]);

	SafeDeleteArray(m_Data.m_ppPasses);


	SafeDeleteArray(m_Data.m_cpShaderName);
}


/*
CQ3A_ShaderPass* CQ3A_Shader::GetPass(UINT iPass)
{

	if (iPass>=MAX_PASS_COUNT) return NULL;
		return m_Data.m_ppPasses[iPass]; 
}
*/


void CQ3A_Shader::AddPass(CQ3A_ShaderPass* pSP)
{	
	m_Data.m_ppPasses[m_Data.m_uiPassesNum++] = pSP;
}

void CQ3A_Shader::ChangePass(CQ3A_ShaderPass* pSP,int iPassNum)
{
	m_Data.m_ppPasses[iPassNum] = pSP;
	if (iPassNum+1 > m_Data.m_uiPassesNum)
		m_Data.m_uiPassesNum = iPassNum+1;
}




void CQ3A_Shader::SetName(const char* cpName)
{
	m_Data.m_cpShaderName = new char[strlen(cpName)+1];
	strcpy(m_Data.m_cpShaderName,cpName);
}

char* CQ3A_Shader::GetName()
{
	return m_Data.m_cpShaderName; 
}

void CQ3A_Shader::SetShaderFlags(DWORD dwShaderFlags)
{
	m_Data.m_dwShaderFlags = dwShaderFlags;
}

DWORD CQ3A_Shader::GetShaderFlags(void)
{
	return m_Data.m_dwShaderFlags;
}

void CQ3A_Shader::SetShaderSort(UINT uiShaderSort)
{
	m_Data.m_uiShaderSort = uiShaderSort;
}

UINT CQ3A_Shader::GetShaderSort(void)
{
	return m_Data.m_uiShaderSort;
}

void CQ3A_Shader::AddDeformVertexesMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_Data.m_bDVUsed=true;
	m_Data.m_DVObject.AddMove(x,y,z,uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CQ3A_Shader::AddDeformVertexesBulge(float f1,float f2,float f3)
{
	m_Data.m_bDVUsed=true;
	m_Data.m_DVObject.AddBulge(f1,f2,f3);
}
void CQ3A_Shader::AddDeformVertexesWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_Data.m_bDVUsed=true;
	m_Data.m_DVObject.AddWave(fDiv,uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CQ3A_Shader::AddDeformVertexesAutoSprite(void)
{
	m_Data.m_bDVUsed=true;
	m_Data.m_DVObject.AddAutoSprite();
}




/*int CQ3A_Shader::GetPassCount()
{
	return m_Data.m_uiPassesNum; 
}*/



////////////////////////////////////
// Shaders Manager
///////////////////////////////////

CQ3A_ShaderManager::CQ3A_ShaderManager()
{
}

CQ3A_ShaderManager::~CQ3A_ShaderManager()
{	

}


/*CQ3A_Shader* CQ3A_ShaderManager::GetShader(UINT uiShaderID)
{
	if (uiShaderID<m_Data.m_uiShadersNum)
		return m_Data.m_ppShaders[uiShaderID];
	else
		return m_Data.m_ppShaders[0]; ////////// JUST FOR DEBUG
}*/



//////////////////////////////////////////////
// if doesn't exist already Push a new CQ3A_Shader
//////////////////////////////////////////////

/*UINT CQ3A_ShaderManager::Push(CQ3A_Shader* pPushMe)
{

	int AlreadyFound = FindShader((*pPushMe).GetName());

	if (AlreadyFound<0)
	{
		int i=FindFirstFreeShader();
		if (i==SHADER_MAN_MAX_SHADERS)
			return SHADER_MAN_MAX_SHADERS;

		m_Data.m_ppShaders[i] = pPushMe;
		m_Data.m_uiShadersNum++;
		return i;
	}
	else
	{
		return (UINT) AlreadyFound;
	}
}*/



CQ3A_Shader* CQ3A_ShaderManager::Load(char* ShaderName)
{
	//int uiTempShaderIndex=0;
	CQ3A_Shader* pTempShader;

	/**/

	strlwr(ShaderName);

	/**/
	
	CQ3A_Shader* pResShader =  AlreadyInManager(ShaderName);

	if (pResShader)
	{
		return pResShader;
	} else //not loaded yet, we have to load it now
	{
	
		bool bFindInCache;

		bool bLoadedFromCache=false;			
		
		CQ3A_Shader* pShader=NULL;
		CQ3A_ShaderPass* pShaderPass=NULL;

				
		//pShader = g_ShaderParser.ParseShader(ShaderName);
		pTempShader =  st_ShaderParser->ParseShader(ShaderName);
		
		//
		/*if (m_Data.m_uiLastShaderNum<SHADER_MAN_MAX_SHADERS)
			m_Data.m_uiLastShaderNum++;*/

		m_ResourcesMap[ShaderName] = pTempShader;

		m_iResourcesNum++;

		return pTempShader;
	}

}

