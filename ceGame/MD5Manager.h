///////////////////////////////////////////
//
// Static Model Manager
//
///////////////////////////////////////////

#ifndef CRIMSON_MD5_MANAGER_H
#define CRIMSON_MD5_MANAGER_H

#pragma once


#define st_MD5Manager CMD5Manager::Inst()

#include "MD5_Shared.h"

#include "SmartPointer.h"

#include "Singleton.h"

#include "Manager.h"


class CMD5Manager : public CManager<CMD5_Shared>,public CSingleton<CMD5Manager>	
{
public:

	CMD5Manager() {};
	~CMD5Manager() {};
	
	CSmartPointer<CMD5_Shared> Load(const char *FileName);
};

#endif
