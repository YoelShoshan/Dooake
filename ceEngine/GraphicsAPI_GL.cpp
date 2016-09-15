#include "stdafx.h"
#include "GraphicsAPI_GL.h"



#include <gl\gl.h>	
#include <gl\glu.h>		
//#include "OpenGLExtensions.h"


#include "VertexBuffer_GL.h"
#include "IndexBuffer_GL.h"
#include "VertexDecleration_GL.h"

#include "Defines.h"
#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include <windows.h>

#include "Cg/cg.h"
#include "Cg/cgGL.h"
extern CGcontext g_cgContext;
extern CGprofile g_VertexProfile,g_FragmentProfile;

extern void handleCgError();

#include "ShaderVertexProgram_GL.h"
#include "ShaderFragmentProgram_GL.h"


#include "GMath.h"
#include "MatrixGenerator.h"

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "Texture_GL.h"

#include "GraphicsAPI.h"
#include "GraphicsAPI_D3D.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "RenderToTexture.h"
#include "RenderToTexture_GL.h"

#include <assert.h>

#include "Profiler.h"


CGraphicsAPI_GL::CGraphicsAPI_GL()
{

	m_uiFrameNum = 0;

	// profiling

	m_ProfData.iTextureBinds = 0;

	//m_bMatricesDirty = false;

	m_bWorldIdentity = false;
		

	m_pActiveVB = NULL;
	m_pActiveIB = NULL;

	m_pActiveIB_DATA = NULL;
}

CGraphicsAPI_GL::~CGraphicsAPI_GL()
{

}

/*void* CGraphicsAPI_GL::GetDevice(void)
{
	return &m_hDC;
}*/

bool CGraphicsAPI_GL::InitExtensions(void)
{
	return (m_GLExtensions.Init());
}

bool CGraphicsAPI_GL::Init(int iResX, int iResY, HWND hWnd)
{

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// Define Our Window Style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// Define The Window's Extended Style

	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		32,			                        							// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		32,																// 32Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	RECT windowRect = {0, 0, iResX, iResY};	// Define Our Window Coordinates

	GLuint PixelFormat;													// Will Hold The Selected Pixel Format





	m_hDC = GetDC (hWnd);									// Grab A Device Context For This Window
	if (!m_hDC)												// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow (hWnd);									// Destroy The Window
		hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	PixelFormat = ChoosePixelFormat (m_hDC, &pfd);				// Find A Compatible Pixel Format
	if (!PixelFormat)												// Did We Find A Compatible Format?
	{
		// Failed
		ReleaseDC (hWnd, m_hDC);							// Release Our Device Context
		m_hDC = 0;												// Zero The Device Context
		DestroyWindow (hWnd);									// Destroy The Window
		hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (SetPixelFormat (m_hDC, PixelFormat, &pfd) == FALSE)	
	{
		// Failed
		ReleaseDC (hWnd, m_hDC);							
		m_hDC = 0;											
		DestroyWindow (hWnd);								
		hWnd = 0;											
		return FALSE;										
	}

	m_hRC = wglCreateContext (m_hDC);						
	if (!m_hRC)											
	{
		// Failed
		ReleaseDC (hWnd, m_hDC);							
		m_hDC = 0;											
		DestroyWindow (hWnd);								
		hWnd = 0;											
		return FALSE;										
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (m_hDC, m_hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (m_hRC);							
		m_hRC = 0;											
		ReleaseDC (hWnd, m_hDC);							
		m_hDC = 0;												
		DestroyWindow (hWnd);									
		hWnd = 0;												
		return FALSE;											
	}


	if (!InitExtensions())
	{
		CR_ERROR(1,"a critical extension isn't supported, quitting."); 
		return false;
	}
	
										
	


	////////
	// CG
	////////

	// set the error callback
	cgSetErrorCallback(handleCgError);

	// create cg context
	g_cgContext = cgCreateContext();	

	if (!g_cgContext)
	{
		g_pLogFile->OutPutPlainText("CGraphicsAPI_GL::Init CG Device Context Failed!",LOG_MESSAGE_ERROR);
		return false;
	}




	g_VertexProfile=cgGLGetLatestProfile(CG_GL_VERTEX);  
	
	if (!g_VertexProfile)
	{
		g_pLogFile->OutPutPlainText("CGraphicsAPI_GL::Init Getting Latest Vertex Profile Failed!",LOG_MESSAGE_ERROR);
		return false;
	}

	g_FragmentProfile=cgGLGetLatestProfile(CG_GL_FRAGMENT);

	if (!g_FragmentProfile)
	{
		g_pLogFile->OutPutPlainText("CGraphicsAPI_GL::Init Getting Latest Vertex Profile Failed!",LOG_MESSAGE_ERROR);
		return false;
	}

	cgGLSetOptimalOptions(g_VertexProfile);  
	cgGLSetOptimalOptions(g_FragmentProfile);

	cgGLEnableProfile(g_VertexProfile);
	cgGLEnableProfile(g_FragmentProfile);






	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_iSamplersNum);

	m_pSamplers = new tSamplerState[m_iSamplersNum];



	

	for (int i=0;i<m_iSamplersNum;i++)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);  
		glEnable(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(!glGetError());
		
		m_pSamplers[i].cFiltering = SS_FILTERING_TRILINEAR;


		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);  
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		m_pSamplers[i].cClamping = SS_CLAMPING_CLAMP;
	}

	
	
	SetClearColor(0.f,0.f,0.f,0.f);
	glClearDepth (1.0f);				

	
	Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);

	glDepthFunc(GL_LEQUAL);	
	assert(!glGetError());
	m_RendererState.m_dwDepthFunc = CR_LESS_OR_EQUAL;
	
	glEnable(GL_DEPTH_TEST);	
	assert(!glGetError());
	m_RendererState.m_bDepthTest=true;

	glDepthMask(GL_TRUE);
	m_RendererState.m_bDepthWrite=true;
	


	glBlendFunc(GL_ONE,GL_ZERO);
	m_RendererState.m_cBlendSource = CR_BF_ONE;
	m_RendererState.m_cBlendTarget = CR_BF_ZERO;
	glDisable(GL_BLEND);

	//SetBlendFunc(CR_BF_ONE,CR_BF_ONE);




	/*glDisable(GL_CULL_FACE);
	m_RendererState.m_bCull=false;*/

	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	assert(!glGetError());
	m_RendererState.m_bCull=true;
	

	/*for some reason, in opengl the supported back face culling only works
		at the reverse way :/*/

	glFrontFace(GL_CW);
	//glFrontFace(GL_CCW);
	assert(!glGetError());

	glCullFace(GL_BACK);
	//glCullFace(GL_FRONT);
	assert(!glGetError());
	m_RendererState.m_dwCullFunc = CR_BACK;


	glShadeModel (GL_SMOOTH);									
	assert(!glGetError());
	//glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_DONT_CARE);		
	
	assert(!glGetError());

	glViewport (0, 0, (GLsizei)(iResX), (GLsizei)(iResY));				
	assert(!glGetError());
	

	m_pfProjection = new float[16];
	m_pfWorld = new float[16];
	m_pfView = new float[16];
	m_pfWorldView = new float[16];

	
	SetProjectionMatrix(45.0f, (GLfloat)(iResX)/(GLfloat)(iResY),CR_NEAR_Z, CR_FAR_Z);
	m_iResX = iResX;
	m_iResY = iResY;
		

	glMatrixMode (GL_MODELVIEW);										
	glLoadIdentity ();	

	return true;
}

void CGraphicsAPI_GL::OnResize(int iResX, int iResY)
{
	// REMEMBER TO ADD CHANGES TO POST PROCESSING RENDER TARGETS!

	glViewport (0, 0, (GLsizei)(iResX), (GLsizei)(iResY));				
	/*glMatrixMode (GL_PROJECTION);										
	glLoadIdentity ();													
	gluPerspective (45.0f, (GLfloat)(iResX)/(GLfloat)(iResY),			
					1.0f, 100.0f);		
	glMatrixMode (GL_MODELVIEW);										
	glLoadIdentity ();				*/

	static bool bFirstResize=true;

	if (!bFirstResize)
	{
		SetProjectionMatrix(45.0f, (float)(iResX)/(float)(iResY),CR_NEAR_Z, CR_FAR_Z);		
		m_iResX = iResX;
		m_iResY = iResY;
	}
		
	bFirstResize=false;
}
void CGraphicsAPI_GL::UnBindTexUnit(int iSamplerNum)
{	
		glActiveTextureARB(GL_TEXTURE0_ARB + iSamplerNum);  
		glDisable(GL_TEXTURE_2D);
		m_pSamplers[iSamplerNum].iTextureType = CR_TEXTURE_OFF;
}

void CGraphicsAPI_GL::BeginFrame(void)
{

	//glViewport (0, 0, (GLsizei)(m_iResX), (GLsizei)(m_iResY));				
	SetViewport(0,0,m_iResX,m_iResY);

	SetClearColor(0.f,0.f,0.f,1.f);
	Clear(CR_CLEAR_COLOR | CR_CLEAR_DEPTH);

	
	g_pGraphicsAPI->SetProjectionMatrix(45.0f, (float)(m_iResX)/(float)(m_iResY),CR_NEAR_Z, CR_FAR_Z);

	for (int i=0;i<API_TEX_UNITS_NUM;i++)
	{
		UnBindTexUnit(i);
	}

	// profiling
	m_ProfData.iTextureBinds = 0;


	// debug debug
	//glScissor(0	,0,200,200);	// Define Scissor Region
	//glEnable(GL_SCISSOR_TEST);	

	
}

void CGraphicsAPI_GL::EnableScissorRect(void) 
{
	if (!m_RendererState.bScissorRect)
	{
		m_RendererState.bScissorRect = true;
		glEnable(GL_SCISSOR_TEST);	
	}
}

void CGraphicsAPI_GL::DisableScissorRect(void) 
{
	if (m_RendererState.bScissorRect)
	{
		m_RendererState.bScissorRect = false;
		glDisable(GL_SCISSOR_TEST);	
	}
}

void CGraphicsAPI_GL::SetScissorRect(int x1, int y1, int x2, int y2) const
{
	int iLeft   = min(x1,x2);
	int iRight  = max(x1,x2);
	int iTop    = max(y1,y2);
	int iBottom = min(y1,y2);


	glScissor(iLeft,iBottom,iRight,iTop);
}




void CGraphicsAPI_GL::Present(void)
{
	//glFlush();								// Flush The GL Rendering Pipeline
	SwapBuffers (m_hDC);					// Swap Buffers (Double Buffering)

	m_uiFrameNum++;
}

IVertexBuffer_Export* CGraphicsAPI_GL::GenerateVertexBuffer(void) const
{
	IVertexBuffer* pVB = new CVertexBuffer_GL();
	return pVB;	
}

IIndexBuffer*  CGraphicsAPI_GL::GenerateIndexBuffer(void) const
{
	IIndexBuffer* pIB = new CIndexBuffer_GL();
	return pIB;	
}

CVertDecl*  CGraphicsAPI_GL::GenerateVertexDecleration(void) const
{
	CVertDecl* pVD = new CVertDecl_GL();
	return pVD;
}

IShaderProgram_Export* CGraphicsAPI_GL::GenerateShaderVertexProgram(void)   const
{
	IShaderProgram* pSVP = new CShaderVertexProgram_GL();
	return pSVP;
}

IShaderProgram_Export* CGraphicsAPI_GL::GenerateShaderFragmentProgram(void) const
{
	IShaderProgram* pSFP = new CShaderFragmentProgram_GL();
	return pSFP;
}

ITexture*   CGraphicsAPI_GL::GenerateTexture(void) const
{
	// no managers for now, always assumes new texture
	ITexture* ans = new CTexture_GL();
	return ans;
}

// generate RenderToTexture
IRenderToTexture*  CGraphicsAPI_GL::GenerateRenderToTexture(void) const
{
	IRenderToTexture* ans = new CRenderToTexture_GL();
	return ans;
}

void CGraphicsAPI_GL::UpdateAutoUniforms(void) 
{
	static CProfileStep step("UpdateAU:");	
	step.Begin();

/*	if (!m_bMatricesDirty)
		return;

	m_bMatricesDirty = false;*/

	// threads...
	float pfMatrix[16];
	
	CMatrix proj,world,view;

	
	world.Set(g_pGraphicsAPI->GetWorldMatrix());
	view.Set(g_pGraphicsAPI->GetViewMatrix());			
	proj.Set(g_pGraphicsAPI->GetProjectionMatrix());


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


// expects column based layout
void CGraphicsAPI_GL::SetWorldMatrix(const float* pMat) 
{
//	m_bMatricesDirty = true;
	CMatrix ident;

	if (pMat)
	{			
		/*CMatrix incomingWorld;

		// HMMMM: commenting this and doing the memcpy
		//        made it work for gl

		//incomingWorld.Set(pMat);
		//incomingWorld.ReturnArray(m_pfWorld);
		memcpy(m_pfWorld,pMat,sizeof(float)*16);*/

		m_bWorldIdentity = false;
		memcpy(m_pfWorld,pMat,sizeof(float)*16);
	}
	else
	{
		if (!m_bWorldIdentity) // only if it wasn't already identity
		{					
			ident.MatMakeUnit();
			ident.ReturnArray(m_pfWorld);
		}
	}

	if (pMat==NULL && m_bWorldIdentity)
		return;


	if (!pMat)
		m_bWorldIdentity = true;

	UpdateAutoUniforms();
}

void CGraphicsAPI_GL::SetViewMatrix(const float* pMat)
{
//	m_bMatricesDirty = true;

	if (pMat)
		memcpy(m_pfView,pMat,sizeof(float)*16);
	else
	{
		CMatrix ident;
		ident.MatMakeUnit();
		ident.ReturnArray(m_pfView);
	}		

	UpdateAutoUniforms();


}

void CGraphicsAPI_GL::SetProjectionMatrix(const float* pMat) 
{
//	m_bMatricesDirty = true;
	/*glMatrixMode (GL_PROJECTION);
	glLoadMatrixf(pMat);*/

	if (pMat)
		memcpy(m_pfProjection,pMat,sizeof(float)*16);
	else
	{
		// i need to store a single instance of identity or sth ...

		CMatrix ident;
		ident.MatMakeUnit();
		ident.ReturnArray(m_pfProjection);
	}

	UpdateAutoUniforms();
}

void CGraphicsAPI_GL::SetProjectionMatrix(float fFovy,float fAspect,float fZNear,float fZFar)
{	
//	m_bMatricesDirty = true;

	CMatrixGenerator::PespectiveRH(m_pfProjection,fFovy,fAspect,fZNear,fZFar);
	
	/*glMatrixMode(GL_PROJECTION);										
	glLoadMatrixf(m_pfProjection);*/

	UpdateAutoUniforms();
}

const float* CGraphicsAPI_GL::GetWorldMatrix(void)
{
	//UpdateAutoUniforms();
	return m_pfWorld;
}

const float* CGraphicsAPI_GL::GetViewMatrix(void)
{
	//UpdateAutoUniforms();
	return m_pfView;
}

const float* CGraphicsAPI_GL::GetProjectionMatrix(void)
{
	//UpdateAutoUniforms();
	/*float tempProjection[16];
	
	glGetFloatv(GL_PROJECTION_MATRIX,&tempProjection[0]);

	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			m_pfProjection[i+j*4] = tempProjection[i*4+j];*/

	return m_pfProjection;
}

const float* CGraphicsAPI_GL::GetModelViewMatrix(void)
{
	//UpdateAutoUniforms();
	// threads...
	static float* pfMatrix = new float[16];

	CMatrix proj,world,view;
	world.Set(g_pGraphicsAPI->GetWorldMatrix());
	view.Set(g_pGraphicsAPI->GetViewMatrix());	

	(view*world).ReturnArray(m_pfWorldView);
	
	return m_pfWorldView;
}

void  CGraphicsAPI_GL::SetDepthTest(bool bDepthTest)
{
	if (m_RendererState.m_bDepthTest!=bDepthTest)
	{
		m_RendererState.m_bDepthTest=bDepthTest;

		if (m_RendererState.m_bDepthTest)
			glEnable (GL_DEPTH_TEST);	
		else
			glDisable (GL_DEPTH_TEST);	
	}
}

void  CGraphicsAPI_GL::SetDepthWrite(bool bDepthWrite)
{
	if (m_RendererState.m_bDepthWrite!=bDepthWrite)
	{
		m_RendererState.m_bDepthWrite=bDepthWrite;

		if (m_RendererState.m_bDepthWrite)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}
}

void CGraphicsAPI_GL::SetAlphaFunc(DWORD dwFunc)
{
	BREAKPOINT(1); // not implemented yet
}

void CGraphicsAPI_GL::SetAlphaReferenceValue(DWORD dwVal)
{
	BREAKPOINT(1); // not implemented yet
}

void CGraphicsAPI_GL::SetBlendFunc(char cSource,char cTarget)
{

	if ( (m_RendererState.m_cBlendSource!=cSource) ||
		 (m_RendererState.m_cBlendTarget!=cTarget))
	{
		m_RendererState.m_cBlendSource=cSource;
	    m_RendererState.m_cBlendTarget=cTarget;


		// add opts to prevent redundant stuff here
		if ( (cSource==CR_BF_ONE) &&(cTarget==CR_BF_ZERO))
			glDisable(GL_BLEND);
		else
			glEnable(GL_BLEND);


		DWORD dwTranslatedSource;

		switch (cSource)
		{
		case CR_BF_ZERO:	            dwTranslatedSource=GL_ZERO;	       break;
		case CR_BF_ONE:	            	dwTranslatedSource=GL_ONE;           break;
		case CR_BF_DST_COLOR:	     	dwTranslatedSource= GL_DST_COLOR;    break;
		case CR_BF_ONE_MINUS_DST_COLOR:	dwTranslatedSource= GL_ONE_MINUS_DST_COLOR; break;
		case CR_BF_SRC_ALPHA:		    dwTranslatedSource= GL_SRC_ALPHA;     break;
		case CR_BF_ONE_MINUS_SRC_COLOR:	dwTranslatedSource= GL_ONE_MINUS_SRC_COLOR;  break;
		case CR_BF_DST_ALPHA:			dwTranslatedSource= GL_DST_ALPHA;    break;
		case CR_BF_ONE_MINUS_DST_ALPHA:	dwTranslatedSource= GL_ONE_MINUS_DST_ALPHA; break;
		case CR_BF_SRC_ALPHA_SATURATE:	dwTranslatedSource= GL_SRC_ALPHA_SATURATE;  break;
		case CR_BF_ONE_MINUS_SRC_ALPHA:	dwTranslatedSource= GL_ONE_MINUS_SRC_ALPHA;  break;
		case CR_BF_SRC_COLOR:			dwTranslatedSource= GL_SRC_COLOR;     break;
		default:
			CR_WARNING0(1,"CGraphicsAPI_D3D::SetBlendFunc - unrecognized blend source value! defaulting to CR_ONE");
			dwTranslatedSource=D3DBLEND_ONE;
		// Invalid Blendfunc
		};

		DWORD dwTranslatedTarget;

		switch (cTarget)
		{
		case CR_BF_ZERO:	            dwTranslatedTarget=GL_ZERO;	       break;
		case CR_BF_ONE:	            	dwTranslatedTarget=GL_ONE;           break;
		case CR_BF_DST_COLOR:	     	dwTranslatedTarget= GL_DST_COLOR;    break;
		case CR_BF_ONE_MINUS_DST_COLOR:	dwTranslatedTarget= GL_ONE_MINUS_DST_COLOR; break;
		case CR_BF_SRC_ALPHA:		    dwTranslatedTarget= GL_SRC_ALPHA;     break;
		case CR_BF_ONE_MINUS_SRC_COLOR:	dwTranslatedTarget= GL_ONE_MINUS_SRC_COLOR;  break;
		case CR_BF_DST_ALPHA:			dwTranslatedTarget= GL_DST_ALPHA;    break;
		case CR_BF_ONE_MINUS_DST_ALPHA:	dwTranslatedTarget= GL_ONE_MINUS_DST_ALPHA; break;
		case CR_BF_SRC_ALPHA_SATURATE:	dwTranslatedTarget= GL_SRC_ALPHA_SATURATE;  break;
		case CR_BF_ONE_MINUS_SRC_ALPHA:	dwTranslatedTarget= GL_ONE_MINUS_SRC_ALPHA;  break;
		case CR_BF_SRC_COLOR:			dwTranslatedTarget= GL_SRC_COLOR;     break;
		default:
			CR_WARNING0(1,"CGraphicsAPI_D3D::SetBlendFunc - unrecognized blend target value! defaulting to CR_ONE");
			dwTranslatedTarget=D3DBLEND_ONE;
		// Invalid Blendfunc
		};



		glBlendFunc(dwTranslatedSource,dwTranslatedTarget);



	}

}

void CGraphicsAPI_GL::SetDepthFunc(DWORD dwDepthFunc)
{
	if (m_RendererState.m_dwDepthFunc!=dwDepthFunc)
	{
		m_RendererState.m_dwDepthFunc = dwDepthFunc;

		switch(m_RendererState.m_dwDepthFunc)
		{

		case CR_ALWAYS:
			glDepthFunc (GL_ALWAYS);
		break;
		case CR_LESS:
			glDepthFunc (GL_LESS);			
		break;

		case CR_LESS_OR_EQUAL:
			glDepthFunc (GL_LEQUAL);	
		break;

		case CR_EQUAL:
			glDepthFunc (GL_EQUAL);	

		break;
		}

	}

}


void CGraphicsAPI_GL::SetCullFunc(DWORD dwCullFunc)
{
	if (m_RendererState.m_dwCullFunc!=dwCullFunc)
	{
		m_RendererState.m_dwCullFunc=dwCullFunc;

		switch(m_RendererState.m_dwCullFunc)
		{			
		case CR_BACK:
			
			glCullFace(GL_BACK);
			if (!m_RendererState.m_bCull)
			{
				m_RendererState.m_bCull=true;
				glEnable(GL_CULL_FACE);
			}
		break;
		case CR_FRONT:
			glCullFace(GL_FRONT);
			if (!m_RendererState.m_bCull)
			{
				m_RendererState.m_bCull=true;
				glEnable(GL_CULL_FACE);
			}
		break;

		case CR_NONE:
			glCullFace(GL_BACK);
			if (m_RendererState.m_bCull)
			{
				m_RendererState.m_bCull=false;
				glDisable(GL_CULL_FACE);
			}
		break;		
		}

	}

}


void CGraphicsAPI_GL::BindVertexShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms)
{
	// remember - there's a problem when rendering only one vertex shader in whole scene!
	//if (m_ActiveVertexProgram != spShaderProgram)
	{
		m_ActiveVertexProgram = spShaderProgram;
		spShaderProgram->Bind(pUniforms);
	}
	/*else
		spShaderProgram->RebindParameters(pUniforms);*/
}

void CGraphicsAPI_GL::UnBindVertexShader(void)
{
	cgGLDisableProfile( g_VertexProfile );

}

void CGraphicsAPI_GL::BindFragmentShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms)
{
	// remember - there's a problem when rendering only one fragment shader in whole scene!
	//if (m_ActiveFragmentProgram != spShaderProgram)
	{
		m_ActiveFragmentProgram = spShaderProgram;
		spShaderProgram->Bind(pUniforms);
	}
	/*else
		spShaderProgram->RebindParameters(pUniforms);*/
}

void CGraphicsAPI_GL::UnBindFragmentShader(void)
{
	cgGLDisableProfile( g_FragmentProfile );

}

void CGraphicsAPI_GL::BindTexture(int iSamplerNum,CSmartPointer<ITexture>& tex)
{
	glActiveTextureARB(GL_TEXTURE0_ARB + iSamplerNum);  
	
	if (m_pSamplers[iSamplerNum].iTextureType != CR_TEXTURE_2D)
	{
		glEnable(GL_TEXTURE_2D);
		m_pSamplers[iSamplerNum].iTextureType = CR_TEXTURE_2D;
	}

	// restore - remember it creates problems for proxy because of the pointers comparison!
	if (tex->GetForceReBind() || m_pSamplers[iSamplerNum].spTex!=tex)
	{
		m_pSamplers[iSamplerNum].spTex = tex;
		tex->Bind(iSamplerNum);

		m_ProfData.iTextureBinds++;
	}

}

// samplers state
void CGraphicsAPI_GL::SetClamping(int iSamplerNum,char cClamping)
{

	if (m_pSamplers[iSamplerNum].cClamping!=cClamping)
	{
		m_pSamplers[iSamplerNum].cClamping = cClamping;

		glClientActiveTextureARB(GL_TEXTURE0_ARB + iSamplerNum);

		switch (m_pSamplers[iSamplerNum].cClamping)
		{
		case SS_CLAMPING_REPEAT:
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
/*		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);		*/
		break;
		case SS_CLAMPING_CLAMP:
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);		
		break;
		}
	}
}

void CGraphicsAPI_GL::SetFiltering(int iSamplerNum,char cFiltering,char cAnisotropicLevel)
{
//if (m_pSamplers[iSamplerNum].cFiltering!=cFiltering)
	{
		m_pSamplers[iSamplerNum].cFiltering = cFiltering;


		int iError;

		switch (m_pSamplers[iSamplerNum].cFiltering)
		{
		
			
		case SS_FILTERING_NEAREST:


		// RESTORE RESTORE
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		iError = glGetError();
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		assert(!glGetError());

		m_pSamplers[iSamplerNum].cAnisotropicLevel = 1;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pSamplers[iSamplerNum].cAnisotropicLevel);
		assert(!glGetError());
		
		break;






		/*case SS_FILTERING_NEAREST_NO_MIPMAPS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		assert(!glGetError());

		m_pSamplers[iSamplerNum].cAnisotropicLevel = 1;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pSamplers[iSamplerNum].cAnisotropicLevel);
		assert(!glGetError());
		break;
*/

		

		case SS_FILTERING_BILINEAR:
		// RESTORE RESTORE
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(!glGetError());

		m_pSamplers[iSamplerNum].cAnisotropicLevel = 1;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pSamplers[iSamplerNum].cAnisotropicLevel);
		assert(!glGetError());
		
		
		break;
		case SS_FILTERING_TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(!glGetError());

		m_pSamplers[iSamplerNum].cAnisotropicLevel = 1;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pSamplers[iSamplerNum].cAnisotropicLevel);
		assert(!glGetError());
		
		break;


		case SS_FILTERING_ANISOTROPIC:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		assert(!glGetError());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(!glGetError());

		m_pSamplers[iSamplerNum].cAnisotropicLevel = cAnisotropicLevel;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pSamplers[iSamplerNum].cAnisotropicLevel);
		assert(!glGetError());

		break;

		default:
			BREAKPOINT(1);
		}
	}
}

void CGraphicsAPI_GL::DebugRenderBBox(const BBox& bbox)
{	
	CVec3f vMin = bbox.min;
	CVec3f vMax = bbox.max;

	//vMin

	CVec3f v1(vMin.v[0],vMin.v[1],vMax.v[2]);
	CVec3f v2(vMax.v[0],vMin.v[1],vMax.v[2]);
	CVec3f v3(vMax.v[0],vMin.v[1],vMin.v[2]);
	CVec3f v4(vMin.v[0],vMin.v[1],vMin.v[2]);
	CVec3f v5(vMin.v[0],vMax.v[1],vMax.v[2]);
	CVec3f v6(vMax.v[0],vMax.v[1],vMax.v[2]);
	CVec3f v7(vMax.v[0],vMax.v[1],vMin.v[2]);
	CVec3f v8(vMin.v[0],vMax.v[1],vMin.v[2]);

	CVec3f col(0.f,0.f,1.f);

	DrawDebugLine(v1,v2,col);
	DrawDebugLine(v1,v4,col);
	DrawDebugLine(v1,v5,col);
	DrawDebugLine(v2,v3,col);
	DrawDebugLine(v3,v4,col);
	DrawDebugLine(v2,v6,col);
	DrawDebugLine(v5,v8,col);
	DrawDebugLine(v7,v8,col);
	DrawDebugLine(v6,v5,col);
	DrawDebugLine(v6,v7,col);
	DrawDebugLine(v4,v8,col);
	DrawDebugLine(v3,v7,col);

}

// debug
void CGraphicsAPI_GL::DrawDebugLine(const CVec3f& v1, const CVec3f& v2, const CVec3f& col) 
{	
	for (int i=0;i<m_iSamplersNum;i++)
		UnBindTexUnit(i);

	glColor4f(col.x,col.y,col.z,0.f);

	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(GetViewMatrix());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(GetProjectionMatrix());

	glBegin(GL_LINES);
	glVertex3f(v1.x,v1.y,v1.z);
	glVertex3f(v2.x,v2.y,v2.z);
	glEnd( );

	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}


void CGraphicsAPI_GL::DrawDebugLine2D(const CVec2f& v1, const CVec2f& v2) 
{
	for (int i=0;i<m_iSamplersNum;i++)
		UnBindTexUnit(i);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(1.f,0.f,0.f,0.f);

	glBegin(GL_LINES);
	glVertex2f(v1.x,v1.y);
	glVertex2f(v2.x,v2.y);
	glEnd();

	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

	

void CGraphicsAPI_GL::BindVB(IVertexBuffer_Export* pVB, const CVertDecl* pVD)
{
	//TODO: support short as well!
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	for (int i=0;i<API_TEX_COORD_UNITS_NUM;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	}


	//if (m_pActiveVB!=pVB)
	{
		m_pActiveVB = pVB;		

		pVD->Bind();
		((IVertexBuffer*)pVB)->BindStreams(pVD);

	}

}

void CGraphicsAPI_GL::BindIB(const IIndexBuffer* pIB)
{
	//TODO: i need to add some test to see if this is a dynamic IndexBuffer, 
	// and do bind if that's the case!
	// just add some flag to the IB if it's dynamic or not
	//if (m_pActiveIB!=pIB)
	{
		m_pActiveIB = pIB;

		m_pActiveIB_DATA = pIB->Bind();		
	}

	//return m_pActiveIB_DATA;

}

void CGraphicsAPI_GL::DrawPrimitives(int iIndicesStart,int iIndicesCount,int iBaseVertexIndex, int iMinIndex,int iVerticesNum) const
{


	glDrawElements(GL_TRIANGLES,
				iIndicesCount,
				GL_UNSIGNED_INT,
				((unsigned int*)m_pActiveIB_DATA)+iIndicesStart);

	/*glDrawElements(GL_TRIANGLES,
				rend->GetIndicesCount(),
				GL_UNSIGNED_INT,
				m_pIndices+rend->GetIndicesStart());
				*/
}

void CGraphicsAPI_GL::SetClearColor(float fR,float fG, float fB, float fA) 
{
	glClearColor(fR,fG,fB,fA);
}

void CGraphicsAPI_GL::Clear(DWORD dwFlags) const
{
	DWORD dwGLFlags=0;
	if (dwFlags&CR_CLEAR_COLOR)
		dwGLFlags|=GL_COLOR_BUFFER_BIT;
	if (dwFlags&CR_CLEAR_DEPTH)
		dwGLFlags|=GL_DEPTH_BUFFER_BIT;
	if (dwFlags&CR_CLEAR_STENCIL)
		dwGLFlags|=GL_STENCIL_BUFFER_BIT;

	glClear(dwGLFlags);	

}

void CGraphicsAPI_GL::SetViewport(int x, int y, int iWidth, int iHeight) const
{
	glViewport(x,y,iWidth,iHeight);
}