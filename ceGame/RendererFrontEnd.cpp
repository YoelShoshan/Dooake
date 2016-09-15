#include "stdafx.h"
#include "RendererFrontEnd.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include <algorithm>

#include "Profiler.h"
#include "ce.h"
#include "SmartPointer.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQ3Map;

#include "Profiler.h"

extern bool g_bRenderableRequirePostProcessing;


CRendererFrontEnd::CRendererFrontEnd()
{	
	m_iActiveRends = 0;

	m_pIndexBuffer_Dynamic = NULL;
	
	m_pQ3BatchingRend = NULL;

	m_bRoughFallBack = false;
}

CRendererFrontEnd::~CRendererFrontEnd()
{
}

void CRendererFrontEnd::Push(const IRenderable_Export* rend)
{

	assert(rend->GetVerticesNum()!=-1);

	if (rend->GetDontRender())
		return;
	//m_pRendsVector[m_iActiveRends] = rend;

	m_pRendsVector.push_back(rend);

	m_iActiveRends++;
}

void CRendererFrontEnd::Clean()
{
	static CProfileStep clean("Clean");
	clean.Begin();

	m_pRendsVector.clear();

	m_iActiveRends = 0;

	clean.End();

}

void CRendererFrontEnd::Sort()
{
	static CProfileStep sort("Sort");
	sort.Begin();
	std::sort(m_pRendsVector.begin(), m_pRendsVector.end(), tMaterialSmaller());
	sort.End();
}

/*
// assumes all renderables share the same single VB !!
void CRendererFrontEnd::RenderBatchedQuake3Style()
{	

	Sort();

	static CProfileStep UpdateBatchesStep("batching");	
	UpdateBatchesStep.Begin();


	bool bFirst = true;

	int iBatchIndicesNum = 0;

	UINT* pBatchIndices = NULL;
	UINT* pCurrentIndices = NULL;

	IIndexBuffer* pCurrentIB = NULL;

	unsigned int uiPrevID = 0xFFFFFFF;



	for (int i=0;i<m_iActiveRends;i++)
	{
		
		if ( (uiPrevID != m_pRendsVector[i]->GetMaterial()->GetID() ) ||
			i == m_iActiveRends-1 // last one
			
			)
		{
			// this is a new material

			// unlock previous and render
			if (!bFirst)
			{
				m_pQ3BatchingIB->Unlock();

				m_pQ3BatchingRend->SetDrawCallParameters(0,iBatchIndicesNum);

				UpdateBatchesStep.End();
				g_pRendererBack->Render(m_pQ3BatchingRend,0);
				UpdateBatchesStep.Begin();
			}

			if (i == m_iActiveRends-1) // last one
				continue;


			iBatchIndicesNum = 0;

			m_pQ3BatchingIB->Lock(CR_LOCK_WRITE_ONLY);
			pBatchIndices = (UINT*) m_pQ3BatchingIB->GetData();

			pCurrentIB = ((IIndexBuffer*) m_pRendsVector[i]->GetIB());

			
			pCurrentIB->Lock(CR_LOCK_READ_ONLY);
			pCurrentIndices = (UINT*) pCurrentIB->GetData();

			
			memcpy(pBatchIndices,
				pCurrentIndices + m_pRendsVector[i]->GetIndicesStart(),
				sizeof(UINT) * m_pRendsVector[i]->GetIndicesCount());

			pCurrentIB->Unlock();


			iBatchIndicesNum += m_pRendsVector[i]->GetIndicesCount();
			pBatchIndices += m_pRendsVector[i]->GetIndicesCount();

			
			// set material

			CSmartPointer<IMaterial_Export> spMat = (IMaterial_Export*) m_pRendsVector[i]->GetMaterial();
			m_pQ3BatchingRend->SetMaterial(spMat);
				
		}
		else
		{
			// accumulate into IB

						

			pCurrentIB->Lock(CR_LOCK_READ_ONLY);
			pCurrentIndices = (UINT*) pCurrentIB->GetData();

		
			memcpy(pBatchIndices,
				pCurrentIndices + m_pRendsVector[i]->GetIndicesStart(),
				sizeof(UINT) * m_pRendsVector[i]->GetIndicesCount());

			pCurrentIB->Unlock();

			iBatchIndicesNum += m_pRendsVector[i]->GetIndicesCount();
			pBatchIndices += m_pRendsVector[i]->GetIndicesCount();

		}			


		bFirst = false;

		uiPrevID = m_pRendsVector[i]->GetMaterial()->GetID();
	}

	
	UpdateBatchesStep.End();

}
*/

void CRendererFrontEnd::RenderDepthFirst()
{
	const std::set<ILight_Export*>* pEffectingLights;

	m_currentRendsSharingMaterial.clear();

	int iDBG = 0;

	/*for (int i=0;i<m_iActiveRends;i++)
	{	
		// ignore if it uses blending
		if (m_pRendsVector[i]->GetMaterial()->GetSort()!=MATERIAL_SORT_OPAQUE)
			continue;
		iDBG++;
		m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);						
	}

	g_pRendererBack->Render(m_currentRendsSharingMaterial,TECHNIQUE_RENDER_DEPTH_FIRST,CR_PASS_RENDER_AMBIENT);*/

	g_pRendererBack->Render(m_pRendsVector,TECHNIQUE_RENDER_DEPTH_FIRST,CR_PASS_RENDER_AMBIENT);

}

void CRendererFrontEnd::RenderDepthMap(ILight_Export* pLight)
{

	const std::set<ILight_Export*>* pEffectingLights;

	m_currentRendsSharingMaterial.clear();

	int iDBG = 0;


	for (int i=0;i<m_iActiveRends;i++)
	{	
		pEffectingLights = m_pRendsVector[i]->GetEffectingLights();
		if (pEffectingLights->find(pLight) != pEffectingLights->end())
		{
			// ignore it if it's not a shadow caster
			if (!m_pRendsVector[i]->IsShadowCaster())
				continue;
			iDBG++;
			m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);				
		}
	}

	//g_pLogFile->OutPutPlainText("CRendererFrontEnd::RenderDepthMap rendering occluders renderables set",LOG_MESSAGE_INFORMATIVE);
	g_pRendererBack->Render(m_currentRendsSharingMaterial,TECHNIQUE_RENDER_DEPTH_MAP,CR_PASS_RENDER_AMBIENT);
	//g_pLogFile->OutPutPlainText("CRendererFrontEnd::RenderDepthMap rendering occluders renderables set /DONE",LOG_MESSAGE_INFORMATIVE);

	//RenderLitBatch(m_currentRendsSharingMaterial);


}

/*void CRendererFrontEnd::RenderSimple(CFrustum* pCamFrustum)
{
	tUniform* time = g_pAutoUniforms->Get("time");
	time->m_pfValue[0] = g_pTimer->GetTimeSecondsFAKE();


	Sort();


}*/

void CRendererFrontEnd::PostProcess(void)
{
	g_pRendererBack->EndCamera();
	g_pRendererBack->PerformPostProcess();
		
	g_pGraphicsAPI->SetProjectionMatrix(m_mProj_BACKUP);
	g_pGraphicsAPI->SetWorldMatrix(m_mWorld_BACKUP);
	g_pGraphicsAPI->SetViewMatrix(m_mView_BACKUP);
}

bool CRendererFrontEnd::RequiresPostProcessing()
{
	for (RENDS_VECTOR::iterator it=m_pRendsVector.begin();it!=m_pRendsVector.end();++it)
	{
		if ((*it)->GetSort()==MATERIAL_SORT_POSTPROCESS ||
			(*it)->GetSort()==MATERIAL_SORT_OPAQUE_TRIGGER_POSTPROCESS ||
			(*it)->GetSort()==MATERIAL_SORT_BLEND_TRIGGER_POSTPROCESS	)
		{
			return true;
		}
	}

		return false;	
}

void CRendererFrontEnd::Render(CFrustum* pCamFrustum)
{

	/*char msg[150];
	sprintf(msg,"*******************************************CRendererFrontEnd::Render() Started.");
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/


	/*if (m_bRoughFallBack)
	{
		RenderSimple(pCamFrustum);
		return;
	}
	*/

	/*
	// DEBUG DEBUG DEBUG
	ILight_Export* pLight = g_pQ3Map->GetLights()[0];
	g_pRendererBack->RenderFullScreenQuadAndInjectRTT(pLight->GetDepthMapRTT());
	return;
	*/


	/////////////////////////////////////////////////////////////////////////////////
	// debug find frustum corners

	ILight_Export** pLights = g_pQ3Map->GetLights();
	int iLightsNum = g_pQ3Map->GetLightsNum();
	
	/*tUniform* pViewProjection = g_pAutoUniforms->Get("viewProjection");*/

if (!m_bRoughFallBack)
{

	for (int i=0;i<iLightsNum;i++)
	{
		/*CMatrix m1,m2;
		m1.SetTransposed(g_pGraphicsAPI->GetViewMatrix());
		m2.SetTransposed(g_pGraphicsAPI->GetProjectionMatrix());

		float pfView[16],pfProjection[16];
		m1.ReturnArray(pfView);
		m2.ReturnArray(pfProjection);*/

		//pLights[i]->GetFrustum()->ProjectCornerPoints(pfView,pfProjection);

		pLights[i]->GetFrustum()->ProjectCornerPoints(g_pGraphicsAPI->GetViewMatrix(),
			g_pGraphicsAPI->GetProjectionMatrix(),pCamFrustum);

		/*pLights[i]->GetFrustum()->ProjectCornerPoints(pViewProjection->m_pfValue);			*/
	}
}

	/*float pfTempLightView[16];
	float pfTempLightProjection[16];

	

	CMatrix mLightView,mLightProjection;

	mLightView.SetTransposed(pLight->GetViewMatrix());
	mLightView.ReturnArray(pfTempLightView);

	mLightProjection.SetTransposed(pLight->GetProjectionMatrix());
	mLightProjection.ReturnArray(pfTempLightProjection);*/

		

	//CFrustum::CalculateCornerPoints(pCorners,pfTempLightView,pfTempLightProjection);

	//pLight->GetFrustum()->CalculateCornerPoints(pLight->GetViewMatrix(),pLight->GetProjectionMatrix());


	///////////////////////////////////////////////////////////////
	


	/*tUniform* time = g_pAutoUniforms->Get("time");
	time->m_pfValue[0] = g_pTimer->GetTimeSecondsFAKE();*/


	Sort();

	


	/*
	// debug
	g_pLogFile->OutPutPlainText("**************TEST!","16","FF0000",true,LOG_MESSAGE_INFORMATIVE);

	int i=0;

	for (RENDS_VECTOR::iterator it=m_pRendsVector.begin();it!=m_pRendsVector.end();++it)
	{
		sprintf(msg,"%d) Sort value is: %d",i,(*it)->GetSort());
		g_pLogFile->OutPutPlainText(msg,"9","FF00FF",true,LOG_MESSAGE_INFORMATIVE);
		i++;
	}



	g_pLogFile->OutPutPlainText("**************TEST END!","16","FF0000",true,LOG_MESSAGE_INFORMATIVE);

	*/

	bool bRequiresPP = RequiresPostProcessing();
	

	g_bRenderableRequirePostProcessing = bRequiresPP;



	if (bRequiresPP)
	{
		g_pRendererBack->StartCamera(PRE_PROCESSING_FINAL_FOUND_PP,0.f,0.f,0.f,0.f);
		//RenderDepthFirst();
	}
	else
	{
		g_pRendererBack->DeclareNonPPCamera();
		g_pRendererBack->StartCamera(PRE_PROCESSING_FINAL_DIDNT_FIND_PP,0.f,0.f,0.f,0.f);
		//RenderDepthFirst();
	}




	// debug debug debug
	// simply rendering everything without batching

	/*for (int i=0;i<m_iActiveRends;i++)
	{
		m_currentRendsSharingMaterial.clear();
		m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);		
		RenderLitBatch(m_currentRendsSharingMaterial);
	}



	return;*/

	// store backups!
	// (will be used after the real scene begin camera)
	
	
	memcpy(m_mProj_BACKUP,g_pGraphicsAPI->GetProjectionMatrix(),sizeof(float)*16);		
	memcpy(m_mWorld_BACKUP,g_pGraphicsAPI->GetWorldMatrix(),sizeof(float)*16);	
	memcpy(m_mView_BACKUP,g_pGraphicsAPI->GetViewMatrix(),sizeof(float)*16);

	
	bool bMetPreProcess=false;


	static CProfileStep LinkedListPS("llists");	
	LinkedListPS.Begin();

	unsigned int uiPrevID = 0xFFFFFFF;

	bool bFirst = true;

	int iRendsCount = 0;


	//// Batching

	bool bBatchData = true; // batch data in order to minimize draw calls number
	m_iDynamicIndexBufferCreationSize = 50000;
	
	if (bBatchData)
	{
		if (!m_pIndexBuffer_Dynamic)
		{
			m_iDynamicBufferPos = 0;
			m_pIndexBuffer_Dynamic = g_pRendererBack->GenerateIndexBuffer();
			m_pIndexBuffer_Dynamic->Allocate(m_iDynamicIndexBufferCreationSize,IB_MEM_GPU | IB_DYNAMIC);
		}

	}
	
	m_currentRendsSharingMaterial.clear();

	for (int i=0;i<m_iActiveRends;i++)
	{
		///////////////////////////////////////////////////////
		// debug debug debug
		/*m_currentRendsSharingMaterial.clear();
		m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);
		g_pRendererBack->Render(m_currentRendsSharingMaterial,0);
		continue;*/			

		if ( 
			((i!=m_iActiveRends-1)  &&// not last
			(uiPrevID == m_pRendsVector[i]->GetMaterial()->GetID() &&
			!m_pRendsVector[i]->GetTransform()) ||
			bFirst)
			) 
		{
			m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);		
			iRendsCount++;
		}
		else 
		{
			LinkedListPS.End();
			
			bool bOptimizedIntoOneDrawCall = false;
			IRenderable_Export* pRend = NULL;
			bool bBBoxFound = false;


			if (bBatchData)
			{

				/////////////////////////////////////////////////////////////////////
				// Minimze Drawcalls - Update dynamic index buffer in order to create one draw call
				//
				// 1. Make sure that all renderables have indices data and it's identical
				bool bCanBeMinimizedToOneDrawCall = true;
				const UINT* pIndicesData = NULL;
				int iTotalIndicesCount = 0;
				int iTotalRenderablesCount = 0;
				int iMinVertex = 9999;
				int iMaxVertex = 0;
				BBox batchBBox;		
				std::set<ILight_Export*> batchLightSet;

				for (RENDS_VECTOR_IT it=m_currentRendsSharingMaterial.begin();it!=m_currentRendsSharingMaterial.end();++it) 
				{
					pRend = (IRenderable_Export*)(*it);
					if (!pRend->GetIndices_Data())
					{
						bCanBeMinimizedToOneDrawCall = false;
						break;
					}
					
					if (!pIndicesData)
						pIndicesData = pRend->GetIndices_Data();

					if (pRend->GetIndices_Data() != pIndicesData)
					{
						bCanBeMinimizedToOneDrawCall = false;
						break;
					}
					iTotalIndicesCount+= pRend->GetIndicesCount();
					iTotalRenderablesCount++;

					if (pRend->GetMinIndex() < iMinVertex)
						iMinVertex = pRend->GetMinIndex();

					if (pRend->GetMinIndex()+pRend->GetVerticesNum() > iMaxVertex)
						iMaxVertex = pRend->GetMinIndex()+pRend->GetVerticesNum();

					const BBox* pBBox = pRend->GetBBox();
					if (pBBox)
					{
						bBBoxFound = true;
						batchBBox.Set( pRend->GetBBox() );
					}
					
				}

				if (iTotalRenderablesCount<2)
					bCanBeMinimizedToOneDrawCall = false;
				
				//bCanBeMinimizedToOneDrawCall = false;
				
				if (bCanBeMinimizedToOneDrawCall)
				{

					DWORD dwLockType = CR_LOCK_WRITE_ONLY_NO_OVERWRITE;

					if (m_iDynamicBufferPos+iTotalIndicesCount >= m_iDynamicIndexBufferCreationSize)
					//if (1)
					{
						m_iDynamicBufferPos = 0;
						dwLockType = CR_LOCK_WRITE_ONLY_DISCARD;
					}
					
					m_pIndexBuffer_Dynamic->Lock(dwLockType,m_iDynamicBufferPos,iTotalIndicesCount);
					UINT* pIndices = (UINT*) m_pIndexBuffer_Dynamic->GetData();
					int iOldPosition = m_iDynamicBufferPos;
					
					
								

					for (RENDS_VECTOR_IT it=m_currentRendsSharingMaterial.begin();it!=m_currentRendsSharingMaterial.end();++it) 
					{
						pRend = (IRenderable_Export*)(*it);
						const UINT* pReadIndices = pRend->GetIndices_Data();
						pReadIndices+= pRend->GetIndicesStart();
						memcpy(pIndices,pReadIndices,sizeof(UINT)*pRend->GetIndicesCount());				
						m_iDynamicBufferPos+=pRend->GetIndicesCount();
						pIndices += pRend->GetIndicesCount();

						if (bBBoxFound)
							batchBBox.Add(pRend->GetBBox());

						// accumulate lights on entire batch
						const std::set<ILight_Export*>* pLightsSet = pRend->GetEffectingLights();

						if (pLightsSet)
						{
							for (std::set<ILight_Export*>::const_iterator l=pLightsSet->begin();l!=pLightsSet->end();l++)
							{
								batchLightSet.insert(*l);								
							}
						}
						
						
					}
					m_pIndexBuffer_Dynamic->Unlock();


					// store old light set for last renderable
					m_EffectingLights_Old.clear();
					const std::set<ILight_Export*>* pLightsSet = pRend->GetEffectingLights();
					for (std::set<ILight_Export*>::const_iterator l=pLightsSet->begin();l!=pLightsSet->end();l++)
					{
						m_EffectingLights_Old.insert(*l);
					}


					// override the last one (could be any of them)

					// push the state,render,and then pop the state

					// store old values
					m_pIB_Old = pRend->GetIB();
					m_iStart_Old = pRend->GetIndicesStart();
					m_iCount_Old = pRend->GetIndicesCount();
					m_iBaseVertexIndex_Old = pRend->GetBaseVertexIndex();
					m_iMinIndex_Old = pRend->GetMinIndex();
					m_iVertsNum_Old = pRend->GetVerticesNum();
					if (bBBoxFound)
						m_BBox_Old.Set(pRend->GetBBox());

					pRend->SetIndexBuffer(m_pIndexBuffer_Dynamic,false);
					pRend->SetDrawCallParameters(iOldPosition,iTotalIndicesCount,0,iMinVertex,iMaxVertex-iMinVertex);
					if (bBBoxFound)
						pRend->SetBBox(batchBBox);
					//pRend->SetDrawCallParameters(iOldPosition,iTotalIndicesCount,0,0,31066);

					for (std::set<ILight_Export*>::const_iterator l=batchLightSet.begin();l!=batchLightSet.end();l++)
					{
						pRend->AddEffectingLight(*l);
					}

					
					assert(!pRend->GetDontRender());

					m_currentRendsSharingMaterial.clear();
					m_currentRendsSharingMaterial.push_back(pRend);


					bOptimizedIntoOneDrawCall = true;
				}
			}


			if (!m_bRoughFallBack)
			{
				RenderLitBatch(m_currentRendsSharingMaterial);
			}
			else 
			{				
				RenderPlainBatch(m_currentRendsSharingMaterial);				
			}

			
			LinkedListPS.Begin();
			//blah++;

			iRendsCount = 0;

			if (bRequiresPP && !bMetPreProcess)
			{
				if (m_pRendsVector[i]->GetSort()==MATERIAL_SORT_POSTPROCESS)
				{

					//g_pLogFile->OutPutPlainText("Met first post processing material!",LOG_MESSAGE_INFORMATIVE);

					bMetPreProcess = true;

					PostProcess();

				}
			}

			
			m_currentRendsSharingMaterial.clear();
		
			m_currentRendsSharingMaterial.push_back(m_pRendsVector[i]);
			
			/*if (i==m_iActiveRends-1) // last
			{
				RenderLitBatch(m_currentRendsSharingMaterial);
			}*/ 

			if (bBatchData && bOptimizedIntoOneDrawCall)
			{
				// pop back the state of the renderable
				// CHANGE THIS INTO A SPECIAL CREATED RENDERABLE
				pRend->SetIndexBuffer(m_pIB_Old,false); //think about the second parameter some more!
				pRend->SetDrawCallParameters(m_iStart_Old,m_iCount_Old,m_iBaseVertexIndex_Old,m_iMinIndex_Old,m_iVertsNum_Old);					
				if (bBBoxFound)
					pRend->SetBBox(m_BBox_Old);
				pRend->ClearEffectingLights();


			}

		}

		bFirst=false;

		uiPrevID = m_pRendsVector[i]->GetMaterial()->GetID();
	}


	// last one
	if (!m_bRoughFallBack)
		RenderLitBatch(m_currentRendsSharingMaterial);
	else 
		RenderPlainBatch(m_currentRendsSharingMaterial);

	LinkedListPS.End();


	// means that we met only post processing TRIGGERING materials, and no post-process directly using RTTs
	// we still must trigger the post process
	// (an example would be that there are only alpha drawing materials that expect a bloom to occur)
	if ( bRequiresPP &&	!bMetPreProcess) 
	{
		PostProcess();
	}

	//int k=blah;

	

	
	/*sprintf(msg,"*******************************************CRendererFrontEnd::Render() Ended");
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/
		
}

void CRendererFrontEnd::RenderPlainBatch(std::vector<const IRenderable_Export*>& batch)
{
	g_pRendererBack->Render(batch,0,CR_PASS_RENDER_AMBIENT);
	//g_pRendererBack->Render(batch,0,CR_PASS_RENDER_LIGHT);
}

void CRendererFrontEnd::RenderLitBatch(std::vector<const IRenderable_Export*>& batch)
{
	g_pRendererBack->Render(batch,0,CR_PASS_RENDER_AMBIENT);
	g_pRendererBack->Render(batch,0,CR_PASS_RENDER_LIGHT);
}

bool CRendererFrontEnd::tMaterialSmaller::operator()(const IRenderable_Export* a, const IRenderable_Export* b) 
 {	 	 
	 if (a->GetSort() < b->GetSort())
		 return true;
	 else
		 return false;

	 /*if (a->GetTransform() < b->GetTransform())
		 return true; 
	 else 
		 return false;

	  if (a->GetMaterial()->GetID() < b->GetMaterial()->GetID())
		  return true;	  
	  else 
		  return false;
	  
	  if (a->GetIndices_Data() < b->GetIndices_Data())
		  return true;
	  else 
		  return false;*/

 }

/*
void CRendererFrontEnd::SetQ3MapBatchingIBSize(unsigned int uiNum,CRenderable& sampleRend)
{
	SafeDelete(m_pQ3BatchingRend);

	m_pQ3BatchingRend = new CRenderable();

	SafeDelete(m_pQ3BatchingIB);

	m_pQ3BatchingIB  = g_pRendererBack->GenerateIndexBuffer();
	m_pQ3BatchingIB->Allocate(uiNum,IB_MEM_GPU | IB_DYNAMIC);


	m_pQ3BatchingRend->SetIndexBuffer(m_pQ3BatchingIB,false);
	m_pQ3BatchingRend->SetVertexBuffer(sampleRend.GetVB(),false);

	CSmartPointer<IMaterial_Export> spMat = (IMaterial_Export*) sampleRend.GetMaterial();

	m_pQ3BatchingRend->SetMaterial(spMat);
	m_pQ3BatchingRend->SetTransform(sampleRend.GetTransform());



}
*/