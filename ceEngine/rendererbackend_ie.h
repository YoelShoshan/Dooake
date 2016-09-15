#ifndef CRIMSON_RENDERER_BACKEND_EXPORT_H
#define CRIMSON_RENDERER_BACKEND_EXPORT_H

#pragma once

#include "Renderable_IE.h"
#include "Texture.h"
#include "VertexBuffer_IE.h"
#include "Q3AShader_Data.h"

#include "RenderToTexture.h"

#include "SmartPointer.h"

#include <vector>

//TODO - generalize this, and support custom names in the 
//       material system instead of defines

#define PRE_PROCESSING_FINAL 0 
#define RENDER_CAMERA_0      1
#define RENDER_CAMERA_1      2
#define RENDER_CAMERA_2      3
#define RENDER_CAMERA_3      4
#define RENDER_CAMERA_4      5
#define RENDER_CAMERA_5      6
#define RENDER_CAMERA_DEPTH      7
#define RENDER_CAMERA_SCALE_DOWN      8
#define RENDER_CAMERA_SCALE_DOWN_2     9

// only for in engine use (inside the backend)
// TODO: REMOVE this from the interface! (user shouldn't see this)
#define PRE_PROCESSING_FINAL_FOUND_PP 10
#define PRE_PROCESSING_FINAL_DIDNT_FIND_PP 11


#define MAX_POST_PROCESS_CHAIN_SIZE 10

#include "ProfileData.h"

#include "Light_IE.h"

class IRendererBackend_Export
{
public:
	IRendererBackend_Export() {};
	virtual ~IRendererBackend_Export() {};

	virtual void Render(std::vector<const IRenderable_Export*>& rends_list,int iTech, DWORD dwLighting) = 0;

	virtual void Render2DQuad(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fWidth, float fHeight) = 0;

	// expects percents [0.f,100.f]
	virtual void Render2DQuad_Perc(CSmartPointer<IMaterial_Export> spMat, float fPosX, float fPosY, float fWidth, float fHeight) = 0;

	virtual void RenderFullScreenQuad(CSmartPointer<IMaterial_Export> spMat,int iWidth,int iHeight) = 0;

	virtual void RenderFullScreenQuadAndInjectRTT(IRenderToTexture* pRTT,int iWidth,int iHeight) = 0;

	//virtual void PerformOnlyDrawCall(const IRenderable_Export* rend) = 0;
	
	virtual IVertexBuffer_Export* GenerateVertexBuffer(void) const        = 0;
	virtual IIndexBuffer*         GenerateIndexBuffer(void) const         = 0;
	virtual ILight_Export*        GenerateLight(void) const               = 0;

	virtual CSmartPointer<ITexture> LoadTexture(const char* pcName) const = 0;
	virtual CSmartPointer<IMaterial_Export> LoadMaterial(const char*pcName) const = 0;
	virtual CSmartPointer<IMaterial_Export> LoadMaterialFromQ3AShader(const CQ3A_Shader_Data* pQ3AShaderData,bool& bScripted) const = 0;

	// render targets

	virtual void DeclareNonPPCamera(void) = 0;

	virtual bool StartCamera(int iNum,float fClearR, float fClearG, float fClearB, float fClearA) = 0;	
	virtual bool EndCamera() = 0;

	virtual int GetCurrentCamera(void) const = 0;
	
	virtual void PerformPostProcess() = 0;

	virtual void CleanPostProcessChain() = 0;

	virtual void AddPostProcessEffectMaterial(const char* pcName) = 0;


	virtual void EnableLightScissorRectTest(void) = 0;
	virtual void DisableLightScissorRectTest(void) = 0;


	virtual const tRendererBackProf& GetProfileData() const = 0;


	virtual void SetForceCulling(DWORD dwCulling) const = 0;
	virtual void DisableForceCulling(void) const = 0;

};


#endif