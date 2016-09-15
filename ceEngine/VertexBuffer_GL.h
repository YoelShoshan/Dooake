#ifndef CRIMSON_VERTEX_BUFFER_GL_H
#define CRIMSON_VERTEX_BUFFER_GL_H

#pragma once

#include "Defines.h"
#include "VertexBuffer.h"
//#include "OpenGLExtensions.h"
//#include "glext.h"
#include "VertexDecleration.h"



class CVertexBuffer_GL : public IVertexBuffer
{
public:
	CVertexBuffer_GL();
	~CVertexBuffer_GL();

	UINT GetLength(void) const;

	// AddStream( STREAM_POSITION, CR_FLOAT3)
	void AddStream(DWORD dwStreamType,DWORD dwElementType);
		

	UINT GetElementTypeSizeBytes(DWORD dwElementType) const;

	// AllocateStreams(VB_MEM_GPU | VB_MERGED , GL_STATIC_DRAW_ARB)
	void AllocateStreams(int iLength,DWORD dwVBFlags,int iUsageTypem);
	
	void* GetStream(DWORD dwStreamType) const;

	void* StreamCopySubData(const char* pData, DWORD dwStreamType,int iLength, int iOffset=0);

	void Lock(int iAccess);
	void Unlock(void);

	bool IsStreamActive(DWORD dwStream) const;

	char GetStreamCRtype(DWORD dwStream) const;

	 // binds all streams that are present in the given vertdecl
	void BindStreams(const CVertDecl* pVD) const;

	 UINT GetVertexSizeBytes(void) { return m_uiVertexSizeBytes;};

	 void CalculateTBN(int iIndexCount,const unsigned int* puiIndices,int iVertxOffset,int iVertCount,const CVec3f* pPosition,const CVec2f* pTexCoords,CVec3f* pNormals, CVec3f* pTangents, CVec3f* pBinormals) const;



 private:

	 // think about what to do here, i don't want to return anything
	// and i don't want the user to act differently in case of sys mem / gpu mem
	 void BindStream(DWORD dwStreamType,const CVertDecl* pVD) const;

	 
	 void AllocateStreamsMERGED(int iUsageType, int iLength);
	 void AllocateStreamsSINGLE(int iUsageType, int iLength);

	 //int m_iLockAccess;


	 UINT  m_uiMergedVBOID;
	 void* m_pMergedData;

	DWORD   m_dwVBFlags;
	UINT    m_uiLength;
	UINT    m_uiVertexSizeBytes;
	//tStream m_Streams[STREAM_MAX];
};








#endif