#ifndef CRIMSON_GRAPHICS_API_GL_H
#define CRIMSON_GRAPHICS_API_GL_H

#pragma once

#include "GraphicsAPI.h"
#include "OpenGLExtensions.h"
#include "VertexBuffer.h"
#include "VertexDecleration.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "RendererState.h"
#include "IndexBuffer.h"

#include "Math3D.h"

class CGraphicsAPI_GL: public IGraphicsAPI
{
public:
	CGraphicsAPI_GL();
	~CGraphicsAPI_GL();

	unsigned int GetFrameNum(void) const { return m_uiFrameNum;};

	// init graphics api
	bool Init(int iResX, int iResY, HWND hWnd);

	// get api handness
	int GetHandness(void) const { return API_RIGHT_HANDED;};

	// updates projection matrix on window resize
	void OnResize(int iResX, int iResY);

	int GetResX() const
	{
		return m_iResX;
	}

	int GetResY() const
	{
		return m_iResY;
	}

	// dummy func
	void* GetDevice(void) const{ return NULL;} ;

	void SetViewport(int x, int y, int iWidth, int iHeight) const;

	void SetClearColor(float fR,float fG, float fB, float fA) ;

	void Clear(DWORD dwFlags) const;
	

	void BeginScene() const {};
	void EndScene() const {};

	void BeginFrame(void);

	void Present(void);

	void DrawPrimitives(int iIndicesStart,int iIndicesCount,int iBaseVertexIndex, int iMinIndex,int iVerticesNum) const;

	// generate buffers
	IVertexBuffer_Export* GenerateVertexBuffer(void) const;
	IIndexBuffer*  GenerateIndexBuffer(void) const;
	CVertDecl*  GenerateVertexDecleration(void) const;

	// generate shaders programs
	IShaderProgram_Export* GenerateShaderVertexProgram(void)   const;
	IShaderProgram_Export* GenerateShaderFragmentProgram(void) const;


	void SetProjectionMatrix(float fFovy,float fAspect,float fZNear,float fZFar);

	// expects column based memory layout !
	void SetWorldMatrix(const float* pMat);
	void SetViewMatrix(const float* pMat);
	void SetProjectionMatrix(const float* pMat);

	
	// return column based memory layout !
	const float* GetProjectionMatrix(void);
	const float* GetWorldMatrix(void);
	const float* GetViewMatrix(void);
	const float* GetModelViewMatrix(void);

	// generate Texture
	ITexture*   GenerateTexture(void) const;

	// generate RenderToTexture
	IRenderToTexture*  GenerateRenderToTexture(void) const;
	
	// update auto uniforms
	void UpdateAutoUniforms(void) ;

	// caps
	int GetSamplersNum(void) const { return m_iSamplersNum;};


	// samplers state

	void BindTexture(int iSamplerNum,CSmartPointer<ITexture>& tex);

	void UnBindTexUnit(int iSamplerNum);
	
	void BindVertexShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms);
	void UnBindVertexShader(void);

	void BindFragmentShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms);
	void UnBindFragmentShader(void);


	// VBs/IBs

	void BindVB(IVertexBuffer_Export* pVB, const CVertDecl* pVD);
	void BindIB(const IIndexBuffer* pIB);

	
	void SetClamping(int iSamplerNum,char cClamping);
	
	void SetFiltering(int iSamplerNum,char cFiltering,char cAnisotropicLevel);

	// renderer state
	void  SetDepthTest(bool bDepthTest);
	DWORD GetDepthTest(void) const { return m_RendererState.m_bDepthTest;};

	void  SetDepthWrite(bool bDepthWrite);
	DWORD GetDepthWrite(void) const { return m_RendererState.m_bDepthWrite;};

	void SetDepthFunc(DWORD dwDepthFunc);
	DWORD GetDepthFunc(void) const { return m_RendererState.m_dwDepthFunc; };

	void SetCullFunc(DWORD dwCullFunc);
	DWORD GetCullFunc(void) const {return m_RendererState.m_dwCullFunc;};

	void SetBlendFunc(char cSource,char cTarget);

	void SetAlphaFunc(DWORD dwFunc);
	void SetAlphaReferenceValue(DWORD dwVal);

	void EnableScissorRect(void) ;
	void DisableScissorRect(void) ;
	void SetScissorRect(int x1, int y1, int x2, int y2) const;

	// debug

	void DebugRenderBBox(const BBox& bbox);

	void DrawDebugLine(const CVec3f& v1, const CVec3f& v2, const CVec3f& col) ;

	void DrawDebugLine2D(const CVec2f& v1, const CVec2f& v2) ;



	const tGfxAPIProf& GetProfileData() const{ return m_ProfData; };

	void AddDebugText(const char* pFormat, ...) const 
	{
		// dummy
	};


	void printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...) const
	{
		// dummy
	};

	void stopPrinting(void) {};
	void startPrinting(void) {};

private:


	bool InitExtensions(void);

	// matrices
	
	float* m_pfWorld;
	bool  m_bWorldIdentity;

	float* m_pfView;
	float* m_pfProjection;
	float* m_pfWorldView;


	// renderer state
	tRendererState m_RendererState;

	// samplers states
	int            m_iSamplersNum;
	tSamplerState* m_pSamplers;

	CSmartPointer<IShaderProgram> m_ActiveVertexProgram;
	CSmartPointer<IShaderProgram> m_ActiveFragmentProgram;
	

	

	IVertexBuffer_Export* m_pActiveVB;
	

	const IIndexBuffer*         m_pActiveIB;
	const void*                 m_pActiveIB_DATA;



	COpenGLExtensions m_GLExtensions;

	HDC		m_hDC;						// Device Context
	HGLRC	m_hRC;						// Rendering Context


	int m_iResX;
	int m_iResY;


	// used to prevent multiple rendering of same entity
	unsigned int m_uiFrameNum;

	// profiling
	tGfxAPIProf m_ProfData;

	// helps only update uniforms when needed
	//bool m_bMatricesDirty;

	
};




#endif