#ifndef CRIMSON_INDEX_BUFFER_D3D_H
#define CRIMSON_INDEX_BUFFER_D3D_H

#pragma once

#include "d3d9.h"
#include "d3dx9.h"


#include "IndexBuffer.h"

class CIndexBuffer_D3D : public IIndexBuffer
{
public:
	CIndexBuffer_D3D();
	~CIndexBuffer_D3D();
	void* GetData(void);

	int GetLength(void) const;
	const void* Bind(void)const;
		
	// Notice: uiOffset and uiSize are in ELEMENTS not BYTES !!
	void Lock(int iAccess,UINT uiOffset, UINT uiSize);
	void Unlock(void);
	
	// Allocate(30,IB_MEM_SYS)
	void Allocate(int iLength,DWORD dwIBFlags);
	

private:

    LPDIRECT3DINDEXBUFFER9 m_D3Dib;

	int m_iLength;
	void* m_pData;
	int m_iLockAccess;
	DWORD m_dwIBFlags;

};








#endif
