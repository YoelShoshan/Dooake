#ifndef SMARTCOMPTR_H
#define SMARTCOMPTR_H

#pragma once

#include "ce.h"

template <class T>
class CSmartCOMPtr
{
public:
	CSmartCOMPtr()
	{
		m_pData=NULL;
	}
	~CSmartCOMPtr()
	{
		SafeRelease(m_pData);
	}

	CSmartCOMPtr<T>& operator=(T* newptr)
	{
		if (m_pData)
			m_pData->Release();
		m_pData=newptr;
		return (*this);
	}

	T* operator->()
	{
		assert(m_pData);
		return m_pData;
	}
	const T* operator->() const
	{
		assert(m_pData);
		return m_pData;
	};

	T** operator&()
	{
		return &m_pData;
	}
	

	operator T*()
	{
		return m_pData;
	}
	operator const T*() const
	{
		return m_pData;
	}

	bool operator==(T* p2) const
	{
		return p2==m_pData;
	}
	bool operator!=(T* p2) const
	{
		return p2!=m_pData;
	}
	bool operator!() const
	{
		return !m_pData;
	}


protected:
	T* m_pData;
};

#endif