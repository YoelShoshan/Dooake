#include "stdafx.h"
#include "Texture_D3D.h"
#include "Defines.h"
#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "ce.h"

#include <string>

#include "stdio.h"



///////////////////////////////////////////////////////////////////
// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
#include "TextureManager.h"
///////////////////////////////////////////////////////////////////

extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;


CTexture_D3D::CTexture_D3D()
{
	m_pD3DTexture = NULL;
}

CTexture_D3D::~CTexture_D3D()
{
	char message[200];
	sprintf_s(message,200,"deleting texture: ""%s"".",m_pcTexName);	
	

	//////////////////////////////////////////////////////////////////////////////////////////
	// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
	st_TextureManager->Unregister(m_pcTexName); // unregister this texture from the tex manager
	//////////////////////////////////////////////////////////////////////////////////////////

	SafeDeleteArray(m_pcTexName);
	SAFE_RELEASE(m_pD3DTexture);

	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

bool CTexture_D3D::FindTextureExtension(char *strFileName)
{
	// i need to avoid all the string copies 
	// which i'm doing because of the find-extension process (for q3 maps)...

	FILE *fp = NULL;

	// if already has extension, change nothing.
	if((fp = fopen(strFileName, "rb")) != NULL)
		return true;

	std::string strJPGPath;
	std::string strTGAPath;
	std::string strBMPPath;

	

	strJPGPath+=strFileName;
	strJPGPath+=".jpg";

	strTGAPath+=strFileName;
	strTGAPath+=".tga";

	strBMPPath+=strFileName;
	strBMPPath+=".bmp";

	if((fp = fopen(strJPGPath.c_str(), "rb")) != NULL)
	{
		strcat(strFileName, ".jpg");
		fclose(fp);
		return true;
	}

	if((fp = fopen(strTGAPath.c_str(), "rb")) != NULL)
	{
		strcat(strFileName, ".tga");
		fclose(fp);
		return true;
	}

	if((fp = fopen(strBMPPath.c_str(), "rb")) != NULL)
	{
		strcat(strFileName, ".bmp");
		fclose(fp);
		return true;
	}

	
	return false;
}

bool CTexture_D3D::Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress)
{
	if(!strFileName) 
	{
		BREAKPOINT(1);
		return false;
	}

	static char ExtendedName[500];
	strcpy(ExtendedName,strFileName);

	if (!FindTextureExtension(ExtendedName))
		return false;

	if (D3DXCreateTextureFromFile(g_pDirect3DDevice,ExtendedName,&m_pD3DTexture)!=D3D_OK)
		return false;

	

	// I MUST do something like:
	// m_iWidth  = texture width;
	// m_iHeight = texture height;
	//BREAKPOINT(1);
	

	m_pcTexName = new char[strlen(ExtendedName)+1];
	strcpy(m_pcTexName,ExtendedName);

	return true;
}

void CTexture_D3D::Bind(int iSamplerNum) const
{
	CR_ERROR(!m_pD3DTexture,"Trying to bind a null d3d texture!");
	HRESULT hr = g_pDirect3DDevice->SetTexture(iSamplerNum,m_pD3DTexture);
	assert(SUCCEEDED(hr));
}