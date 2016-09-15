#ifndef DXDEBUGTEXT_H
#define DXDEBUGTEXT_H

#include "GraphicsAPI.h"

#include<d3d9.h>
#include<d3dx9.h>

#include "Math3D.h"

#include "SmartComPointer.h"

/*namespace cdDX
{*/

#define DEBUG_TEXT_MAX_LENGTH 1024

class CDXDebugText
{
public:
	CDXDebugText();
	~CDXDebugText();
	void Init(LPDIRECT3DDEVICE9 pDev);
	void printf(const CVec3f& vCol,int x,int y, const char* pFormat, ...);
protected:
	CSmartCOMPtr<ID3DXFont> m_pFont;

	LPDIRECT3DDEVICE9 m_pDevice;

	char m_chText[DEBUG_TEXT_MAX_LENGTH];
};

/*};*/

#endif


