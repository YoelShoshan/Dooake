#ifndef CRIMSON_INDEX_BUFFER_GL_H
#define CRIMSON_INDEX_BUFFER_GL_H

#pragma once

#include "IndexBuffer.h"

class CIndexBuffer_GL : public IIndexBuffer
{
public:
	CIndexBuffer_GL();
	~CIndexBuffer_GL();
	void* GetData(void);

	int GetLength(void) const;
	const void* Bind(void)const;
		
	void Lock(int iAccess,UINT uiOffset, UINT uiSize);
	void Unlock(void);
	
	// Allocate(30,IB_MEM_SYS)
	void Allocate(int iLength,DWORD dwIBFlags);
	

private:

    UINT  m_uiVBOID;

	int m_iLength;
	unsigned int * m_pData;
	int m_iLockAccess;
	DWORD m_dwIBFlags;	
};





#endif