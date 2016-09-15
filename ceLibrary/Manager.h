#ifndef CRIMSON_MANAGER_INTERFACE_H
#define CRIMSON_MANAGER_INTERFACE_H

#pragma once

#include "SmartPointer.h"

#include <map>
#include <string>


template <class T>
class CManager
{
public:
	CManager::CManager()
	{
		m_iResourcesNum = 0;
	}

	~CManager() {};

	void Unregister(const char* pcResourceName) 
	{	
		// TODO: notice when this happens!!!
		if (!pcResourceName)
			return;

		std::map<std::string,T*>::iterator it = m_ResourcesMap.begin();
	

		it = m_ResourcesMap.find(std::string(pcResourceName));

		if (it == m_ResourcesMap.end())
			return;

		m_ResourcesMap.erase(it);
	}

	int  GetResourcesNum(void) const
	{
		return m_iResourcesNum;
	}

protected:

	T* AlreadyInManager(const char* pcResourceName) const
	{
		std::map<std::string,T*>::const_iterator it = m_ResourcesMap.begin();

		it = m_ResourcesMap.find(std::string(pcResourceName));

		if (it!=m_ResourcesMap.end()) // found!
		{
			return it->second;
		}
		else // not found
			return NULL;

	}

	int m_iResourcesNum;

	std::map<std::string,T*> m_ResourcesMap;
};

#endif