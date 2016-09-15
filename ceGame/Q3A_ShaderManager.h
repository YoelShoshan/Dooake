#ifndef Q3A_SHADER_MANAGER_H
#define Q3A_SHADER_MANAGER_H

#pragma once

#include <windows.h>
#include "Math3d.h"
#include "GMath.h"
#include "SmartPointer.h"
#include "Texture.h"
#include "Timer_IE.h"
#include "Singleton.h"

#include "Q3AShader_Data.h"







class CQ3A_Shader: public CReferenceCount
{
public:

     
     CQ3A_Shader(); 
     ~CQ3A_Shader(); 

	//int GetPassCount();

	const CQ3A_Shader_Data* GetData() { return &m_Data; }

	inline int CQ3A_Shader::GetPassCount() const
	{
		return m_Data.m_uiPassesNum; 
	}
	
	
	 void AddPass(CQ3A_ShaderPass* pSP); 
	 void ChangePass(CQ3A_ShaderPass* pSP,int iPassNum); 


	inline CQ3A_ShaderPass* CQ3A_Shader::GetPass(const UINT iPass) const
	{
		if (iPass>=MAX_PASS_COUNT) return NULL;
			return m_Data.m_ppPasses[iPass]; 
	}

	 void SetName(const char* cpName);
	 char* GetName();

	 void SetShaderFlags(DWORD dwShaderFlags);
	 DWORD GetShaderFlags(void);

	 void SetShaderSort(UINT dwShaderSort);
	 UINT GetShaderSort(void);

	 void SetSurfaceParm(DWORD dwSurfaceParm) { m_Data.m_dwSurfaceParm = dwSurfaceParm;};
	 DWORD GetSurfaceParm(void) {return m_Data.m_dwSurfaceParm;};


	 // TODO: change this into sth else:
	 //       a variable that keeps the TYPE of the shader:
	 //       1. opaque shader  - a shader that uses an opaque texture of the first tex unit
	 //       2. blended shader - a shader that doesn't
	 //       ...

	 void SetAllBlended(bool bAllBlended) { m_Data.m_bAllBlended = bAllBlended;};
	 bool GetAllBlended(void) { return m_Data.m_bAllBlended;}


	 //////////////////
	 // deformVertexes
	 //////////////////

	 void AddDeformVertexesMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	 void AddDeformVertexesWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	 void AddDeformVertexesAutoSprite(void);
	 void AddDeformVertexesBulge(float f1,float f2,float f3);

	 SDeformVertexesObject* GetDVObject(void) { return &m_Data.m_DVObject;};

	 bool GetIsDVUsed(void) { return m_Data.m_bDVUsed;};


	 // sky booleans

	 void SetFarBoxUsed(bool bSet){m_Data.m_bFarBoxUsed = bSet;};
	 bool GetFarBoxUsed(void){return m_Data.m_bFarBoxUsed;};

	 // Was this shader found in the shader files? or was it fallback-auto-generated
	 bool IsScripted(void) const { return m_bScripted; }
	 void SetScripted(bool bScripted)  { m_bScripted = bScripted;}
		
protected:

	bool m_bScripted;

	CQ3A_Shader_Data m_Data;	

};

#define SHADER_MAN_MAX_SHADERS 3000

#define st_Q3A_ShaderManager CQ3A_ShaderManager::Inst()

class CQ3A_ShaderManager : public CManager<CQ3A_Shader>,public CSingleton<CQ3A_ShaderManager>	
{
public:
	CQ3A_ShaderManager();
	~CQ3A_ShaderManager();

	CQ3A_Shader* Load(char* ShaderName);

	//CQ3A_Shader* GetShader(UINT uiShaderID);

	//int GetShadersNum(void) { return (int)m_uiShadersNum;};


private:
	//UINT       m_uiShadersNum;
	//UINT        m_uiLastShaderNum;
	//CQ3A_Shader ** m_ppShaders;
	//CSmartPointer<CQ3A_Shader>* m_sppShaders;

};




#endif YOEL_SHADER_H