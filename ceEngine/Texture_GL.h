#ifndef CRIMSON_TEXTURE_GL_H
#define CRIMSON_TEXTURE_GL_H

#pragma once

//#include "ReferenceCount.h"
//#include "SmartPointer.h"
//#include "iostream.h"
#include "Windows.h"

//#include "jpeglib.h"

#include "Texture.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

// Largest supported anistropic texture ratio extension address
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

	// Anistropic texturing extension address
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif


#define TGA_RGB		 2	
#define TGA_A		 3	
#define TGA_RLE		10	




struct tImage
{
	int channels;			// how many channels? (3 is just rgb, 4 has also alpha - rgba)
	int sizeX;			
	int sizeY;			
	unsigned char *data;	// the actual pixels data
};

class CTexture_GL: public ITexture
{ 
	friend class CGraphicsAPI_GL;
public:      
     ~CTexture_GL(); 
 
     //bool LoadRaw(byte* pImageBits,int iWidth, int iHeight); 
     //void CreateNormalizationCubeMap(int iSize);  --> ask dTag about this

/*	 void ChangeGamma(char *pImage, int size, float factor);
     
	 void LoadLightmapTexture(char *pImageBits, int width, int height);*/
	 

	 bool Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress);

	 //bool LoadSkyBoxTextures(const char* strFileName);

     void Bind(int iSamplerNum) const; 

     inline const char* GetName() const { return m_TexName; }; 

	 int GetWidth() const
	 {
		 return m_iWidth;
	 }
	 int GetHeight() const
	 {
		 return m_iHeight;
	 }

	 bool GetForceReBind() const
	 {
		 return false;
	 }

/*     inline int GetWidth() const { return m_iWidth; }; 
     inline int GetHeight() const  { return m_iHeight; }; 
 
	 inline void SetID(UINT uiID) { m_uiTextureID = uiID;};
     inline UINT GetID() const { return m_uiTextureID; }; 

	 inline bool GetIsLoaded() const { return m_bIsLoaded;};
	 inline void SetIsLoaded(bool bIsLoaded) { m_bIsLoaded = bIsLoaded;}; */
 
protected: 
	CTexture_GL(); 
     //void GetCubeVector(int i, int iCubeSize, int x, int y, CVector3& v); 

	bool FindTextureExtension(char *strFileName);
 
     tImage* LoadBMP(const char *pchFileName); 
     tImage* LoadTGA(const char *pchFileName); 
     tImage* LoadJPG(const char *pchFileName); 
     
	 //void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData); 

	 bool m_bIsLoaded;
 
     UINT m_uiTextureID; 
     //LPSTR sName; 
	 char* m_TexName;

 
     int m_iWidth; 
     int m_iHeight; 
}; 

///////////////////////////////////////////
//
// Texture Manager
//
///////////////////////////////////////////

/*#define st_TextureManager CSingleton<CTextureManager>


#define TEX_MAN_MAX_TEXTURES 2048

class CTextureManager
{
public:

	CTextureManager();
	~CTextureManager();
	
	CTexture* LoadTexAndPush(const char *FileName,bool bMipMap,bool bClamp,bool bCompress);

	int AlreadyInManager(const char* TextureName);

	// finds the first free tex, allocates it, and returns a pointer to the new allocated CTexture
	CTexture* AllocFirstFreeTex(void);

	bool Unregister(CTexture* pTex);

	int GetTexturesNum(void);


private:

	UINT        m_uiLastTexNum;
	CSmartPointer<CTexture>* m_spTexturesQueue;
};
*/

#endif