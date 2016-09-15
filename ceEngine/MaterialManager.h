///////////////////////////////////////////
//
// Material Manager
//
///////////////////////////////////////////

#ifndef CRIMSON_MATERIAL_MANAGER_H
#define CRIMSON_MATERIAL_MANAGER_H

#pragma once


#define st_MaterialManager CMaterialManager::Inst()

#include "Material_IE.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Q3AShader_Data.h"

#include "Manager.h"

//NOTICE:

//if (FileName[0]=='@') // means generate shadow mapped,light passes based material
//if (FileName[0]=='#') // means generate simple basic texture material
//if (FileName[0]=='!') // means generate alpha blended basic texture material

//TODO: add alphafunc to my renderers! and support a new material type

class CMaterialManager : public CManager<IMaterial_Export>,public CSingleton<CMaterialManager>	
{
public:

	CMaterialManager() {};
	~CMaterialManager() {};
	
	CSmartPointer<IMaterial_Export> Load(const char *FileName);

	CSmartPointer<IMaterial_Export> LoadQ3AShader(const CQ3A_Shader_Data* pQ3AShaderData,bool& bScripted);

	
};

#endif
