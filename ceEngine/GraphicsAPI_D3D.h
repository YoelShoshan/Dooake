
#ifndef CRIMSON_GRAPHICS_API_D3D_H
#define CRIMSON_GRAPHICS_API_D3D_H

#pragma once

#include "GraphicsAPI.h"

#include<d3d9.h>
#include<d3dx9.h>

#include "VertexBuffer.h"
#include "VertexDecleration.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "RendererState.h"

#include "SmartPointer.h"

#include "DebugText_D3D.h"

#include <string>

class CGraphicsAPI_D3D: public IGraphicsAPI
{
public:
	CGraphicsAPI_D3D();
	~CGraphicsAPI_D3D();

	unsigned int GetFrameNum(void) const { return m_uiFrameNum;};

	// init graphics api
	bool Init(int iResX, int iResY, HWND hWnd);

	// get api handness
	int GetHandness(void) const { return API_LEFT_HANDED;};

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

	//  get d3d device
	void* GetDevice(void) const;

	void BeginFrame(void);

	void BeginScene() const;
	void EndScene() const;

	void Present(void);

	void DrawPrimitives(int iIndicesStart,int iIndicesCount,int iBaseVertexIndex, int iMinIndex,int iVerticesNum) const;

	

	// generate buffers
	IVertexBuffer_Export* GenerateVertexBuffer(void) const;
	IIndexBuffer*  GenerateIndexBuffer(void) const;
	CVertDecl*  GenerateVertexDecleration(void) const;

	// generate shaders programs
	IShaderProgram_Export* GenerateShaderVertexProgram(void)   const;
	IShaderProgram_Export* GenerateShaderFragmentProgram(void) const;

	// generate texture
	ITexture*   GenerateTexture(void) const;

	// generate RenderToTexture
	IRenderToTexture*  GenerateRenderToTexture(void) const;

	void SetProjectionMatrix(float fFovy,float fAspect,float fZNear,float fZFar);

	// expects column based memory layout !
	void SetWorldMatrix(const float* pMat);
	void SetViewMatrix(const float* pMat);
	void SetProjectionMatrix(const float* pMat) ;

	// return column based memory layout
	const float* GetProjectionMatrix(void);
	const float* GetWorldMatrix(void);
	const float* GetViewMatrix(void);
	const float* GetModelViewMatrix(void);

	// update auto uniforms
	void UpdateAutoUniforms(void) ;

	
	// caps
	int GetSamplersNum(void) const { return m_iSamplersNum;};

	// samplers state

	void BindTexture(int iSamplerNum,CSmartPointer<ITexture>& tex);

	void UnBindTexUnit(int iSamplerNum);

	void BindVertexShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms);

	void UnBindVertexShader(void); // dummy - i should fill this

	void BindFragmentShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms);

	void UnBindFragmentShader(void); // dummy - i should fill this

	// binding Index/Vertex buffers

	void BindVB(IVertexBuffer_Export* pVB, const CVertDecl* pVD) ;
	void BindIB(const IIndexBuffer* pIB) ;
	

	void SetClamping(int iSamplerNum,char cClamping);

	void SetFiltering(int iSamplerNum,char cFiltering,char cAnisotropicLevel);

	// renderer state
	void  SetDepthTest(bool bDepthTest);
	DWORD GetDepthTest(void) const { return m_RendererState.m_bDepthTest;};

	void SetDepthWrite(bool bDepthWrite);
	DWORD GetDepthWrite(void) const { return m_RendererState.m_bDepthWrite;};

	void SetDepthFunc(DWORD dwDepthFunc);
	DWORD GetDepthFunc(void) const { return m_RendererState.m_dwDepthFunc; };

	void SetCullFunc(DWORD dwCullFunc);
	DWORD GetCullFunc(void) const {return m_RendererState.m_dwCullFunc;};

	void SetBlendFunc(char cSource,char cTarget);

	void SetAlphaFunc(DWORD dwFunc);
	void SetAlphaReferenceValue(DWORD dwVal);
	

	void SetViewport(int x, int y, int iWidth, int iHeight) const;

	void SetClearColor(float fR,float fG, float fB, float fA) ;

	void Clear(DWORD dwFlags) const;


	void EnableScissorRect(void)  ;
	void DisableScissorRect(void) ;
	void SetScissorRect(int x1, int y1, int x2, int y2) const;

	// debug

	void DebugRenderBBox(const BBox& bbox);

	void DrawDebugLine(const CVec3f& v1, const CVec3f& v2, const CVec3f& col) ;

	void DrawDebugLine2D(const CVec2f& v1, const CVec2f& v2) ;


	const tGfxAPIProf& GetProfileData() const { return m_ProfData; };

	void AddDebugText(const char* pFormat, ...) const;
	
	void printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...) const;

	void stopPrinting(void) {m_bPrintText=false;};
	void startPrinting(void) {m_bPrintText=true;};

private:

	bool m_bPrintText;

	LPDIRECT3D9					m_pDirect3DObject;	
	D3DPRESENT_PARAMETERS		m_PresentParameters;
	D3DCAPS9					m_DeviceCaps;
	//D3DXMATRIX					m_matProjection;

	D3DXMATRIX m_dxProjection,m_dxWorld,m_dxView;


	// matrices
	float* m_pfProjection;
	
	float* m_pfWorld;
	bool  m_bWorldIdentity;


	float* m_pfView;
	float* m_pfWorldView;

	// samplers states
	int            m_iSamplersNum;
	tSamplerState* m_pSamplers;

	// Vertex/Fragment programs
	CSmartPointer<IShaderProgram> m_ActiveVertexProgram;
	CSmartPointer<IShaderProgram> m_ActiveFragmentProgram;

	// Vertes/Index buffers
	IVertexBuffer_Export* m_pActiveVB;
	const IIndexBuffer*         m_pActiveIB;

	// renderer state
	tRendererState m_RendererState;

	float m_ClearColor[4];

	int m_iResX;
	int m_iResY;

	// helps only update uniforms when needed
	//bool m_bMatricesDirty;
	


	// used to prevent multiple rendering of same entity
	unsigned int m_uiFrameNum;

	// profiling
	tGfxAPIProf m_ProfData;

	mutable CDXDebugText m_DebugText;

	mutable std::string m_DebugString;
	mutable char TempDebugText[1000];
	char*        pDebugTextPos;


	IDirect3DSurface9* m_pMainDepthStencil;
};





#endif