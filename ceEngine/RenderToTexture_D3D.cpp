#include "stdafx.h"
#include "RenderToTexture_D3D.h"

#include "Singleton.h"


#include <string>
#include <assert.h>

#include "ce.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////////////////////////////////////////
// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
#include "TextureManager.h"

extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;


LPDIRECT3DSURFACE9 CRenderToTexture_D3D::m_pBack = NULL;

CRenderToTexture_D3D::CRenderToTexture_D3D()
{
	m_pRenderToSurface = NULL;
    m_pDynamicTexture  = NULL;
	m_pTextureSurface  = NULL;
}

CRenderToTexture_D3D::~CRenderToTexture_D3D()
{
	SAFE_RELEASE(m_pRenderToSurface);
	SAFE_RELEASE(m_pDynamicTexture);
	SAFE_RELEASE(m_pTextureSurface);
}


void CRenderToTexture_D3D::Init(const char* pName, DWORD dwType,int iWidth, int iHeight)
{
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_dwType = dwType;

	
	m_pTexName = new char[strlen(pName)+1];
	strcpy(m_pTexName,pName);




	//
	// Create our dynamic texture for use by the "render to" surface...
	//

	HRESULT hr;

	/*
	    V_RETURN( pd3dDevice->CreateTexture( ShadowMap_SIZE, ShadowMap_SIZE,
                                         1, D3DUSAGE_RENDERTARGET,
                                         D3DFMT_R32F,
                                         D3DPOOL_DEFAULT,
                                         &g_pShadowMap,
                                         NULL ) );
										 */

	if (m_dwType==CR_RTT_SHADOW_MAP)
	{
		m_bDepth = true;

		// RESTORE RESTORE

		/*g_pDirect3DDevice->CreateTexture( iWidth, iHeight,
                                         1, D3DUSAGE_RENDERTARGET,
                                         D3DFMT_R32F,
                                         D3DPOOL_DEFAULT,
                                         &m_pDynamicTexture,
                                         NULL ) ;*/


		hr = D3DXCreateTexture( g_pDirect3DDevice, 
		                    iWidth, 
		                    iHeight, 
							1, 
							// when using AUTOGENMIPMAP, it fails when trying to
							// bind a rtt and render into ANOTHER rtt :/
							//D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, 
							D3DUSAGE_RENDERTARGET ,
							D3DFMT_R32F,
							D3DPOOL_DEFAULT, 
							&m_pDynamicTexture );


		
		


	} else if (m_dwType==CR_RTT_COLOR_AND_DEPTH)
	{
		m_bDepth = false;

	
		 /*V_RETURN( D3DXCreateTexture( pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 
                                 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                  D3DPOOL_DEFAULT, &g_pFullScreenRenderTarget ) );*/

		//hr = g_pDirect3DDevice->CreateTexture(iWidth,iHeight,1,D3DUSAGE_RENDERTARGET,desc.Format,D3DPOOL_DEFAULT,&m_pDynamicTexture,NULL);


		hr = g_pDirect3DDevice->CreateTexture(iWidth,iHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pDynamicTexture,NULL);

		/*hr = D3DXCreateTexture( g_pDirect3DDevice, 
		                    iWidth, 
		                    iHeight, 
							1, 
							// when using AUTOGENMIPMAP, it fails when trying to
							// bind a rtt and render into ANOTHER rtt :/
							// original
							D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, 
							//D3DUSAGE_RENDERTARGET ,
							D3DFMT_A8R8G8B8, 
							D3DPOOL_DEFAULT, 
							&m_pDynamicTexture );*/

		
		
	} else
	{
		
		g_pLogFile->OutPutPlainText("CRenderToTexture_D3D::Init - unrecognized rtt init type value - defaulting to color+depth!",LOG_MESSAGE_ERROR);				
		BREAKPOINT(1);
	}

	
	if( FAILED(hr) )
    {
       g_pLogFile->OutPutPlainText("Failed to Create an off-screen render-to surface!","3","990000",true,LOG_MESSAGE_ERROR);
		return;
    }

	


	D3DSURFACE_DESC desc;
	m_pDynamicTexture->GetSurfaceLevel( 0, &m_pTextureSurface );	
	m_pTextureSurface->GetDesc( &desc );
	

	/*IDirect3DSurface9* origTarget = NULL;
	g_pDirect3DDevice->GetRenderTarget( 0, &origTarget );
    D3DSURFACE_DESC desc;
    origTarget->GetDesc( &desc );*/

	

	if (m_dwType==CR_RTT_SHADOW_MAP)
	{
		// original
		hr = D3DXCreateRenderToSurface( g_pDirect3DDevice, 
	                            desc.Width, 
	                            desc.Height, 
	                            desc.Format, 
	                            TRUE, 
	                            //D3DFMT_D16, 
								//D3DFMT_D32, 
								D3DFMT_D24S8, // restore thissssss
								//D3DFMT_D16,
								//D3DFMT_R32F,
	                            &m_pRenderToSurface );
		
	} else if (m_dwType==CR_RTT_COLOR_AND_DEPTH)
	{
		/*hr = D3DXCreateRenderToSurface( g_pDirect3DDevice, 
									desc.Width, 
									desc.Height, 
									desc.Format, 
									//false,
									TRUE, 
								   //D3DFMT_UNKNOWN,
								   D3DFMT_D24S8,
								   //D3DFMT_R32F,
									&m_pRenderToSurface );*/

		hr = m_pDynamicTexture->GetSurfaceLevel(0, &m_pTextureSurface_RT );

		//hr = g_pDirect3DDevice->CreateRenderTarget(desc.Width,desc.Height,desc.Format,desc.MultiSampleType,desc.MultiSampleQuality,false,&m_pTextureSurface_RT,NULL);
		
	 	
		
	}


	if( FAILED(hr) )
    {
       g_pLogFile->OutPutPlainText("Failed to Create an off-screen render-to surface!","3","990000",true,LOG_MESSAGE_ERROR);
		return;
    }

	
}



void CRenderToTexture_D3D::BindRenderTo(/*int iSamplerNum*/) const
{


	HRESULT hr;

	if (m_bDepth)
	{
		hr= m_pRenderToSurface->BeginScene( m_pTextureSurface, NULL );
	}
	else
	{
		if (!m_pBack)
			hr = g_pDirect3DDevice->GetRenderTarget(0,(LPDIRECT3DSURFACE9*)&m_pBack);
		hr = g_pDirect3DDevice->SetRenderTarget( 0, m_pTextureSurface_RT );
		g_pDirect3DDevice->BeginScene();
	}
	
	
	assert(SUCCEEDED(hr));	
    
	g_pGraphicsAPI->SetViewport(0,0,m_iWidth,m_iHeight);
	

	
	//SetSamplerState

	
	if (m_dwType==CR_RTT_SHADOW_MAP)
	{
		g_pGraphicsAPI->SetClearColor(1.f,1.f,1.f,1.f); // far as possible
		//g_pGraphicsAPI->Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);	 
		g_pGraphicsAPI->Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);	 
		
	}
	else
	{
		//g_pGraphicsAPI->SetClearColor(0.f,0.f,0.f,0.f);
		g_pGraphicsAPI->SetClearColor(0.f,0.f,0.f,0.f);
		g_pGraphicsAPI->Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);	 
	}

	// original
	
	
	

	//g_pGraphicsAPI->Clear(CR_CLEAR_COLOR );	 
}

void CRenderToTexture_D3D::UnbindRenderTo() const
{
	HRESULT hr;

	

	if (m_bDepth)
	{
		hr = m_pRenderToSurface->EndScene( 0);
	}
	else
	{
		g_pDirect3DDevice->EndScene();
		//hr = g_pDirect3DDevice->SetRenderTarget( 0, NULL );

				
		hr = g_pDirect3DDevice->SetRenderTarget(0,m_pBack);
		g_pGraphicsAPI->SetViewport(0,0,g_pGraphicsAPI->GetResX(), g_pGraphicsAPI->GetResY());

		D3DSURFACE_DESC desc;
		m_pBack->GetDesc( &desc );

		int k=0;
		k++;

	}

	 assert(SUCCEEDED(hr));
}

void CRenderToTexture_D3D::Bind(int iSamplerNum) const
{
	HRESULT hr = g_pDirect3DDevice->SetTexture(iSamplerNum,m_pDynamicTexture);
	assert(SUCCEEDED(hr));
}



const char* CRenderToTexture_D3D::GetName(void) const
{
	return m_pTexName;
}
