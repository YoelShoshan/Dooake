#ifndef CRIMSON_VERTEX_BUFFER_D3D_H
#define CRIMSON_VERTEX_BUFFER_D3D_H

#pragma once
#include "VertexBuffer.h"

#include "VertexDecleration.h"




#include "d3d9.h"
#include "d3dx9.h"


class CVertexBuffer_D3D: public IVertexBuffer
{
public:
	CVertexBuffer_D3D();
	~CVertexBuffer_D3D();

	UINT GetLength(void) const;

	// AddStream( STREAM_POSITION, 3, GL_FLOAT)
	void AddStream(DWORD dwStreamType,DWORD dwElementType);
	

	// AllocateStreams(100, VB_MEM_GPU | VB_MERGED , GL_STATIC_DRAW_ARB)
	void AllocateStreams(int iLength,DWORD dwVBFlags,int iUsageType);
	

	void* GetStream(DWORD dwStreamType) const;

	void* StreamCopySubData(const char* pData, DWORD dwStreamType,int iLength, int iOffset=0);

	void Lock(int iAccess);
	void Unlock(void);

	bool IsStreamActive(DWORD dwStream) const;

	char GetStreamCRtype(DWORD dwStream) const;

	
	void BindStreams(const CVertDecl* pVD) const;

	UINT GetVertexSizeBytes(void);

	void CalculateTBN(int iIndexCount,const unsigned int* puiIndices,int iVertxOffset,int iVertCount,const CVec3f* pPosition,const CVec2f* pTexCoords,CVec3f* pNormals, CVec3f* pTangents, CVec3f* pBinormals) const;

private:

	// think about what to do here, i don't want to return anything
	// and i don't want the user to act differently in case of sys mem / gpu mem
	void BindStream(DWORD dwStreamType,const CVertDecl* pVD) const;

	UINT GetElementTypeSizeBytes(DWORD dwElementType) const;

	LPDIRECT3DVERTEXBUFFER9 m_D3Dvb;
	
	void* m_pMergedData;

	DWORD m_dwVBFlags;
	unsigned int m_uiLength;

	int m_iUsageType;
	

	//int          m_iLockAccess;

	//tStream m_Streams[STREAM_MAX];
	




};












#endif