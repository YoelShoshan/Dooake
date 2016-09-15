#include "stdafx.h"
#pragma hdrstop


#include "DebugText_D3D.h"
/*namespace cdDX
{*/

//----------------------------------------------------------------------------------------------------------
CDXDebugText::CDXDebugText()
{
	m_pFont=NULL;
	m_pDevice=NULL;
}
//----------------------------------------------------------------------------------------------------------
CDXDebugText::~CDXDebugText()
{
}
//----------------------------------------------------------------------------------------------------------
void CDXDebugText::Init(LPDIRECT3DDEVICE9 pDev)
{
	HRESULT hr = D3DXCreateFont( pDev,            // D3D device
					             //12,               // Height
								 20,               // Height
								 0,                     // Width
								 FW_BOLD,               // Weight
								 1,                     // MipLevels, 0 = autogen mipmaps
								 FALSE,                 // Italic
								 DEFAULT_CHARSET,       // CharSet
								 OUT_DEFAULT_PRECIS,    // OutputPrecision
								 DEFAULT_QUALITY,       // Quality
								 DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
								 "Arial",              // pFaceName
								 &m_pFont);

	m_pDevice = pDev;
	assert(SUCCEEDED(hr));
}
//----------------------------------------------------------------------------------------------------------
void CDXDebugText::printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...)
{		
	//debug
	//return;

	
	va_list argumentPtr;


	if (!pFormat) return;

	/*if (strlen(pFormat)>=DEBUG_TEXT_MAX_LENGTH-1)
		return;*/

	va_start(argumentPtr, pFormat);
	vsprintf(m_chText, pFormat, argumentPtr);
	
	va_end(argumentPtr);

	RECT rc;
	rc.left=x;
	rc.top=y;
	rc.right=10000;
	rc.bottom=10000;


	//m_pDevice->BeginScene();

	// restore
	m_pFont->DrawTextA( NULL, m_chText, -1, &rc, DT_LEFT | DT_WORDBREAK | DT_EXPANDTABS, D3DXCOLOR( vCol.x, vCol.y, vCol.z, 1.0f ));

	//m_pDevice->EndScene();
}
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*};*/
