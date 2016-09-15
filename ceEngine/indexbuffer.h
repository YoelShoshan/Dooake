#ifndef CRIMSON_INDEX_BUFFER_H
#define CRIMSON_INDEX_BUFFER_H

#pragma once

#include "Defines.h"

#define IB_MEM_GPU    (1<<0)
#define IB_MEM_SYS    (1<<1)
#define IB_DYNAMIC    (1<<2)
#define IB_STATIC     (1<<3)

class IIndexBuffer
{
public:
	IIndexBuffer() {};
	virtual ~IIndexBuffer() {};
	virtual void* GetData(void) = 0;
	virtual int GetLength(void) const= 0;

	virtual const void* Bind(void)const = 0;

	virtual void Lock(int iAccess,UINT uiOffset, UINT uiSize) = 0;
	virtual void Unlock(void) = 0;
	
	// Allocate(30,IB_MEM_SYS)
	virtual void Allocate(int iLength,DWORD dwIBFlags) = 0;
};

#endif