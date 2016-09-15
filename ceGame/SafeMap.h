#ifndef CRIMSON_THREAD_SAFE_MAP_H
#define CRIMSON_THREAD_SAFE_MAP_H

#pragma once

#include <map>
#include "Mutex.h"

//when using remember to always acquire/release

template<class KEY,class DATA>
class CSafeMap : public CMutex
{
public:
	CSafeMap()
	{

	}

	~CSafeMap()
	{

	}

	std::map<KEY,DATA>* GetMap(void) { return &m_Map; };



private:
	std::map<KEY,DATA> m_Map;
};







#endif