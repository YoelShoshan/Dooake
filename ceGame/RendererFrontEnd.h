#ifndef CRIMSON_RENDERER_FRONT_END_H
#define CRIMSON_RENDERER_FRONT_END_H

#pragma once

#include "Renderable.h"
#include "IndexBuffer.h"

#include <vector>

typedef  std::vector<const IRenderable_Export*> RENDS_VECTOR;
typedef std::vector<const IRenderable_Export*>::iterator RENDS_VECTOR_IT ;



class CRendererFrontEnd
{
public:
	CRendererFrontEnd();
	~CRendererFrontEnd();

	void Push(const IRenderable_Export* rend);

	void Clean();
	
	

	void Render(CFrustum* pCamFrustum);

	void RenderDepthMap(ILight_Export* pLight);

	void RenderDepthFirst();

	bool RequiresPostProcessing();

	/*void SetQ3MapBatchingIBSize(unsigned int uiNum,CRenderable& sampleRend);*/

	// assumes all renderables share the same single VB !!
	/*void RenderBatchedQuake3Style();*/

	void SetRoughFallBack(bool bSet) { m_bRoughFallBack = bSet;};
	bool GetRoughFallBack(void) const { return m_bRoughFallBack; };
	

private:

	void GenerateShadowMaps();

	void PostProcess(void);


	void RenderLitBatch(std::vector<const IRenderable_Export*>& batch);

	void RenderPlainBatch(std::vector<const IRenderable_Export*>& batch);

	void Sort();

	 struct tMaterialSmaller 
     { 
          bool operator()(const IRenderable_Export* a, const IRenderable_Export* b) ;
     };

	int m_iActiveRends;


	std::vector<const IRenderable_Export*> m_currentRendsSharingMaterial;

	

	RENDS_VECTOR m_pRendsVector;

	IIndexBuffer*	m_pIndexBuffer_Dynamic;
	int				m_iDynamicBufferPos;
	int				m_iDynamicIndexBufferCreationSize;
	CRenderable*	m_pQ3BatchingRend;


	// internal use

	float m_mProj_BACKUP[16];
	float m_mWorld_BACKUP[16];
	float m_mView_BACKUP[16];


	bool m_bRoughFallBack;

	// temp

	const IIndexBuffer*	m_pIB_Old;
	int m_iStart_Old;
	int m_iCount_Old;
	int m_iBaseVertexIndex_Old;
	int m_iMinIndex_Old;
	int m_iVertsNum_Old;
	BBox m_BBox_Old;
	std::set<ILight_Export*> m_EffectingLights_Old;

};





#endif