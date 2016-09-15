#include "stdafx.h"
#include "RendererBackend.h"
#include "Defines.h"

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "VertexBuffer.h"
#include "VertexDecleration.h"



#include "TextureManager.h"

#include "Material.h"

#include "MaterialManager.h"

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "CTimer.h"

#include "Mmsystem.h"

#include "Profiler.h"

#include "Light.h"

#include "SmartPointer.h"

#include "RTTProxy.h"

#include <set>

extern DWORD g_dwForcedCullingMethod;


CRendererBackend::CRendererBackend()
{
	for (int i=0;i<RENDER_TARGETS_MAX;i++)
		m_pTestRenderToTexture[i] = NULL;

	m_pPreProcessRenderToTexture = NULL;
	m_pPostProcessRenderToTexture = NULL;

	m_pFullScreenQuadRend = NULL;

	/*// current pre render to texture post process
	m_pCurrentPreRTT_PP = NULL;
	// current post render to texture post process
	m_pCurrentPostRTT_PP = NULL;*/

	m_sppPostProcessMaterial = new CSmartPointer<IMaterial_Export>[MAX_POST_PROCESS_CHAIN_SIZE];

	m_iPPChainLength = 0;

	m_iCurrentCameraRenderTarget = -1;

	// profiling
	m_ProfData.iTriCount = 0;
	m_ProfData.iApiDrawCalls = 0;

	m_bLightScissorRectTest = true;

	m_bCurrentCameraDeclaredNonPP = false;

}

CRendererBackend::~CRendererBackend()
{
	
}


void CRendererBackend::SetEnv(DWORD dwSource,DWORD dwDest,int iTexUnitNum)
{
/*	if (iTexUnitNum==0) // if the current tex unit of the card things are quite simple...
	{
		glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (dwSource == GL_ONE && dwDest == GL_ZERO)
		{
			glDisable(GL_BLEND);							
		}	
		else
		{						

			glEnable(GL_BLEND);		
			glBlendFunc(dwSource,dwDest);			
		}
	}
	else // if the current tex unit is not the first, we should use glTexEnvi to emulate things
	{							

		if (dwSource == GL_ONE && dwDest == GL_ONE)
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		else
		if (dwSource == GL_ONE && dwDest == GL_ZERO)
			{
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
		else
		if ( (dwSource == GL_ZERO && dwDest== GL_SRC_COLOR)
			|| (dwSource == GL_DST_COLOR && dwDest== GL_ZERO)	)
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else
		if (dwSource == GL_SRC_ALPHA && dwDest == GL_ONE_MINUS_SRC_ALPHA)
			{
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR); 
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA); 
			}					
		else // default
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
*/
}

void CRendererBackend::RenderFullScreenQuadAndInjectRTT(IRenderToTexture* pRTT,int iWidth, int iHeight)
{
	//BREAKPOINT(1);

	static CMaterial*  pMat = NULL;

	if (!pMat)
	{
		pMat = new CMaterial();
		pMat->Load("Materials/RenderLightDepth.mat");
	}
	

	pMat->InjectRTT(pRTT);

	CSmartPointer<IMaterial_Export> spMat = pMat;

	RenderFullScreenQuad(spMat,iWidth,iHeight);
}

void CRendererBackend::Render2DQuad_Perc(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fWidth, float fHeight)
{
	float fResX = (float) g_pGraphicsAPI->GetResX();
	float fResY = (float) g_pGraphicsAPI->GetResY();

	Render2DQuad(spMat,
		fPosX*0.01f*fResX,
		fPosY*0.01f*fResY,
		fWidth*0.01*fResX,
		fHeight*0.01*fResY);

	

}

void CRendererBackend::Render2DQuad(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fWidth, float fHeight)
{
	//BREAKPOINT(1);

	// NOTICE: KEEP CHECKING CULLING!

	static std::vector<const IRenderable_Export*> rends;

	if (!m_pFullScreenQuadRend)
	{
		m_pFullScreenQuadRend = new CEngineRenderable();
		m_pFullScreenQuadRend->Generate(ENGINE_RENDERABLE_FULL_SCREEN_QUAD);
	}


	g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
	
	m_pFullScreenQuadRend->SetMaterial(spMat);
	
	g_pGraphicsAPI->SetProjectionMatrix(NULL);
	g_pGraphicsAPI->SetViewMatrix(NULL);
	g_pGraphicsAPI->SetWorldMatrix(NULL);
	    

		
	float fResX = (float) g_pGraphicsAPI->GetResX();
	float fResY = (float) g_pGraphicsAPI->GetResY();
	
	/*if (g_pGraphicsAPI->GetHandness())
		fPosY = fResY-(fPosY+fHeight);*/

	
	// compute scale needed
	float fScaleX = fWidth  / fResX;
	float fScaleY = fHeight / fResY;
	CMatrix mScale;
	mScale.MatMakeScale(fScaleX,fScaleY,1.f);
	

	float fTransX = ((fPosX / fResX)*2.f)-1.f+fScaleX;
	float fTransY = ((fPosY / fResY)*2.f)-1.f+fScaleY;
	CMatrix mTrans;
	mTrans.MatMakeMove(fTransX,fTransY,0.f);
	//mTrans.MatMakeMove(0.6f,0.6f,0.f);

	

	float pfMatrix[16];
	(mTrans*mScale).ReturnArray(pfMatrix);
	//(mScale*mTrans).ReturnArray(pfMatrix);

	//(mTrans).ReturnArray(pfMatrix);

	//g_pGraphicsAPI->SetWorldMatrix(pfMatrix);i 
	m_pFullScreenQuadRend->SetTransform(pfMatrix);

	m_pFullScreenQuadRend->SetDrawCallParameters(0,6,0,0,4);

	

	rends.clear();
	rends.push_back(m_pFullScreenQuadRend);
	Render(rends,0,CR_PASS_RENDER_ALL_PASSES);


}

void CRendererBackend::RenderFullScreenQuad(CSmartPointer<IMaterial_Export> spMat,int iWidth, int iHeight)
{
	RenderFullScreenQuadPP(NULL,spMat,false,iWidth,iHeight);
}

void CRendererBackend::RenderFullScreenQuadPP(CRenderToTextureProxy* pRTT , CSmartPointer<IMaterial_Export> mat,bool bFlip, int iWidth, int iHeight) 
{	
	//BREAKPOINT(1);

	// NOTICE: KEEP CHECKING CULLING!

	static std::vector<const IRenderable_Export*> rends;

	if (!m_pFullScreenQuadRend)
	{
		m_pFullScreenQuadRend = new CEngineRenderable();
		m_pFullScreenQuadRend->Generate(ENGINE_RENDERABLE_FULL_SCREEN_QUAD);
	}


	// if we are not last in the chain
	if (pRTT)
	{	
		// render to the post render target
		
		BREAKPOINT(!pRTT);
		pRTT->BindRenderTo();
			
		
	}
	else
	{		
		g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
	}


	m_pFullScreenQuadRend->SetMaterial(mat);

	g_pGraphicsAPI->SetProjectionMatrix(NULL);	
	g_pGraphicsAPI->SetViewMatrix(NULL);
	g_pGraphicsAPI->SetWorldMatrix(NULL);


	// solve the full screen quad offset issue
	float fOffsetX = -1.f / ((float) iWidth);
	float fOffsetY = 1.f / ((float) iHeight);

	if (g_pGraphicsAPI->GetHandness()==API_RIGHT_HANDED)
		fOffsetY*=-1.f;


	CMatrix m;
	m.MatMakeMove(fOffsetX,fOffsetY,0.f);
	float m_array[16];
	m.ReturnArray(m_array);
	
	
	m_pFullScreenQuadRend->SetTransform(m_array);
	
    
	rends.clear();

	rends.push_back(m_pFullScreenQuadRend);
	

	Render(rends,0,CR_PASS_RENDER_ALL_PASSES);


	// unbind post process
				

	// if we're not the last on the chain
	if (pRTT)
	{
		// unbind post process
		BREAKPOINT(!pRTT);
		pRTT->UnbindRenderTo();
		
		// flip between pre and post render targets
		// so next post process render can assume the same pre-condition
		if (bFlip)
		{
			IRenderToTexture* t1 = pRTT->GetRenderToTexture();
			IRenderToTexture* t2 = pRTT->GetRenderToTexture();

			m_pPreProcessRenderToTexture->SetRenderToTexture(t2);
			pRTT->SetRenderToTexture(t1);
		}
	}else
	{

	}

	

}



void CRendererBackend::Render(std::vector<const IRenderable_Export*>& rends_list , int iTech,DWORD dwLighting)
{
	if (rends_list.empty())
		return;
	
	static CProfileStep ScissorComputation("scissorComputation");	

	std::vector<const IRenderable_Export*>::iterator IT = rends_list.begin();	
	const IRenderable_Export* rend = (*IT);

	if (m_iCurrentCameraRenderTarget==-1 && iTech != TECHNIQUE_RENDER_DEPTH_MAP && iTech != TECHNIQUE_RENDER_DEPTH_FIRST /*&& iTech!= TECHNIQUE_GUI*/)
	{
		char message[100];
		sprintf_s(message,100,"ERROR:a Render not inside a StartCamera,EndCamera clause has occured! - ignored.");
		g_pLogFile->OutPutPlainText(message,"4","990000",true,LOG_MESSAGE_WARNING_LVL_3);
		BREAKPOINT(1);
		return;
	}

	tUniform* objCameraPos;
	CVec3f camPos;
	
	tUniform* objLightPos;
	CVec3f vObjLightPos;

	tUniform* objLightLookAt;
	CVec3f vObjLightLookAt;

	tUniform* lightCol;
	tUniform* lightRadius;

	tUniform* lightViewProjMat;


	g_pGraphicsAPI->SetWorldMatrix(rend->GetTransform());

	static CMaterial* pRenderDepthMat  = NULL;

	if (!pRenderDepthMat)
	{
		pRenderDepthMat = new CMaterial();
		pRenderDepthMat->Load("Materials/TestRenderDepth.mat");		
	}

	static CMaterial* pDepthFirstMat  = NULL;

	if (!pDepthFirstMat)
	{
		pDepthFirstMat = new CMaterial();
		pDepthFirstMat->Load("Materials/DepthFirst.mat");		
	}
						

	const CMaterial* pMat;
	const CTechnique* pTech;
			
	if (iTech == TECHNIQUE_RENDER_DEPTH_MAP /*|| iTech ==TECHNIQUE_GUI*/)
	{
		pTech = pRenderDepthMat->GetTechnique(0);		
		pMat = pRenderDepthMat;
		pMat->Bind();

	} else if (iTech == TECHNIQUE_RENDER_DEPTH_FIRST /*|| iTech ==TECHNIQUE_GUI*/)
	{

		pTech = pDepthFirstMat->GetTechnique(0);		
		pMat = pDepthFirstMat;
		pMat->Bind();
	}
	else
	{
		pMat = (const CMaterial*) rend->GetMaterial();		
		pTech= pMat->GetTechnique(iTech);
		rend->BindMaterial(); // right now nothing happens inside!
	}
	
	

	/*char msg[150];
	sprintf(msg,"Renderable material %s sort value:%d",pMat->GetName(),pMat->GetSort());
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/

	//const CPass* pPass;


	int iGraphicsAPIResX = g_pGraphicsAPI->GetResX();
	int iGraphicsAPIResY = g_pGraphicsAPI->GetResY();
	
	

	// for now only use technique 0 ...
	for (int i=0;i<pTech->GetPassesNum();i++)
	{
		// if there was a direction to render only ambient-passes and this is light-pass then skip
		if ( (dwLighting==CR_PASS_RENDER_AMBIENT)
			&& (pTech->GetPass(i)->m_dwLighting==MAT_PASS_LIGHTING_LIGHT))
			continue;

		// if there was a direction to render only light-passes and this is ambient-pass then skip
		if ( (dwLighting==CR_PASS_RENDER_LIGHT)
			&& (pTech->GetPass(i)->m_dwLighting==MAT_PASS_LIGHTING_AMBIENT))
			continue;

		

		

		
		
		

		const std::set<ILight_Export*>* pActiveLights = NULL;
		std::set<ILight_Export*>::const_iterator activeLights_IT;


		for (IT=rends_list.begin();IT!=rends_list.end();++IT) 
		{
			rend = (*IT);

			g_pGraphicsAPI->SetWorldMatrix(rend->GetTransform());
			
			if (pTech->GetPass(i)->m_dwLighting==MAT_PASS_LIGHTING_AMBIENT
				|| iTech == TECHNIQUE_RENDER_DEPTH_MAP  || iTech == TECHNIQUE_RENDER_DEPTH_FIRST || dwLighting==CR_PASS_RENDER_ALL_PASSES
				
				) // ambient pass or rendering depth map or there was a direction to render all passes
			{
				// just render

					pTech->BindPass(i);

					const CVertDecl* pPassVD = pTech->GetPass(i)->vertexProgram.GetProgram()->GetVD();
					g_pGraphicsAPI->BindVB(((IVertexBuffer*)rend->GetVB()),pPassVD);
					g_pGraphicsAPI->BindIB(rend->GetIB());

					g_pGraphicsAPI->DrawPrimitives(rend->GetIndicesStart(),rend->GetIndicesCount(),rend->GetBaseVertexIndex(), rend->GetMinIndex(),rend->GetVerticesNum());
					// profiling
					m_ProfData.iTriCount+= rend->GetIndicesCount()/3;
					m_ProfData.iApiDrawCalls++;
				continue;
			}

			// assumed to be lighting pass by now

			
			pActiveLights = rend->GetEffectingLights();


			if (!pActiveLights) // no need to handle lights uniforms
			{
				//todo: figure out if i'm supposed to do nothing here.

				/*pTech->BindPass(i);

				const CVertDecl* pPassVD = pTech->GetPass(i)->vertexProgram.GetProgram()->GetVD();
				g_pGraphicsAPI->BindVB(((IVertexBuffer*)rend->GetVB()),pPassVD);
				g_pGraphicsAPI->BindIB(rend->GetIB());

				//g_pGraphicsAPI->DrawPrimitives(rend->GetIndicesStart(),rend->GetIndicesCount());
				// profiling
				m_ProfData.iTriCount+= rend->GetIndicesCount()/3;
				m_ProfData.iApiDrawCalls++;*/
			}
			else
			{
				if (m_bLightScissorRectTest)
					g_pGraphicsAPI->EnableScissorRect();

				for (activeLights_IT = pActiveLights->begin();activeLights_IT!=pActiveLights->end();++activeLights_IT)
				{
					//////////////////////////////////////////////////////////////////////////////////////
					// set lights settings

				/*	if (!rend->GetTransform()) // if world transform is identity
					{
						objCameraPos = g_pAutoUniforms->Get("objCameraPos");
						camPos.Set( g_pAutoUniforms->Get("cameraPos")->m_pfValue);
						objLightPos  = g_pAutoUniforms->Get("objLightPos");
						//vObjLightPos.Set(g_pAutoUniforms->Get("lightPos")->m_pfValue);

						vObjLightPos.Set((*activeLights_IT)->GetPos());

						memcpy(objCameraPos->m_pfValue,camPos.v,sizeof(float)*3);
						memcpy(objLightPos->m_pfValue,camPos.v,sizeof(float)*3);
						//s_bPreviousWasIdentityWorld = true;
					}
					else*/
													
						lightCol  = g_pAutoUniforms->Get("lightCol");
						memcpy(lightCol->m_pfValue,(*activeLights_IT)->GetColor().v,sizeof(CVec3f));

						lightRadius = g_pAutoUniforms->Get("lightRadius");
						lightRadius->m_pfValue[0] = (*activeLights_IT)->GetRadius();


						
												

			if (rend->GetTransform())
					{
												
						lightViewProjMat = g_pAutoUniforms->Get("lightViewProjMat");
						
						CMatrix mLightViewMat;
						mLightViewMat.Set((*activeLights_IT)->GetViewMatrix());

						CMatrix mLightProjectionMat;
						mLightProjectionMat.Set((*activeLights_IT)->GetProjectionMatrix());

						/*CMatrix mLightViewProjMat;
						mLightViewProjMat.Set((*activeLights_IT)->GetViewProjectionMatrix());*/


						CMatrix mWorld;
						mWorld.Set(rend->GetTransform());
						

						(mLightProjectionMat * mLightViewMat * mWorld).ReturnArray(lightViewProjMat->m_pfValue);

						//(mLightViewProjMat * mWorld).ReturnArray(lightViewProjMat->m_pfValue);
						
						


						objCameraPos = g_pAutoUniforms->Get("objCameraPos");
						camPos.Set( g_pAutoUniforms->Get("cameraPos")->m_pfValue);

						objLightPos  = g_pAutoUniforms->Get("objLightPos");
						vObjLightPos.Set((*activeLights_IT)->GetPos());

						objLightLookAt  = g_pAutoUniforms->Get("objLightLookAt");						
						vObjLightLookAt.Set((*activeLights_IT)->GetLookAt());

						

						//////////////////////////////////////////////
						// make rotating vector
						float fAng = (float)timeGetTime()/4000.f;
						CMatrix tempRot;
						tempRot.MatMakeRot(0,fAng,0);
						CVec3f vTempLookAt(0.f,0.f,-1.f);
						vTempLookAt = tempRot*vTempLookAt;
						//////////////////////////////////////////////


						CMatrix world_inv;
						// also try not-transposed
						world_inv.SetTransposed(g_pGraphicsAPI->GetWorldMatrix());
						world_inv.Inverse();
						world_inv.Transpose();
	
						// set object space camera position

						CVec3f vObjCamPos = world_inv * camPos;
						objCameraPos->m_pfValue[0] = vObjCamPos.v[0];
						objCameraPos->m_pfValue[1] = vObjCamPos.v[1];
						objCameraPos->m_pfValue[2] = vObjCamPos.v[2];

						// set object space light look at 
						// DEBUG DEBUG DEBUG
						//vObjLightLookAt = vObjLightLookAt*-1.f;

						CMatrix temp = world_inv;

						temp.m[0][3] = 0.f;
						temp.m[1][3] = 0.f;
						temp.m[2][3] = 0.f;
						temp.m[3][3] = 1;

						

						vObjLightLookAt = temp * vObjLightLookAt;
						
						
						// RESTORE
						memcpy(objLightLookAt->m_pfValue,vObjLightLookAt.v,3*sizeof(float));

						// DEBUG
						//memcpy(objLightLookAt->m_pfValue,vTempLookAt.v,3*sizeof(float));
						

					
						// set object light position
						vObjLightPos = world_inv * vObjLightPos;
						
						
						//float fMove = -0.5f + sinf( ((float)timeGetTime()/100.f));	
						

						/*
						////////////////////////////////////////////////////////////
						// make light float up-down
						float fDbgLightUpDownAni = g_pTimer->GetTime()/100.f;
						float fMove = -0.5f + sinf( fDbgLightUpDownAni);	
						vObjLightPos.v[1]+= fMove*10.f;
						////////////////////////////////////////////////////////////
						*/
						
						
						
						


						objLightPos->m_pfValue[0] = vObjLightPos.v[0];
						objLightPos->m_pfValue[1] = vObjLightPos.v[1];
						objLightPos->m_pfValue[2] = vObjLightPos.v[2];
					}
				else // world matrix is identity
					{

						lightViewProjMat = g_pAutoUniforms->Get("lightViewProjMat");
						memcpy(lightViewProjMat->m_pfValue,(*activeLights_IT)->GetViewProjectionMatrix(),sizeof(float)*16);

						//////////////////////////////////////////////
						// make rotating vector
						/*float fAng = (float)timeGetTime()/4000.f;
						CMatrix tempRot;
						tempRot.MatMakeRot(0,fAng,0);
						CVec3f vTempLookAt(0.f,0.f,-1.f);
						vTempLookAt = tempRot*vTempLookAt;*/
						//////////////////////////////////////////////

						objCameraPos = g_pAutoUniforms->Get("objCameraPos");
						camPos.Set( g_pAutoUniforms->Get("cameraPos")->m_pfValue);
						memcpy(objCameraPos->m_pfValue,camPos.v,sizeof(CVec3f));


						objLightPos  = g_pAutoUniforms->Get("objLightPos");
						vObjLightPos.Set((*activeLights_IT)->GetPos());
						

						/*
						////////////////////////////////////////////////////////////
						// make light float up-down
						float fDbgLightUpDownAni = g_pTimer->GetTime()/100.f;
						float fMove = -0.5f + sinf( fDbgLightUpDownAni);	
						vObjLightPos.v[1]+= fMove*10.f;
						////////////////////////////////////////////////////////////
						*/

						

						memcpy(objLightPos->m_pfValue,vObjLightPos.v,sizeof(CVec3f));



						objLightLookAt  = g_pAutoUniforms->Get("objLightLookAt");

						// RESTORE RESTORE RESTORE
						vObjLightLookAt.Set((*activeLights_IT)->GetLookAt());

						// DEBUG DEBUG DEBUG
						//vObjLightLookAt.Set(vTempLookAt);

						
						// RESTORE
						memcpy(objLightLookAt->m_pfValue,vObjLightLookAt.v,sizeof(CVec3f));

						// DEBUG
						//objLightLookAt->m_pfValue[0] = 0.4f;
						//objLightLookAt->m_pfValue[1] = 0.f;
						//objLightLookAt->m_pfValue[2] = 0.f;


					}

						//s_bPreviousWasIdentityWorld = false;
					



					// inject the shadow map into the material
					CPass* pTempPass = (CPass*) pTech->GetPass(i);
					CRenderToTextureProxy* pRTTProxy = pTempPass->GetShadowMapRTTProxy();
					BREAKPOINT(!pRTTProxy);
					pRTTProxy->SetRenderToTexture(	(*activeLights_IT)->GetDepthMapRTT());

					pTech->BindPass(i);

					const CVertDecl* pPassVD = pTech->GetPass(i)->vertexProgram.GetProgram()->GetVD();
					g_pGraphicsAPI->BindVB(((IVertexBuffer*)rend->GetVB()),pPassVD);
					g_pGraphicsAPI->BindIB(rend->GetIB());
					//////////////////////////////////////////////////////////////////////////////////////


					ScissorComputation.Begin();

					// scissor rect
					float fXmin,fXmax,fYmin,fYmax;
					
					(*activeLights_IT)->GetFrustum()->GetProjectedCornersBoundingSquare(fXmin,fXmax,fYmin,fYmax);

					int iXmin,iXmax,iYmin,iYmax;		

					fXmin*= (float) iGraphicsAPIResX;
					fXmax*= (float) iGraphicsAPIResX;

					fYmin*= (float) iGraphicsAPIResY;
					fYmax*= (float) iGraphicsAPIResY;

					iXmin = (int) fXmin;
					iXmax = (int) fXmax;
					iYmin = (int) fYmin;
					iYmax = (int) fYmax;

					ScissorComputation.End();

					/*BREAKPOINT(iXmin>0);
					BREAKPOINT(iYmin>0);

					BREAKPOINT(iXmax<iGraphicsAPIResX);
					BREAKPOINT(iYmax<iGraphicsAPIResY);*/
					
					if (m_bLightScissorRectTest)
						g_pGraphicsAPI->SetScissorRect(iXmin,iYmin,iXmax,iYmax);

					g_pGraphicsAPI->DrawPrimitives(rend->GetIndicesStart(),rend->GetIndicesCount(),0,0,rend->GetVerticesNum());




					// debug
					//g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());

					// profiling
					m_ProfData.iTriCount+= rend->GetIndicesCount()/3;
					m_ProfData.iApiDrawCalls++;



				}

				if (m_bLightScissorRectTest)
					g_pGraphicsAPI->DisableScissorRect();
			}

					
			


			/*pPass->vertexProgram.Unbind();
			pPass->fragmentProgram.Unbind();*/
		} 

		
	}

		


		

}

ILight_Export*  CRendererBackend::GenerateLight(void) const
{
	CLight* pRet = new CLight();
	return pRet;

}

IVertexBuffer_Export* CRendererBackend::GenerateVertexBuffer(void) const
{
	return g_pGraphicsAPI->GenerateVertexBuffer();
}

IIndexBuffer*  CRendererBackend::GenerateIndexBuffer(void) const
{
	return g_pGraphicsAPI->GenerateIndexBuffer();
}

CVertDecl*  CRendererBackend::GenerateVertexDecleration(void) const
{
	return g_pGraphicsAPI->GenerateVertexDecleration();
}


CSmartPointer<ITexture> CRendererBackend::LoadTexture(const char* pcName) const
{
	return st_TextureManager->Load(pcName);
}

//bScripted means that we actually found a script for this material and we're not using the default image.
CSmartPointer<IMaterial_Export> CRendererBackend::LoadMaterialFromQ3AShader(const CQ3A_Shader_Data* pQ3AShaderData,bool& bScripted) const
{
	return st_MaterialManager->LoadQ3AShader(pQ3AShaderData,bScripted);
}

CSmartPointer<IMaterial_Export> CRendererBackend::LoadMaterial(const char*pcName) const
{
	return st_MaterialManager->Load(pcName);
}

void CRendererBackend::DeclareNonPPCamera(void)
{
	m_bCurrentCameraDeclaredNonPP = true;

}

bool CRendererBackend::StartCamera(int iCameraID,float fClearR, float fClearG, float fClearB, float fClearA) 
{	
	
	// profiling
  	m_ProfData.iTriCount = 0; // tri count is per CAMERA render

	m_ProfData.iApiDrawCalls = 0;

	if (iCameraID!=PRE_PROCESSING_FINAL_FOUND_PP && iCameraID!=PRE_PROCESSING_FINAL_DIDNT_FIND_PP && m_iCurrentCameraRenderTarget>=0)
	{
		char message[200];
		// huh?? "buffer too small"
		//sprintf_s(message,70,"ERROR: StartCamera used while another StartCamera was active! ignored. - see ""EndCamera()"" for more details.");
		sprintf(message,"ERROR: StartCamera used while another StartCamera was active! ignored. - see ""EndCamera()"" for more details.");
		g_pLogFile->OutPutPlainText(message,"4","990000",true,LOG_MESSAGE_WARNING_LVL_3);
		BREAKPOINT(1);
		return;

	}

	m_iCurrentCameraRenderTarget = iCameraID;

	// should i restore this ??
	//if (dwType & CR_RTT_COLOR)

	g_pGraphicsAPI->SetClearColor(0.f,0.f,0.f,0.f);			

	// If depth is erased, the particles will be rendered on top of solid geometry (even if behind...)
	//g_pGraphicsAPI->Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);
	g_pGraphicsAPI->Clear(CR_CLEAR_COLOR);

	
	//g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());

	DWORD dwType;

	if (iCameraID==PRE_PROCESSING_FINAL)
	{
		/*if (m_bCurrentCameraDeclaredNonPP )
		{
			g_pGraphicsAPI->BeginScene();
			//g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());
			// set view port here!
			return true;
		}*/

		/*BREAKPOINT(!m_pPreProcessRenderToTexture);
		m_pPreProcessRenderToTexture->BindRenderTo();		

		dwType = m_pPreProcessRenderToTexture->GetType();*/

		g_pGraphicsAPI->BeginScene();
		
	} else	if (m_iCurrentCameraRenderTarget==PRE_PROCESSING_FINAL_FOUND_PP) // frontend decided that we must use pre process render
	{
		//g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());
		BREAKPOINT(!m_pPreProcessRenderToTexture);
		m_pPreProcessRenderToTexture->BindRenderTo();		
		dwType = m_pPreProcessRenderToTexture->GetType();


		return true;

	} else if (m_iCurrentCameraRenderTarget==PRE_PROCESSING_FINAL_DIDNT_FIND_PP) // frontend decided that there's no pre proces RTT render needed
	{
		g_pGraphicsAPI->BeginScene();

		g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());
		// set view port here!

		return true;
	}	
	else
	{
		// TODO: decide what to do when user wants to render into a RTT that is never used 
		// (notice: not only here...)
		BREAKPOINT(!m_pTestRenderToTexture[iCameraID]); 


		m_pTestRenderToTexture[iCameraID]->BindRenderTo();

		dwType = m_pTestRenderToTexture[iCameraID]->GetType();
		// REMOVED FOR RENDERER BACKEND MERGE
		/*
		glViewport (0, 0, m_pTestRenderToTexture[iCameraID]->GetWidth(), m_pTestRenderToTexture[iCameraID]->GetHeight());				
		*/
	}


	return true;
}



bool CRendererBackend::EndCamera() 
{
	bool bCurrentCameraDeclaredPP = m_bCurrentCameraDeclaredNonPP;

	if (m_bCurrentCameraDeclaredNonPP)
		m_bCurrentCameraDeclaredNonPP = false;

	if (m_iCurrentCameraRenderTarget==PRE_PROCESSING_FINAL)
	{
		g_pGraphicsAPI->EndScene();
		/*if (bCurrentCameraDeclaredPP )
		{			
			g_pGraphicsAPI->EndScene();
			m_iCurrentCameraRenderTarget = -1;
			return true;
		}*/

		/*BREAKPOINT(!m_pPreProcessRenderToTexture);
		m_pPreProcessRenderToTexture->UnbindRenderTo();		
		m_iCurrentCameraRenderTarget = -1;*/


		m_iCurrentCameraRenderTarget = -1;
		return true;
	}
	else if (m_iCurrentCameraRenderTarget==PRE_PROCESSING_FINAL_FOUND_PP)
	{
		BREAKPOINT(!m_pPreProcessRenderToTexture);
		m_pPreProcessRenderToTexture->UnbindRenderTo();		
		m_iCurrentCameraRenderTarget = -1;
	}
	else if (m_iCurrentCameraRenderTarget==PRE_PROCESSING_FINAL_DIDNT_FIND_PP)
	{
		g_pGraphicsAPI->EndScene();
		m_iCurrentCameraRenderTarget = -1;
	} else
	{
		BREAKPOINT(!m_pTestRenderToTexture[m_iCurrentCameraRenderTarget]);
		m_pTestRenderToTexture[m_iCurrentCameraRenderTarget]->UnbindRenderTo();		
		m_iCurrentCameraRenderTarget = -1;
	}

	
	return true;
}

ITexture* CRendererBackend::GetRenderTarget(int iCameraID)
{
		
	if (iCameraID==RENDER_CAMERA_DEPTH)
	{
		if (!m_pTestRenderToTexture[iCameraID])
		{
			m_pTestRenderToTexture[iCameraID] = new CRenderToTextureProxy();
			m_pTestRenderToTexture[iCameraID]->Init("CameraDepth",
													CR_RTT_SHADOW_MAP,
													//CR_RTT_COLOR_AND_DEPTH,
													g_pGraphicsAPI->GetResX(),
													g_pGraphicsAPI->GetResY());
													/*g_pGraphicsAPI->GetResX(),
													g_pGraphicsAPI->GetResY());*/

		 }

		return (ITexture*) m_pTestRenderToTexture[iCameraID];
	}


	if (iCameraID==RENDER_CAMERA_SCALE_DOWN)
	{
		if (!m_pTestRenderToTexture[iCameraID])
		{
			m_pTestRenderToTexture[iCameraID] = new CRenderToTextureProxy();
			m_pTestRenderToTexture[iCameraID]->Init("ScaleDownCamera",
													CR_RTT_COLOR_AND_DEPTH,
													g_pGraphicsAPI->GetResX()/SCALE_DOWN,
													g_pGraphicsAPI->GetResY()/SCALE_DOWN);

		 }

		return (ITexture*) m_pTestRenderToTexture[iCameraID];
	}

	if (iCameraID==RENDER_CAMERA_SCALE_DOWN_2)
	{
		if (!m_pTestRenderToTexture[iCameraID])
		{
			m_pTestRenderToTexture[iCameraID] = new CRenderToTextureProxy();
			m_pTestRenderToTexture[iCameraID]->Init("ScaleDownCamera",
													CR_RTT_COLOR_AND_DEPTH,
													g_pGraphicsAPI->GetResX()/SCALE_DOWN,
													g_pGraphicsAPI->GetResY()/SCALE_DOWN);
			//m_pTestRenderToTexture[iCameraID]->Init("ScaleDownCamera2",g_pGraphicsAPI->GetResX()/16,g_pGraphicsAPI->GetResY()/16);
			//m_pTestRenderToTexture[iCameraID]->Init("ScaleDownCamera",g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
		 }

		return (ITexture*) m_pTestRenderToTexture[iCameraID];
	}


	if (iCameraID==PRE_PROCESSING_FINAL)
	{
		if (!m_pPreProcessRenderToTexture)
		{
			m_pPreProcessRenderToTexture = new CRenderToTextureProxy();
			m_pPreProcessRenderToTexture->Init("PreProcess",
												CR_RTT_COLOR_AND_DEPTH,
												g_pGraphicsAPI->GetResX(),
												g_pGraphicsAPI->GetResY());
												//2048,2048);
			//m_pPreProcessRenderToTexture->Init("PreProcess",512,512);
		}
		
		return (ITexture*) m_pPreProcessRenderToTexture;
	}

	// else, it's a regular camera render target

	if (!m_pTestRenderToTexture[iCameraID])
	{
		char name[40];
		sprintf(name,"RenderTar%d",iCameraID);
		m_pTestRenderToTexture[iCameraID] = new CRenderToTextureProxy();
		m_pTestRenderToTexture[iCameraID]->Init(name,
												CR_RTT_COLOR_AND_DEPTH,
												g_pGraphicsAPI->GetResX(),
												g_pGraphicsAPI->GetResY());
	}

	
	return (ITexture*) m_pTestRenderToTexture[iCameraID];
}

void CRendererBackend::AddPostProcessEffectMaterial(const char* pcName)
{
	m_sppPostProcessMaterial[m_iPPChainLength++] =  LoadMaterial(pcName);

	if (m_iPPChainLength>1) // if more then a single post processing effect
	{
		// we'll need to flip and use an extra rendertarget

		if (!m_pPostProcessRenderToTexture)
		{
			m_pPostProcessRenderToTexture = new CRenderToTextureProxy();
			m_pPostProcessRenderToTexture->Init("PostProcess",
												CR_RTT_COLOR_AND_DEPTH,
												g_pGraphicsAPI->GetResX(),
												g_pGraphicsAPI->GetResY());
		}
		
	}

}



void CRendererBackend::CleanPostProcessChain()
{
	m_iPPChainLength=0;
	// perhaps do:
	// run on the chain and set all to null and only then set length to zero

}


void CRendererBackend::PerformPostProcess() 
{
	
	 // RESTORE FOR DOF!
	if (!m_PostProcessScale)
	{
		m_PostProcessScale = st_MaterialManager->Load("Materials/pp/Bloom_ScaleDown_BrightPass.mat");
		//m_PostProcessScale = st_MaterialManager->Load("Materials/scalePP-TRILINEAR-FROM-PRE-PROCESS.mat");
		//m_PostProcessScale = st_MaterialManager->Load("Materials/scalePP-NEAREST.mat");
	}

	
	// blur
	if (!m_PostProcessBlur_X)
	{
		m_PostProcessBlur_X = st_MaterialManager->Load("Materials/PP/blur_3x3_PP_X.mat");
	}

	if (!m_PostProcessBlur_Y)
	{
		m_PostProcessBlur_Y = st_MaterialManager->Load("Materials/PP/blur_3x3_PP_Y.mat");
	}



	if (!m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN])
		{
		
		}


	//int iSaveCameraRenderTarget = m_iCurrentCameraRenderTarget;

	// special value to silence the render function
	// because we're doing Render() calls NOT inside StartCamera/EndCamera clause
	m_iCurrentCameraRenderTarget = -2;


	D3DPERF_BeginEvent(D3DCOLOR_ARGB(0,1,0,0),L"PostProcessing  1");	
	//TODO: remove depth
	RenderFullScreenQuadPP(m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN],m_PostProcessScale,false,
		m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetWidth(),m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetHeight());
	D3DPERF_EndEvent();
	
	D3DPERF_BeginEvent(D3DCOLOR_ARGB(0,1,0,0),L"PostProcessing  2");	
	RenderFullScreenQuadPP(m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2],m_PostProcessBlur_X,false,
		m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2]->GetWidth(),m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2]->GetHeight());
	D3DPERF_EndEvent();
		
	D3DPERF_BeginEvent(D3DCOLOR_ARGB(0,1,0,0),L"PostProcessing  3");	
	RenderFullScreenQuadPP(m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN],m_PostProcessBlur_Y,false,
		m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetWidth(),m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetHeight());
	D3DPERF_EndEvent();
	
	D3DPERF_BeginEvent(D3DCOLOR_ARGB(0,1,0,0),L"PostProcessing  4");	
	RenderFullScreenQuadPP(m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2],m_PostProcessBlur_X,false,
		m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2]->GetWidth(),m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN_2]->GetHeight());
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_ARGB(0,1,0,0),L"PostProcessing  5");	
	RenderFullScreenQuadPP(m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN],m_PostProcessBlur_Y,false,
		m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetWidth(),m_pTestRenderToTexture[RENDER_CAMERA_SCALE_DOWN]->GetHeight());
	D3DPERF_EndEvent();
	
	m_iCurrentCameraRenderTarget = -2; //make it shut up (not inside camera begin/end clause

	for (int i=0;i<m_iPPChainLength;i++)
	{
		if (i == m_iPPChainLength - 1) // last
		{
			//g_pGraphicsAPI->BeginScene();
			StartCamera(PRE_PROCESSING_FINAL_DIDNT_FIND_PP,0.f,0.f,0.f,0.f);
			//g_pGraphicsAPI->SetViewport(0,0,1024,768);
			RenderFullScreenQuadPP(NULL,m_sppPostProcessMaterial[i],true,
				g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
		}
		else
			RenderFullScreenQuadPP(m_pPostProcessRenderToTexture,m_sppPostProcessMaterial[i],true,
			g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());
	}

	if (!m_iPPChainLength)
		StartCamera(PRE_PROCESSING_FINAL_DIDNT_FIND_PP,0.f,0.f,0.f,0.f);

	//m_iCurrentCameraRenderTarget = iSaveCameraRenderTarget;

	//m_iCurrentCameraRenderTarget = PRE_PROCESSING_FINAL;

}

/*ITexture* CRendererBackend_GL::GetPreProcessingRenderTarget(int iNum)
{
	if (!m_pPreProcessRenderToTexture)
	{
		m_pPreProcessRenderToTexture = g_pGraphicsAPI->GenerateRenderToTexture();
		m_pPreProcessRenderToTexture->Init("Test1",512,512);
	}
	
	return (ITexture*) m_pPreProcessRenderToTexture;
}*/


void  CRendererBackend::EnableLightScissorRectTest(void)
{
	m_bLightScissorRectTest = true;
}

void  CRendererBackend::DisableLightScissorRectTest(void)
{
	m_bLightScissorRectTest = false;
}

void CRendererBackend::SetForceCulling(DWORD dwCulling) const
{
	g_dwForcedCullingMethod = dwCulling;
}

void CRendererBackend::DisableForceCulling(void) const
{
	g_dwForcedCullingMethod = CE_DONT_FORCE_CULLING_METHOD;	
}