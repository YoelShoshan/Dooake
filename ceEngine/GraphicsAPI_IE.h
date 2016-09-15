#ifndef CRIMSON_GRAPHICS_API_INTERFACE_EXPORT_H
#define CRIMSON_GRAPHICS_API_INTERFACE_EXPORT_H

#pragma once

#define API_RIGHT_HANDED 0
#define API_LEFT_HANDED  1

#define API_TEX_UNITS_NUM       4
#define API_TEX_COORD_UNITS_NUM 8


#include "VertexBuffer_IE.h"
#include "ShaderProgram_IE.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "RenderToTexture.h"

#include "SmartPointer.h"

#include "ProfileData.h"


class IGraphicsAPI_Export
{
public:
	IGraphicsAPI_Export() {};
	virtual ~IGraphicsAPI_Export() {};

	virtual int         GetHandness(void) const = 0;
	
	virtual bool Init(int iResX, int iResY, HWND hWnd) = 0;
	virtual void OnResize(int iResX, int iResY)        = 0;

	virtual void BeginScene()  const                   = 0;
	virtual void EndScene()    const                   = 0;

	virtual void BeginFrame(void)                      = 0;
	virtual void Present(void)                         = 0;
	virtual void* GetDevice(void) const                = 0;

	virtual IVertexBuffer_Export*  GenerateVertexBuffer(void)             const = 0;
	virtual IIndexBuffer*      GenerateIndexBuffer(void)                  const = 0;
	virtual IShaderProgram_Export*    GenerateShaderVertexProgram(void)   const = 0;
	virtual IShaderProgram_Export*    GenerateShaderFragmentProgram(void) const = 0;
	virtual ITexture*          GenerateTexture(void)                      const = 0;
	virtual IRenderToTexture*  GenerateRenderToTexture(void)              const = 0;


	virtual void UnBindVertexShader(void) = 0;
	virtual void UnBindFragmentShader(void) = 0;

	// expects column based memory layout !
	virtual void SetWorldMatrix(const float* pMat) = 0;
	virtual void SetViewMatrix(const float* pMat) = 0;
	virtual void SetProjectionMatrix(const float* pMat) = 0;	
	virtual void SetProjectionMatrix(float fFovy,float fAspect,float fZNear,float fZFar) = 0;	


	// returns in column based memory layout
	virtual const float* GetProjectionMatrix(void) = 0;
	virtual const float* GetWorldMatrix(void) = 0;	
	virtual const float* GetViewMatrix(void) = 0;
	virtual const float* GetModelViewMatrix(void) = 0 ;



	virtual void SetBlendFunc(char cSource,char cTarget) = 0;
	virtual void SetAlphaFunc(DWORD dwAlphaFunc) = 0;
	virtual void SetAlphaReferenceValue(DWORD dwVal) = 0;

	virtual int GetResX() const = 0 ;
	virtual int GetResY() const = 0 ;

	virtual void SetClearColor(float fR,float fG, float fB, float fA) = 0;

	virtual void Clear(DWORD dwFlags) const = 0;

	virtual void SetViewport(int x, int y, int iWidth, int iHeight) const = 0;



	virtual void EnableScissorRect(void) = 0;
	virtual void DisableScissorRect(void) = 0;
	virtual void SetScissorRect(int x1, int y1, int x2, int y2) const = 0;

	virtual void DebugRenderBBox(const BBox& bbox) = 0;

	virtual void DrawDebugLine(const CVec3f& v1, const CVec3f& v2, const CVec3f& col)  = 0;

	virtual void DrawDebugLine2D(const CVec2f& v1, const CVec2f& v2)  = 0;


	virtual const tGfxAPIProf& GetProfileData() const = 0;

	virtual void AddDebugText(const char* pFormat, ...) const = 0;

	virtual void printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...) const = 0;
	virtual void stopPrinting(void)  = 0;
	virtual void startPrinting(void)  = 0;

	virtual unsigned int GetFrameNum(void) const = 0;


};




#endif