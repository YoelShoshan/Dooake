#ifndef CRIMSON_STATIC_MODEL_SHARED_H
#define CRIMSON_STATIC_MODEL_SHARED_H

#pragma once

#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "ReferenceCount.h"
#include "StaticModelManager.h"
#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"
#include "Renderable_IE.h"



struct tSubMesh_3DS
{
	tSubMesh_3DS(){	ZeroMemory(this,sizeof(tSubMesh_3DS));};

	int  iVerticesNum;
	int  iTexCoordsNum;
	int  iIndicesNum;
	float*          pfVertices;
	float*          pfTexCoords;
	unsigned int* uiIndices;
	char*           pcName;
};

#define SUB_MESHES_MAX_3DS 200



#define GENERATE_QUAD   1
#define GENERATE_SPHERE 2 // place holder - not implemented
#define GENERATE_BOX    3 // place holder - not implemented

class CStaticModel_Shared : public CReferenceCount
{
friend class CStaticModelManager;
public:	
	~CStaticModel_Shared();

	bool Load3DS(const char* pcFileName, float fScaleX = 1.f, float fScaleY = 1.f, float fScaleZ = 1.f);

	bool Generate(DWORD dwGenerate, int iTessLevel,float fScaleX = 1.f, float fScaleY = 1.f, float fScaleZ = 1.f,DWORD dwGeneratePPL = RENDERABLE_GENERATE_NONE);

	const char* GetName(void)const { return m_pcName;} ;
	int GetSubMeshesNum(void)const { return m_iSubMeshesNum;};
	const tSubMesh_3DS* GetSubMesh(int iIndex)const { return m_ppSubMeshes[iIndex];};

	const IVertexBuffer_Export* GetVertexBuffer(int iIndex) const { return m_ppVertexBuffers[iIndex];};
	const IIndexBuffer*  GetIndexBuffer(int iIndex) const { return m_ppIndexBuffers[iIndex];};

	BBox m_BBox;

protected:	
	CStaticModel_Shared();


	bool GenerateBuffers(DWORD dwGenerate);

	IVertexBuffer_Export** m_ppVertexBuffers;
	IIndexBuffer**  m_ppIndexBuffers;

	private:
	char* m_pcName;

	int        m_iSubMeshesNum;
	tSubMesh_3DS** m_ppSubMeshes;

	


};

#endif