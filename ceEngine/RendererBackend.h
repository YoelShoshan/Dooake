#ifndef CRIMSON_RENDERER_BACKEND_H
#define CRIMSON_RENDERER_BACKEND_H

#pragma once

#include "Renderable_IE.h"
#include "VertexDecleration.h"
#include "Texture.h"
#include "VertexBuffer_IE.h"

#include "RendererBackend_IE.h"

#include "RenderToTexture.h"

#include "SmartPointer.h"

#include "RTTProxy.h"

#include "RendererBackend.h"
#include "RenderToTexture.h"

#include "Material.h"
#include "Q3AShader_Data.h"

#include "EngineRenderable.h"

#include <vector>


#define RENDER_TARGETS_MAX 10

class CRendererBackend : public IRendererBackend_Export
{
	
public:
	CRendererBackend();
	~CRendererBackend();

	void EnableLightScissorRectTest(void);
	void DisableLightScissorRectTest(void);

	
	void DisableForceCulling(void) const;
	void SetForceCulling(DWORD dwCulling) const;


	void SetEnv(DWORD dwSource,DWORD dwDest,int iTexUnitNum);
	//void DebugTestDraw(void);

	void Render(std::vector<const IRenderable_Export*>& rends_list ,int iTech,DWORD dwLighting);

	//void PerformOnlyDrawCall(const IRenderable_Export* rend);

	void Render2DQuad(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fWidth, float fHeight);

	void Render2DQuad_Perc(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fHeight, float fWidth);
	

	void RenderFullScreenQuadPP(CRenderToTextureProxy* pRTT , CSmartPointer<IMaterial_Export> mat,bool bFlip, int iWidth, int iHeight);

	void RenderFullScreenQuad(CSmartPointer<IMaterial_Export> spMat,int iWidth, int iHeight);

	void RenderFullScreenQuadAndInjectRTT(IRenderToTexture* pRTT,int iWidth, int iHeight);


	IVertexBuffer_Export* GenerateVertexBuffer(void) const;
	IIndexBuffer*  GenerateIndexBuffer(void) const;
	CVertDecl*  GenerateVertexDecleration(void) const;
	ITexture*     GenerateTexture(void) const;
	
	ILight_Export*        GenerateLight(void) const;


	CSmartPointer<ITexture> LoadTexture(const char* pcName) const;
	CSmartPointer<IMaterial_Export> LoadMaterial(const char* pcName) const;

	CSmartPointer<IMaterial_Export> LoadMaterialFromQ3AShader(const CQ3A_Shader_Data* pQ3AShaderData,bool& bScripted) const;


	// Render Targets

	void DeclareNonPPCamera(void);

	bool StartCamera(int iCameraID,float fClearR, float fClearG, float fClearB, float fClearA) ;
	bool EndCamera() ;

	int GetCurrentCamera(void) const { return m_iCurrentCameraRenderTarget;};


	// you get back a regular texture interface,
	// that way the material manager can treat it as regulat texture :)
	ITexture* GetRenderTarget(int iRenTarget);

	void AddPostProcessEffectMaterial(const char* pcName);

	void CleanPostProcessChain();


	/*bool BindPreProcessRenderTarget(int iNum);
	bool UnbindPreProcessRenderTarget(int iNum) const;*/

	void PerformPostProcess() ;


	/*ITexture* GetPreProcessingRenderTarget(int iNum);*/

	// profiling

	const tRendererBackProf& GetProfileData() const { return m_ProfData; };
	
private:

	bool m_bLightScissorRectTest;

	// render to textures
	CRenderToTextureProxy* m_pTestRenderToTexture[RENDER_TARGETS_MAX];


	// post processing:
	CRenderToTextureProxy* m_pPreProcessRenderToTexture;
	CRenderToTextureProxy* m_pPostProcessRenderToTexture;

	CSmartPointer<IMaterial_Export>* m_sppPostProcessMaterial;

	int m_iPPChainLength; // Post Process Current Chain Length

	int m_iCurrentCameraRenderTarget;

	CSmartPointer<IMaterial_Export> m_PostProcessBlur_X;
	CSmartPointer<IMaterial_Export> m_PostProcessBlur_Y;
	CSmartPointer<IMaterial_Export> m_PostProcessScale;

	const unsigned int* m_pIndices;

	// for full screen quad

	CEngineRenderable* m_pFullScreenQuadRend;

	bool m_bCurrentCameraDeclaredNonPP;
		
	// profiling
	tRendererBackProf m_ProfData;





};

#endif

