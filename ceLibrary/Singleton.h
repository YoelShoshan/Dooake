#ifndef CRIMSON_SINGLETON_H
#define CRIMSON_SINGLETON_H

#pragma once

#include "Defines.h"
#include "Singleton.h"
#include <windows.h>
#include "ce.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

template <class T> class CSingleton 
{
public:
	CSingleton() {};
	~CSingleton() 
	{
		//SafeDelete(pinstance);
	};

	static void CreateInstance(void) 
	{
		if (!pinstance)
			pinstance = new T;
		else
			MessageBox(HWND_DESKTOP, "SingleTon!!!", "Error", MB_OK);
	};
	static T* Inst(void)	
		{
			return pinstance;	
		};	
	static void Destroy() 
	{
		SafeDelete(pinstance);
	};

protected:
	static T* pinstance;
};

template <class T> 
T* CSingleton<T>::pinstance=NULL; 



#endif