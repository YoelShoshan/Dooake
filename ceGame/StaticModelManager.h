///////////////////////////////////////////
//
// Static Model Manager
//
///////////////////////////////////////////

#ifndef CRIMSON_STATIC_MODEL_MANAGER_H
#define CRIMSON_STATIC_MODEL_MANAGER_H

#pragma once


#define st_StaticModelManager CStaticModelManager::Inst()

#include "StaticModel_Shared.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"

class CStaticModel_Shared;
class CStaticModelManager : public CManager<CStaticModel_Shared>,public CSingleton<CStaticModelManager>	
{
public:

	CStaticModelManager() {};
	~CStaticModelManager() {};
	
	CSmartPointer<CStaticModel_Shared> Load(const char *FileName,float fScaleX, float fScaleY, float fScaleZ);
};

#endif
