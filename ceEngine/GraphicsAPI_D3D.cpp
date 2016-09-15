#include "stdafx.h"
#include "GraphicsAPI_D3D.h"

#include "LogFile.h"
#include "VertexBuffer_D3D.h"
#include "IndexBuffer_D3D.h"
#include "VertexDecleration_D3D.h"

#include "Defines.h"

#include "d3d9.h"
#include "d3dx9.h"

#include "EngineOptions.h"
extern CEngineOptions* g_pEngineOptions;

extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;

#include "Cg/cg.h"
#include <cg/cgD3D9.h>

extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;

#include "ShaderVertexProgram_D3D.h"
#include "ShaderFragmentProgram_D3D.h"

#include "GMath.h"
#include "MatrixGenerator.h"

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "Profiler.h"


#include "Texture_D3D.h"
#include "RenderToTexture_D3D.h"

#include "GraphicsAPI.h"
#include "GraphicsAPI_D3D.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include <assert.h>

extern void handleCgError();

CGraphicsAPI_D3D::CGraphicsAPI_D3D()
{
	m_bPrintText=true;
	m_uiFrameNum = 0;

	// profiling

	m_ProfData.iTextureBinds = 0;


	m_bWorldIdentity = false;

	m_pActiveVB = NULL;
	m_pActiveIB = NULL;


}

CGraphicsAPI_D3D::~CGraphicsAPI_D3D()
{
	cgDestroyContext(g_cgContext);
	SAFE_RELEASE(m_pDirect3DObject);
	SAFE_RELEASE(g_pDirect3DDevice);
}

void* CGraphicsAPI_D3D::GetDevice(void) const
{
	return &g_pDirect3DDevice;
}

bool CGraphicsAPI_D3D::Init(int iResX, int iResY, HWND hWnd)
{

	//create Direct3D object
	if((m_pDirect3DObject = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
 		CR_ERROR(1,"Direct3DCreate9() failed!"); 
 
	ZeroMemory(&m_PresentParameters,sizeof(m_PresentParameters));


	
	m_PresentParameters.Windowed = g_pEngineOptions->GetWindowed();
	m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_PresentParameters.EnableAutoDepthStencil = true;
	//m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	//m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D32;
    m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	//m_PresentParameters.AutoDepthStencilFormat = D3DFMT_R32F;
	
	m_PresentParameters.hDeviceWindow = hWnd;
	m_PresentParameters.BackBufferWidth = iResX;
	m_PresentParameters.BackBufferHeight = iResY;
	m_PresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	bool bVsync = false;
	m_PresentParameters.PresentationInterval = bVsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

	/*if(FAILED(m_pDirect3DObject->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
	                                            hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,
												&m_PresentParameters,&g_pDirect3DDevice)))
	 	CR_ERROR(1,"CreateDevice() failed!");*/

	    // Create the D3DDevice 
#ifndef  PERF_HUD
    if( FAILED( m_pDirect3DObject->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									  //D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                      &m_PresentParameters,&g_pDirect3DDevice)))
    { 
        CR_ERROR(1,"CreateDevice() failed!");
    } 
#else 
     // For NVPerfHud 
     if( FAILED( m_pDirect3DObject->CreateDevice( m_pDirect3DObject->GetAdapterCount()-1, D3DDEVTYPE_REF, hWnd, 
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                     &m_PresentParameters,&g_pDirect3DDevice)))
    { 
       CR_ERROR(1,"CreateDevice() failed!");
    } 
#endif

	g_pDirect3DDevice->GetDepthStencilSurface(&m_pMainDepthStencil);

	m_pfProjection = new float[16];
	m_pfWorld = new float[16];
	m_pfView = new float[16];
	m_pfWorldView = new float[16];
	

	SetProjectionMatrix(90.0f, (float)(iResX)/(float)(iResY),CR_NEAR_Z, CR_FAR_Z);
	m_iResX = iResX;
	m_iResY = iResY;


	D3DCAPS9 caps;

	g_pDirect3DDevice->GetDeviceCaps(&caps);


	/*glDepthFunc (GL_LEQUAL);	
	m_dwDepthFunc = CR_LESS_OR_EQUAL;
	
	glEnable (GL_DEPTH_TEST);	
	m_bDepthTest=true;*/

	m_iSamplersNum = caps.MaxSimultaneousTextures;

	m_pSamplers = new tSamplerState[m_iSamplersNum];



	HRESULT hr;

	for (int i=0;i<m_iSamplersNum;i++)
	{
		hr=g_pDirect3DDevice->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(i,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));

		m_pSamplers[i].cFiltering = SS_FILTERING_TRILINEAR;
		m_pSamplers[i].cAnisotropicLevel = 1;


		hr=g_pDirect3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		assert(SUCCEEDED(hr));

		m_pSamplers[i].cClamping = SS_CLAMPING_REPEAT;

		//m_pSamplers[i].pTex = NULL;

	}


	/*g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_RendererState.m_dwCullFunc = CR_BACK;*/

	g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_RendererState.m_dwCullFunc = CR_BACK;

	/*case CR_BACK: //reversed on purpose...
			g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
		break;*/

	/*g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	m_RendererState.m_dwCullFunc = CR_FRONT;*/

	/*g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_RendererState.m_dwCullFunc = CR_NONE;*/

	

	g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	m_RendererState.m_bDepthTest=true;


	g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,D3DZB_TRUE);
	m_RendererState.m_bDepthWrite=true;
	
	g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);			
	m_RendererState.m_dwDepthFunc = CR_LESS_OR_EQUAL;



	hr=g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	assert(SUCCEEDED(hr));

	hr=g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	assert(SUCCEEDED(hr));


	m_RendererState.m_cBlendSource = CR_BF_ONE;
	m_RendererState.m_cBlendTarget = CR_BF_ZERO;
	
	hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,0);
	assert(SUCCEEDED(hr));

	hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_LESSEQUAL);
	assert(SUCCEEDED(hr));

	

	// debug
	//SetBlendFunc(CR_BF_ONE,CR_BF_ONE);


	g_pDirect3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,0),1.0f,0);
	g_pDirect3DDevice->Present(NULL,NULL,NULL,NULL);



	
	//////////////////////////////////



	////////
	// CG
	////////

	// set the error callback
	cgSetErrorCallback(handleCgError);

	// create cg context
	g_cgContext = cgCreateContext();	

	cgD3D9SetDevice( g_pDirect3DDevice );


	g_VertexProfile=cgD3D9GetLatestVertexProfile();  
	g_FragmentProfile=cgD3D9GetLatestPixelProfile();

	char temp[1024];

	sprintf(temp,"Latest Vertex Profile: %s\n", cgGetProfileString(g_VertexProfile));
	OutputDebugStringA(temp);

	sprintf(temp,"Latest Pixel Profile: %s\n", cgGetProfileString(g_FragmentProfile));
	OutputDebugStringA(temp);


	/*CGprofile profile;
	int nProfiles;
	int ii;

	nProfiles = cgGetNumSupportedProfiles();

	
	sprintf(temp,"NumSupportedProfiles: %i\n", nProfiles);
	OutputDebugStringA(temp);
	
	for (ii=0; ii<nProfiles; ++ii) 
	{
		profile = cgGetSupportedProfile(ii);
		sprintf(temp,"SupportedProfile %i: %s %i\n", ii, cgGetProfileString(profile),
								   cgGetProfile(cgGetProfileString(profile)));
		OutputDebugStringA(temp);
	}*/


	//CG_PROFILE_PS_2_X

	m_DebugText.Init(g_pDirect3DDevice);

	return true;
}

void CGraphicsAPI_D3D::OnResize(int iResX, int iResY)
{
	// REMEMBER TO ADD CHANGES TO POST PROCESSING RENDER TARGETS!

	static bool bFirstResize=true;

	if (!bFirstResize)
	{
		SetProjectionMatrix(90.0f, (float)(iResX)/(float)(iResY),CR_NEAR_Z, CR_FAR_Z);
		m_iResX = iResX;
		m_iResY = iResY;
	}
		
	bFirstResize=false;
}

void CGraphicsAPI_D3D::UnBindTexUnit(int iSamplerNum)
{	
		g_pDirect3DDevice->SetTexture(iSamplerNum, NULL); 
		m_pSamplers[iSamplerNum].iTextureType = CR_TEXTURE_OFF;	
}

void CGraphicsAPI_D3D::BeginFrame(void)
{		
	SetViewport(0,0,m_iResX,m_iResY);

	SetClearColor(0.f,0.f,0.f,0.f);
	Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);

	SetProjectionMatrix(90.0f, (float)(m_iResX)/(float)(m_iResY),CR_NEAR_Z, CR_FAR_Z);

	for (int i=0;i<API_TEX_UNITS_NUM;i++)
	{
		UnBindTexUnit(i);
	}
	
	// profiling

	m_ProfData.iTextureBinds = 0;


	//g_pDirect3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	
	// do things that require the clipping

	//g_pDirect3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE , FALSE);

	
}

void CGraphicsAPI_D3D::EnableScissorRect(void) 
{
	if (!m_RendererState.bScissorRect)
	{
		m_RendererState.bScissorRect = true;
		g_pDirect3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE , TRUE);
	}
}

void CGraphicsAPI_D3D::DisableScissorRect(void) 
{
	if (m_RendererState.bScissorRect)
	{
		m_RendererState.bScissorRect = false;
		g_pDirect3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE , FALSE);
	}
}

void CGraphicsAPI_D3D::SetScissorRect(int x1, int y1, int x2, int y2) const
{
	RECT clipRect;
	
	clipRect.left = min(x1,x2);
	clipRect.top = m_iResY - min(y1,y2);

	clipRect.bottom = m_iResY - max(y1,y2);



	clipRect.right = max(x1,x2);


	
	
	HRESULT hr;

	BREAKPOINT(clipRect.left<0);
	BREAKPOINT(clipRect.left>m_iResX);
	
	BREAKPOINT(clipRect.right<0);
	BREAKPOINT(clipRect.right>m_iResX);

	BREAKPOINT(clipRect.top<0);
	BREAKPOINT(clipRect.top>m_iResX);

	BREAKPOINT(clipRect.bottom<0);
	BREAKPOINT(clipRect.bottom>m_iResY);
	

	hr = g_pDirect3DDevice->SetScissorRect(&clipRect);
	assert(SUCCEEDED(hr));
}


IVertexBuffer_Export* CGraphicsAPI_D3D::GenerateVertexBuffer(void) const
{
	IVertexBuffer* pVB = new CVertexBuffer_D3D();
	return pVB;	
}

void CGraphicsAPI_D3D::Present(void)
{		
	g_pDirect3DDevice->Present(NULL,NULL,NULL,NULL);	
	m_uiFrameNum++;
}

IIndexBuffer*  CGraphicsAPI_D3D::GenerateIndexBuffer(void) const
{
	IIndexBuffer* pIB = new CIndexBuffer_D3D();
	return pIB;	
}

CVertDecl*  CGraphicsAPI_D3D::GenerateVertexDecleration(void) const
{
	CVertDecl* pVD = new CVertDecl_D3D();
	return pVD;

}

IShaderProgram_Export* CGraphicsAPI_D3D::GenerateShaderVertexProgram(void)   const
{
	IShaderProgram* pSVP = new CShaderVertexProgram_D3D();
	return pSVP;
}

IShaderProgram_Export* CGraphicsAPI_D3D::GenerateShaderFragmentProgram(void) const
{
	IShaderProgram* pSVP = new CShaderFragmentProgram_D3D();
	return pSVP;
}

ITexture*   CGraphicsAPI_D3D::GenerateTexture(void) const
{
	// no managers for now, always assumes new texture
	ITexture* ans = new CTexture_D3D();
	return ans;
}

// generate RenderToTexture
IRenderToTexture*  CGraphicsAPI_D3D::GenerateRenderToTexture(void) const
{
	IRenderToTexture* ans = new CRenderToTexture_D3D();
	return ans;
}

void CGraphicsAPI_D3D::UpdateAutoUniforms(void) 
{

	static CProfileStep step("UpdateAU:");	
	step.Begin();


	static float* pfMatrix = new float[16];
	
	CMatrix proj,world,view;

	proj.Set(g_pGraphicsAPI->GetProjectionMatrix());
	world.Set(g_pGraphicsAPI->GetWorldMatrix());
	view.Set(g_pGraphicsAPI->GetViewMatrix());			


	/////////////////////////////////////////////////////////////////////
	// "modelViewProjection"
	(proj*view*world).ReturnArray(pfMatrix);
	tUniform* pAutoUniform = g_pAutoUniforms->Get("worldViewProjection");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// "world"
	(world).ReturnArray(pfMatrix);
	pAutoUniform = g_pAutoUniforms->Get("world");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// "view"
	(view).ReturnArray(pfMatrix);
	pAutoUniform = g_pAutoUniforms->Get("view");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// "projection"
	(proj).ReturnArray(pfMatrix);
	pAutoUniform = g_pAutoUniforms->Get("projection");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// "viewProjection"
	(proj*view).ReturnArray(pfMatrix);
	pAutoUniform = g_pAutoUniforms->Get("viewProjection");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// "worldView"
	(view*world).ReturnArray(pfMatrix);
	pAutoUniform = g_pAutoUniforms->Get("worldView");
	if (pAutoUniform)
	{
		if (pAutoUniform->m_pfValue)
			memcpy(pAutoUniform->m_pfValue,pfMatrix,sizeof(float)*16);
		else 
			BREAKPOINT(1);
	}
	//////////////////////////////////////////////////////////////////////

	step.End();
}


void CGraphicsAPI_D3D::SetProjectionMatrix(float fFovy,float fAspect,float fZNear,float fZFar)
{
//	m_bMatricesDirty = true;
	//CMatrixGenerator::PespectiveLH(m_pfProjection,fFovy,fAspect,fZNear,fZFar);

	CMatrixGenerator::PespectiveRH(m_pfProjection,fFovy,fAspect,fZNear,fZFar);

	/*D3DXMATRIX proj(m_pfProjection);
	
	g_pDirect3DDevice->SetTransform(D3DTS_PROJECTION,&proj);*/

	UpdateAutoUniforms();
}

void CGraphicsAPI_D3D::SetProjectionMatrix(const float* pMat) 
{
//	m_bMatricesDirty = true;
	

	if (pMat)
		memcpy(m_pfProjection,pMat,sizeof(float)*16);
	else
	{
		CMatrix ident;
		ident.MatMakeUnit();		
		ident.ReturnArray(m_pfProjection);
	
	}
	/*D3DXMATRIX proj(pMat);	
	g_pDirect3DDevice->SetTransform(D3DTS_PROJECTION,&proj);*/

	UpdateAutoUniforms();
}

void CGraphicsAPI_D3D::SetWorldMatrix(const float* pMat)
{	

	if (pMat)
	{
		m_bWorldIdentity = false;
		memcpy(m_pfWorld,pMat,sizeof(float)*16);
		/*D3DXMATRIX world(pMat);		
		g_pDirect3DDevice->SetTransform(D3DTS_WORLD,&world);		*/
	}
	else
	{	
		if (!m_bWorldIdentity) // only if it wasn't already identity
		{		
			CMatrix ident;
			ident.MatMakeUnit();		
			ident.ReturnArray(m_pfWorld);
		}
		//ident.ReturnArray(m_pfWorld);
	}
	

	if (pMat==NULL && m_bWorldIdentity)
		return;


	if (!pMat)
		m_bWorldIdentity = true;

	UpdateAutoUniforms();
}




void CGraphicsAPI_D3D::SetViewMatrix(const float* pMat)
{
//	m_bMatricesDirty = true;
	if (pMat)
	{
		memcpy(m_pfView,pMat,sizeof(float)*16);
/*		D3DXMATRIX view(pMat);
		g_pDirect3DDevice->SetTransform(D3DTS_WORLD,&view);			*/
	}
	else
	{
		//BREAKPOINT(1);
		/*D3DXMATRIX view;
		D3DXMatrixIdentity(&view);
		g_pDirect3DDevice->SetTransform(D3DTS_VIEW,&view);	*/

		CMatrix ident;
		ident.MatMakeUnit();
		ident.ReturnArray(m_pfView);

	}

	UpdateAutoUniforms();
}


const float* CGraphicsAPI_D3D::GetWorldMatrix(void)
{	
/*    g_pDirect3DDevice->GetTransform( D3DTS_WORLD, &m_dxWorld );	
	m_pfWorld = (FLOAT*) m_dxWorld;*/
	return m_pfWorld;

}

const float* CGraphicsAPI_D3D::GetViewMatrix(void)
{
/*    g_pDirect3DDevice->GetTransform( D3DTS_VIEW, &m_dxView );	
	m_pfView = (FLOAT*) m_dxView;*/
	return m_pfView;
}

const float* CGraphicsAPI_D3D::GetProjectionMatrix(void)
{
/*	g_pDirect3DDevice->GetTransform( D3DTS_PROJECTION, &m_dxProjection );	
	m_pfProjection = (FLOAT*) m_dxProjection;*/
	return m_pfProjection;
}

const float* CGraphicsAPI_D3D::GetModelViewMatrix(void)
{
	// threads...
	static float* pfMatrix = new float[16];

	CMatrix proj,world,view;
	world.Set(g_pGraphicsAPI->GetWorldMatrix());
	view.Set(g_pGraphicsAPI->GetViewMatrix());	

	(view*world).ReturnArray(m_pfWorldView);
	
	return m_pfWorldView;
}


void  CGraphicsAPI_D3D::SetDepthTest(bool bDepthTest)
{
	if (m_RendererState.m_bDepthTest!=bDepthTest)
	{
		m_RendererState.m_bDepthTest=bDepthTest;

		if (m_RendererState.m_bDepthTest)
			g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
		else
			g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	}
}

void CGraphicsAPI_D3D::SetDepthWrite(bool bDepthWrite)
{
	if (m_RendererState.m_bDepthWrite!=bDepthWrite)
	{
		m_RendererState.m_bDepthWrite=bDepthWrite;

		if (m_RendererState.m_bDepthWrite)
			g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,D3DZB_TRUE);
		else
			g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,D3DZB_FALSE);
	}
}


void CGraphicsAPI_D3D::SetDepthFunc(DWORD dwDepthFunc)
{
	if (m_RendererState.m_dwDepthFunc!=dwDepthFunc)
	{
		m_RendererState.m_dwDepthFunc = dwDepthFunc;

		switch(m_RendererState.m_dwDepthFunc)
		{
		case CR_ALWAYS:
			g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);		
		break;

		case CR_LESS:
			g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);			
		break;

		case CR_LESS_OR_EQUAL:
			g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);			
		break;

		case CR_EQUAL:
			g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);			
		break;
		}

	}

}



void  CGraphicsAPI_D3D::SetCullFunc(DWORD dwCullFunc)
{
	/*
	// DEBUG
	g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	return;
*/

	if (m_RendererState.m_dwCullFunc!=dwCullFunc)
	{
		m_RendererState.m_dwCullFunc = dwCullFunc;

		switch(m_RendererState.m_dwCullFunc)
		{
		case CR_FRONT: //reversed on purpose...
			g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
		break;


		case CR_BACK: //reversed on purpose...
			g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		break;

		case CR_NONE:
			g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		break;
		}
	}
}

// debug

void CGraphicsAPI_D3D::DebugRenderBBox(const BBox& bbox)
{
	//TODO: add this	
}

void CGraphicsAPI_D3D::DrawDebugLine(const CVec3f& v1, const CVec3f& v2, const CVec3f& col) 
{
	//TODO: add this	
}

void CGraphicsAPI_D3D::DrawDebugLine2D(const CVec2f& v1, const CVec2f& v2) 
{
	//TODO: add this	
}

void CGraphicsAPI_D3D::BindVertexShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms)
{
	// remember - there's a problem when rendering only one vertex shader in whole scene!

	//TODO: add a path that knows if there's a need to change only uniforms


	if (m_ActiveVertexProgram != spShaderProgram)
	{
		m_ActiveVertexProgram = spShaderProgram;
		spShaderProgram->Bind(pUniforms);
	}
	else
		spShaderProgram->RebindParameters(pUniforms);

}

void CGraphicsAPI_D3D::UnBindVertexShader(void)
{
	// dummy - i should fill this
}

void CGraphicsAPI_D3D::BindFragmentShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms)
{
	// remember - there's a problem when rendering only one fragment shader in whole scene!
	if (m_ActiveFragmentProgram != spShaderProgram)
	{
		m_ActiveFragmentProgram = spShaderProgram;
		spShaderProgram->Bind(pUniforms);
	}
	else
		spShaderProgram->RebindParameters(pUniforms);
}

void CGraphicsAPI_D3D::UnBindFragmentShader(void)
{
	// dummy - i should fill this
}


void CGraphicsAPI_D3D::BindTexture(int iSamplerNum,CSmartPointer<ITexture>& tex)
{
	// restore - remember it creates problems for proxy because of the pointers comparison!
	if (m_pSamplers[iSamplerNum].spTex!=tex 
		|| (m_pSamplers[iSamplerNum].iTextureType == CR_TEXTURE_OFF) 
		|| tex->GetForceReBind() )
	{
		m_pSamplers[iSamplerNum].spTex = tex;
		tex->Bind(iSamplerNum);
		m_pSamplers[iSamplerNum].iTextureType = CR_TEXTURE_2D;

		// profiling
		m_ProfData.iTextureBinds++;
	}
}
// samplers state
void CGraphicsAPI_D3D::SetClamping(int iSamplerNum,char cClamping)
{
	if (m_pSamplers[iSamplerNum].cClamping!=cClamping)
	{
		m_pSamplers[iSamplerNum].cClamping = cClamping;

		HRESULT hr;

		switch (m_pSamplers[iSamplerNum].cClamping)
		{
		case SS_CLAMPING_REPEAT:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		assert(SUCCEEDED(hr));
		break;
		case SS_CLAMPING_CLAMP:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
		assert(SUCCEEDED(hr));
		break;
		}
	}
}

void CGraphicsAPI_D3D::SetAlphaReferenceValue(DWORD dwVal)
{
	//TODO: prevent redundancy
	HRESULT hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAREF,dwVal);
	assert(SUCCEEDED(hr));
}

void CGraphicsAPI_D3D::SetAlphaFunc(DWORD dwFunc)
{
	HRESULT hr = S_OK;
	switch (dwFunc)
	{
	case CR_LESS:
		if (m_RendererState.m_dwAlphaFunc != CR_LESS)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_LESS);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_LESS;
		}
		break;
	case CR_LESS_OR_EQUAL:
		if (m_RendererState.m_dwAlphaFunc != CR_LESS_OR_EQUAL)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_LESS_OR_EQUAL;
		}
		break;
	case CR_EQUAL:   
		if (m_RendererState.m_dwAlphaFunc != CR_EQUAL)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_EQUAL);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_EQUAL;
		}
		break;
	case CR_ALWAYS:
		if (m_RendererState.m_dwAlphaFunc != CR_ALWAYS)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
			assert(SUCCEEDED(hr));

			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_ALWAYS;
		}
		break;
	case CR_NEVER:
		if (m_RendererState.m_dwAlphaFunc != CR_NEVER)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_NEVER);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_NEVER;
		}
		break;
	case CR_GREATER:
		if (m_RendererState.m_dwAlphaFunc != CR_GREATER)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_GREATER;
		}
		break;

	case CR_GREATER_OR_EQUAL:
		if (m_RendererState.m_dwAlphaFunc != CR_GREATER_OR_EQUAL)
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
			assert(SUCCEEDED(hr));
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			assert(SUCCEEDED(hr));

			m_RendererState.m_dwAlphaFunc = CR_GREATER_OR_EQUAL;
		}
		break;
	}


}

void CGraphicsAPI_D3D::SetBlendFunc(char cSource,char cTarget)
{
	if ( (m_RendererState.m_cBlendSource!=cSource) ||
		 (m_RendererState.m_cBlendTarget!=cTarget))
	{
		m_RendererState.m_cBlendSource=cSource;
	    m_RendererState.m_cBlendTarget=cTarget;

		HRESULT hr;

		// add opts to prevent redundant stuff here
		//if ( (cSource==CR_BF_ZERO) &&(cTarget==CR_BF_ONE))
		if ( (cSource==CR_BF_ONE) &&(cTarget==CR_BF_ZERO))
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,0);
			assert(SUCCEEDED(hr));
			return;
		}
		else
		{
			hr=g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,1);
			assert(SUCCEEDED(hr));			
		}

		DWORD dwTranslatedSource;

		switch (cSource)
		{
		case CR_BF_ZERO:	            dwTranslatedSource= D3DBLEND_ZERO;	       break;
		case CR_BF_ONE:	            	dwTranslatedSource= D3DBLEND_ONE;           break;
		case CR_BF_DST_COLOR:	     	dwTranslatedSource= D3DBLEND_DESTCOLOR;    break;
		case CR_BF_ONE_MINUS_DST_COLOR:	dwTranslatedSource= D3DBLEND_INVDESTCOLOR; break;
		case CR_BF_SRC_ALPHA:		    dwTranslatedSource= D3DBLEND_SRCALPHA;     break;
		case CR_BF_ONE_MINUS_SRC_COLOR:	dwTranslatedSource= D3DBLEND_INVSRCCOLOR;  break;
		case CR_BF_DST_ALPHA:			dwTranslatedSource= D3DBLEND_DESTALPHA;    break;
		case CR_BF_ONE_MINUS_DST_ALPHA:	dwTranslatedSource= D3DBLEND_INVDESTALPHA; break;
		case CR_BF_SRC_ALPHA_SATURATE:	dwTranslatedSource= D3DBLEND_SRCALPHASAT;  break;
		case CR_BF_ONE_MINUS_SRC_ALPHA:	dwTranslatedSource= D3DBLEND_INVSRCALPHA;  break;
		case CR_BF_SRC_COLOR:			dwTranslatedSource= D3DBLEND_SRCCOLOR;     break;
		default:
			CR_WARNING0(1,"CGraphicsAPI_D3D::SetBlendFunc - unrecognized blend source value! defaulting to CR_ONE");
			dwTranslatedSource=D3DBLEND_ONE;
		// Invalid Blendfunc
		};

		DWORD dwTranslatedTarget;

		switch (cTarget)
		{
		case CR_BF_ZERO:	            dwTranslatedTarget=D3DBLEND_ZERO;	       break;
		case CR_BF_ONE:	            	dwTranslatedTarget=D3DBLEND_ONE;           break;
		case CR_BF_DST_COLOR:	     	dwTranslatedTarget= D3DBLEND_DESTCOLOR;    break;
		case CR_BF_ONE_MINUS_DST_COLOR:	dwTranslatedTarget= D3DBLEND_INVDESTCOLOR; break;
		case CR_BF_SRC_ALPHA:		    dwTranslatedTarget= D3DBLEND_SRCALPHA;     break;
		case CR_BF_ONE_MINUS_SRC_COLOR:	dwTranslatedTarget= D3DBLEND_INVSRCCOLOR;  break;
		case CR_BF_DST_ALPHA:			dwTranslatedTarget= D3DBLEND_DESTALPHA;    break;
		case CR_BF_ONE_MINUS_DST_ALPHA:	dwTranslatedTarget= D3DBLEND_INVDESTALPHA; break;
		case CR_BF_SRC_ALPHA_SATURATE:	dwTranslatedTarget= D3DBLEND_SRCALPHASAT;  break;
		case CR_BF_ONE_MINUS_SRC_ALPHA:	dwTranslatedTarget= D3DBLEND_INVSRCALPHA;  break;
		case CR_BF_SRC_COLOR:			dwTranslatedTarget= D3DBLEND_SRCCOLOR;     break;
		default:
			CR_WARNING0(1,"CGraphicsAPI_D3D::SetBlendFunc - unrecognized blend target value! defaulting to CR_ONE");
			dwTranslatedTarget=D3DBLEND_ONE;
		// Invalid Blendfunc
		};



		hr=g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND,dwTranslatedSource);
		assert(SUCCEEDED(hr));		

		hr=g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND,dwTranslatedTarget);
		assert(SUCCEEDED(hr));		



	}

}

void CGraphicsAPI_D3D::SetFiltering(int iSamplerNum,char cFiltering,char cAnisotropicLevel)
{
	if (m_pSamplers[iSamplerNum].cFiltering!=cFiltering)
	{
		m_pSamplers[iSamplerNum].cFiltering = cFiltering;

		HRESULT hr;
		

		switch (m_pSamplers[iSamplerNum].cFiltering)
		{
		case SS_FILTERING_NEAREST:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		break;

		


		case SS_FILTERING_NEAREST_NO_MIPMAPS:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
		assert(SUCCEEDED(hr));
		break;


		case SS_FILTERING_TRILINEAR:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));		
		break;

		case SS_FILTERING_BILINEAR:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
		assert(SUCCEEDED(hr));
		break;


		case SS_FILTERING_ANISOTROPIC:
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAGFILTER,D3DTEXF_ANISOTROPIC);
		assert(SUCCEEDED(hr));
		hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		assert(SUCCEEDED(hr));


		if (m_pSamplers[iSamplerNum].cAnisotropicLevel!=cAnisotropicLevel)
		{
			m_pSamplers[iSamplerNum].cAnisotropicLevel = cAnisotropicLevel;
			hr=g_pDirect3DDevice->SetSamplerState(iSamplerNum,D3DSAMP_MAXANISOTROPY,m_pSamplers[iSamplerNum].cAnisotropicLevel);
			assert(SUCCEEDED(hr));
		}

		break;

		default:
			BREAKPOINT(1);
		}
	}

}
void CGraphicsAPI_D3D::BindVB(IVertexBuffer_Export* pVB, const CVertDecl* pVD)
{
	//if (m_pActiveVB!=pVB)
	{
		m_pActiveVB = pVB;		

		pVD->Bind();
		((IVertexBuffer*)pVB)->BindStreams(pVD);

	}
	
}

void CGraphicsAPI_D3D::BindIB(const IIndexBuffer* pIB)
{
	//TODO: i need to add some test to see if this is a dynamic IndexBuffer, 
	// and do bind if that's the case!
	// just add some flag to the IB if it's dynamic or not

	
		

	//if (m_pActiveIB!=pIB)
	{
		m_pActiveIB = pIB;
		pIB->Bind();		
		
	}


}

void CGraphicsAPI_D3D::DrawPrimitives(int iIndicesStart,int iIndicesCount,int iBaseVertexIndex, int iMinIndex,int iVerticesNum) const
{
	HRESULT hr;

	// TODO - make this global
	static LPDIRECT3DDEVICE9 pDevice = *((LPDIRECT3DDEVICE9*)g_pGraphicsAPI->GetDevice());

	hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,iBaseVertexIndex,iMinIndex,iVerticesNum,iIndicesStart,iIndicesCount/3);		

	assert(SUCCEEDED(hr));

}

void CGraphicsAPI_D3D::Clear(DWORD dwFlags) const
{
	DWORD dwDXFlags=0;
	if (dwFlags&CR_CLEAR_COLOR)
		dwDXFlags|=D3DCLEAR_TARGET;
	if (dwFlags&CR_CLEAR_DEPTH)
		dwDXFlags|=D3DCLEAR_ZBUFFER;
	if (dwFlags&CR_CLEAR_STENCIL)
		dwDXFlags|=D3DCLEAR_STENCIL;

	
	//TODO: support custom color/depth/stencil value!

	//g_pDirect3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		//D3DCOLOR_XRGB(0,0,0),1.0f,0);

	HRESULT hr=g_pDirect3DDevice->Clear(0,NULL,dwDXFlags, 
		//D3DCOLOR_RGBA(m_ClearColor[0],m_ClearColor[1],m_ClearColor[2],m_ClearColor[3])

		D3DCOLOR_RGBA(int(255*m_ClearColor[0]),int(255*m_ClearColor[1]),int(255*m_ClearColor[2]),int(255*m_ClearColor[3])),
		1.f,0
		);


	

	assert(SUCCEEDED(hr));


}

void CGraphicsAPI_D3D::SetClearColor(float fR,float fG, float fB, float fA) 
{
	m_ClearColor[0] = fR;
	m_ClearColor[1] = fG;
	m_ClearColor[2] = fB;
	m_ClearColor[3] = fA;

}

void CGraphicsAPI_D3D::SetViewport(int x, int y, int iWidth, int iHeight) const
{
	D3DVIEWPORT9 vp;
	g_pDirect3DDevice->GetViewport(&vp);
	vp.X=x;
	vp.Y=y;
	vp.Width=iWidth;
	vp.Height=iHeight;
	g_pDirect3DDevice->SetViewport(&vp);
}

void CGraphicsAPI_D3D::BeginScene() const
{
	g_pDirect3DDevice->BeginScene();	

	//g_pDirect3DDevice->SetDepthStencilSurface(m_pMainDepthStencil);

	/*EnableScissorRect();
	SetScissorRect(0,0,300,300);*/
}

void CGraphicsAPI_D3D::EndScene() const
{

	// RESTORE!
	m_DebugText.printf(CVec3f(0.f,0.f,1.f),600,50,m_DebugString.c_str());

	

	m_DebugString.clear();


	g_pDirect3DDevice->EndScene();
}

void CGraphicsAPI_D3D::AddDebugText(const char* pFormat, ...) const
{


	if (!m_bPrintText)
		return;

	if (m_DebugString.length()>1000)
		return;

	va_list argumentPtr;

	if (!pFormat) return;
	va_start(argumentPtr, pFormat);
	vsprintf(TempDebugText, pFormat, argumentPtr);
	va_end(argumentPtr);
	

	

	m_DebugString += TempDebugText;

}

void CGraphicsAPI_D3D::printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...) const
{	
	va_list argumentPtr;

	if (!pFormat) return;
	va_start(argumentPtr, pFormat);
	vsprintf(TempDebugText, pFormat, argumentPtr);
	va_end(argumentPtr);

	m_DebugText.printf(vCol,x,y,TempDebugText);

}