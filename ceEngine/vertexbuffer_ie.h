#ifndef CRIMSON_VERTEX_BUFFER_INTERFACE_EXPORT_H
#define CRIMSON_VERTEX_BUFFER_INTERFACE_EXPORT_H

#pragma once

#include "Math3d.h"

class IVertexBuffer_Export
{
	public:
	IVertexBuffer_Export() {};
	virtual ~IVertexBuffer_Export() {};

	virtual UINT GetLength(void) const  = 0;


	// AddStream( STREAM_POSITION, 3, GL_FLOAT)
	virtual void AddStream(DWORD dwStreamType,DWORD dwElementType) = 0;
	

	// AllocateStreams(100, VB_MEM_GPU | VB_MERGED , GL_STATIC_DRAW_ARB)
	virtual void AllocateStreams(int iLength,DWORD dwVBFlags,int iUsageType) = 0;
	

	virtual void* GetStream(DWORD dwStreamType) const = 0;

	virtual void* StreamCopySubData(const char* pData, DWORD dwStreamType,int iLength, int iOffset=0) = 0;

	virtual void Lock(int iAccess) = 0;
	virtual void Unlock(void) = 0;



	virtual bool IsStreamActive(DWORD dwStream) const = 0;

	virtual char GetStreamCRtype(DWORD dwStream) const = 0;
	
	
	// think about what to do here, i don't want to return anything
	// and i don't want the user to act differently in case of sys mem / gpu mem
	
	//virtual void BindStreams(const CVertDecl* pVD) const = 0;

	virtual UINT GetVertexSizeBytes(void)= 0 ;
	
	virtual void CalculateTBN(int iIndexCount,const unsigned int* puiIndices,int iVertxOffset,int iVertCount,const CVec3f* pPosition,const CVec2f* pTexCoords,CVec3f* pNormals, CVec3f* pTangents, CVec3f* pBinormals) const= 0;

};



#endif