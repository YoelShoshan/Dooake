#ifndef CRIMSON_MATERIAL_H
#define CRIMSON_MATERIAL_H

#pragma once

#include "Shader.h"
#include "Defines.h"

#include "Texture.h"

#include "GraphicsAPI.h"

#include "RendererState.h"

#include "SmartPointer.h"

#include "ReferenceCount.h"

#include "Technique.h"

#include "Pass.h"


#include "Technique.h"

#include "Material_IE.h"


#define CRIMSON_TECHNIQUES_MAX 16


#include <string>
#include <map>

typedef std::map<std::string,std::string>                     PARAMETERS_BINDING_MAP;
typedef std::map<std::string,std::string>::iterator           PARAMETERS_BINDING_MAP_IT;

#include "ceParser.h"

////////////////////////////////////////////////////////////
//
// Important TODO:
//
// Create a VD per pass
// which is actually created inside the ShaderProgram
////////////////////////////////////////////////////////////



class CMaterial : public IMaterial_Export
{
friend class CMaterialManager;
public:	

	// perhaps make this constructer private
	CMaterial();

	~CMaterial();

	bool Load(const char* pchFileName);
	//bool Load(const CQ3A_Shader* pQ3A_Shader);

	/*bool GenerateSimpleTextureMaterial(const char* pchTexName);*/

	bool GeneratePerPixelLitMaterial(const char* pchTexName);
	bool GenerateSimpleTextureMaterial(const char* pchTexName);
	bool GenerateAlphaBlendedAlwaysDrawTextureMaterial(const char* pchTexName);
	
	bool GenerateFromQ3AShader(const CQ3A_Shader_Data* pQ3AShader);

	const CTechnique* GetTechnique(unsigned int iIndex) const;

	void Bind(void) const;

	//bool AddTechnique(CTechnique* pTech);
	CTechnique* AddTechnique(void);

	bool CreateMaxVD(void);
	const tMaxVD* GetMaxVD(void) const { return &m_MaxVD; };

	const char* GetName(void) const { return m_pcName; } ;

	unsigned int GetID(void) const { return m_uiID; };

	void InjectRTT(IRenderToTexture* pRTT);


	unsigned int GetSort(void) const { return m_uiSort;};

	bool IsTransparent(void);

	//bool GetPostProcessing(void) const { return m_bPostProcessing;};

	// TODO: i need to create a vd per pass, by using the parsed data from the cgVertexShader

private:
	

	bool LoadMAT(const char* pchFileName,PARAMETERS_BINDING_MAP& parBinding);

	bool LoadMAT(const char* pchFileName);


	void AddVertexDeformation(CPass* pPass,const CQ3A_Shader_Data* pQ3AShader, UINT uiPassNum);
	bool Generate_AddQ3AShaderPass(const CQ3A_Shader_Data* pQ3AShader, UINT uiPassNum);

	

	void FillCurrentQueute(char* pFillMe,CParser& par,PARAMETERS_BINDING_MAP& parBinding);
	void FillCurrentWord(char* pFillMe,CParser& par,PARAMETERS_BINDING_MAP& parBinding);

	unsigned int m_uiTechniquesNum;
	CTechnique** m_ppTechniques;

	unsigned int m_uiID;
	
	unsigned int m_uiSort;

	tMaxVD m_MaxVD;

	char* m_pcName;

	//bool m_bPostProcessing;
};









#endif