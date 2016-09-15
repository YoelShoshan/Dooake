#include "stdafx.h"
#include "Texture_GL.h"

#include "Singleton.h"
#include <gl/GL.h>
//#include <gl/GLAux.h>

#include "glext.h"
#include <string>
#include <assert.h>

#include "ce.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;


///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////////////////////////////////////////
// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
#include "TextureManager.h"
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// TODO: add warning if texture isn't a power of two
//////////////////////////////////////////////////////

CTexture_GL::CTexture_GL()
{
	m_uiTextureID = 0; 
	m_TexName=NULL;
    m_iWidth =  m_iHeight = 0; 
}


CTexture_GL::~CTexture_GL()
{
	char message[200];
	sprintf(message,"deleting texture: ""%s"".",m_TexName);

	///////////////////////////////////////////////////////////////////////////////////////////
	// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
	st_TextureManager->Unregister(m_TexName); // unregister this texture from the tex manager 
	///////////////////////////////////////////////////////////////////////////////////////////
	
	SafeDeleteArray(m_TexName);
	
	glDeleteTextures(1,&m_uiTextureID); // actually free the memory from the card
	
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}


void CTexture_GL::Bind(int iSamplerNum) const
{	
	glBindTexture(GL_TEXTURE_2D,m_uiTextureID);
}

/*
	glClientActiveTextureARB(GL_TEXTURE0_ARB+iTexUnit);
		glActiveTextureARB(GL_TEXTURE0_ARB+iTexUnit);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(m_iTextureType, m_iTextureID);

	static CVar<int> cvTextureBinds("cvTextureBinds",0,"Number of texture binds this frame");
	cvTextureBinds++;
	*/

bool CTexture_GL::FindTextureExtension(char *strFileName)
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

///////////////////////////////// LOAD BMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the BMP file and returns it's data in a tImage struct
/////	
///////////////////////////////// LOAD BMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

tImage *CTexture_GL::LoadBMP(const char *strFileName)
{
	/*AUX_RGBImageRec *pBitmap = NULL;
	FILE *pFile = NULL;
	char caError[200];


	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		sprintf(caError,"Unable to load BMP File:%s",strFileName);
		return NULL;
	}

	// Loading the bitmap using the aux function stored in glaux.lib
	pBitmap = auxDIBImageLoad(strFileName);				*/


	char caError[200];
	HBITMAP hBMP;														// Handle Of The Bitmap
	BITMAP	BMP;														// Bitmap Structure

	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), 
		strFileName, IMAGE_BITMAP, 
		0, 
		0, 
		LR_CREATEDIBSECTION | LR_LOADFROMFILE );

	if (!hBMP)															// Does The Bitmap Exist?
	{
		sprintf(caError,"Unable to load BMP File:%s",strFileName);
		return NULL;
	}

	GetObject(hBMP, sizeof(BMP), &BMP);									// Get The Object
	// hBMP:        Handle To Graphics Object
	// sizeof(BMP): Size Of Buffer For Object Information
	// &BMP:        Buffer For Object Information

	

	
	tImage *pImage = new tImage[1];

	pImage->channels = 3;
	pImage->sizeX = BMP.bmWidth;
	pImage->sizeY = BMP.bmHeight;
	//pImage->data  = BMP->bmBits;

	pImage->data = new unsigned char [BMP.bmWidthBytes * BMP.bmHeight];
	memcpy(pImage->data,BMP.bmBits,BMP.bmWidthBytes * BMP.bmHeight);

	DeleteObject(hBMP);													// Delete The Object

	return pImage;
}


///////////////////////////////// LOAD TGA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the TGA file and returns it's data in a tImageTGA struct
/////
///////////////////////////////// LOAD TGA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

tImage *CTexture_GL::LoadTGA(const char *strFileName)
{
	tImage *pImageData = NULL;			// This stores our important image data
	WORD width = 0, height = 0;			// The dimensions of the image
	char length = 0;					// The length in bytes to the pixels
	char imageType = 0;					// The image type (RLE, RGB, Alpha...)
	char bits = 0;						// The bits per pixel for the image (16, 24, 32)
	FILE *pFile = NULL;					// The file pointer
	int channels = 0;					// The channels of the image (3 = RGA : 4 = RGBA)
	int stride = 0;						// The stride (channels * width)
	int i = 0;							// A counter

	char caError[200];

	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		sprintf(caError,"Unable to load TGA File:%s",strFileName);
		// RESTORE RESTORE RESTORE RESTORE 
		//MessageBox(g_pWindow->hWnd, caError, "Error", MB_OK);
		return NULL;
	}
		
	pImageData = new tImage[1];

	fread(&length, sizeof(char), 1, pFile);
	
	fseek(pFile,1,SEEK_CUR); 

	fread(&imageType, sizeof(char), 1, pFile);
	
	fseek(pFile, 9, SEEK_CUR); 

	fread(&width,  sizeof(WORD), 1, pFile);
	fread(&height, sizeof(WORD), 1, pFile);
	fread(&bits,   sizeof(char), 1, pFile);
	
	fseek(pFile, length + 1, SEEK_CUR); 

	// Check if the image is RLE compressed or not
	if(imageType != TGA_RLE)
	{
		// Check if the image is a 24 or 32-bit image
		if(bits == 24 || bits == 32)
		{
			// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
			// Next, we calculate the stride and allocate enough memory for the pixels.
			channels = bits / 8;
			stride = channels * width;
			pImageData->data = new unsigned char [stride*height];

			// Load in all the pixel data line by line
			for(int y = 0; y < height; y++)
			{
				// Store a pointer to the current line of pixels
				unsigned char *pLine = &(pImageData->data[stride * y]);

				// Read in the current line of pixels
				fread(pLine, stride, 1, pFile);
			
				// Go through all of the pixels and swap the B and R values since TGA
				// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
				for(i = 0; i < stride; i += channels)
				{
					int temp     = pLine[i];
					pLine[i]     = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
		// Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;

			// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
			// We then calculate the stride and allocate memory for the pixels.
			channels = 3;
			stride = channels * width;
			pImageData->data = new unsigned char [stride*height];


			// Load in all the pixel data pixel by pixel
			for(int i = 0; i < width*height; i++)
			{
				// Read in the current pixel
				fread(&pixels, sizeof(unsigned short), 1, pFile);
				
				// Convert the 16-bit pixel into an RGB
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;
				
				// This essentially assigns the color to our array and swaps the
				// B and R values at the same time.
				pImageData->data[i * 3 + 0] = r;
				pImageData->data[i * 3 + 1] = g;
				pImageData->data[i * 3 + 2] = b;
			}
		}	
		// Else return a NULL for a bad or unsupported pixel format
		else
			return NULL;
	}
	// Else, it must be Run-Length Encoded (RLE)
	else
	{
		// Create some variables to hold the rleID, current colors read, channels, & stride.
		char rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * width;

		// Next we want to allocate the memory for the pixels and create an array,
		// depending on the channel count, to read in for each pixel.
		pImageData->data = new unsigned char [stride*height];

		char* pColors = new char[channels];

		// Load in all the pixel data
		while(i < width*height)
		{
			// Read in the current color count + 1
			fread(&rleID, sizeof(char), 1, pFile);
			
			// Check if we don't have an encoded string of colors
			if(rleID < 128)
			{
				// Increase the count by 1
				rleID++;

				// Go through and read all the unique colors found
				while(rleID)
				{
					// Read in the current color
					fread(pColors, sizeof(char) * channels, 1, pFile);

					// Store the current pixel in our image array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			// Else, let's read in a string of the same character
			else
			{
				// Minus the 128 ID + 1 (127) to get the color count that needs to be read
				rleID -= 127;

				// Read in the current color, which is the same for a while
				fread(pColors, sizeof(char) * channels, 1, pFile);

				// Go and read as many pixels as are the same
				while(rleID)
				{
					// Assign the current pixel to the current index in our pixel array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
				
			}
				
		}
	}

	// Fill in our tImage structure to pass back
	pImageData->channels = channels;
	pImageData->sizeX    = width;
	pImageData->sizeY    = height;

	/// since tga is y-flipped, i need to flip it ;)

	/*int siz = pImageData->sizeX*pImageData->sizeY*pImageData->channels;  
	unsigned char *pTmp=new unsigned char[siz];  
	int o=0;  

	for (int i=pImageData->sizeY-1;i>=0;i--)  
	{  
		memcpy(&pTmp[o*pImageData->sizeX*pImageData->channels],&pImageData->data[i*pImageData->sizeX*pImageData->channels],pImageData->sizeX*pImageData->channels);  
		o++;  
	}  
	

	SafeDeleteArray(pImageData->data); 
	pImageData->data=pTmp;  */



	if (pFile)
		fclose(pFile);

	// Return the TGA data (remember, you must free this data after you are done)
	return pImageData;
}


///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This decodes the jpeg and fills in the tImage structure
/////
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

/*void CTexture_GL::DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData)
{
	// Read in the header of the jpeg file
	jpeg_read_header(cinfo, TRUE);
	
	// Start to decompress the jpeg file with our compression info
	jpeg_start_decompress(cinfo);

	// Get the image dimensions and channels to read in the pixel data
	pImageData->channels = cinfo->num_components;
	pImageData->sizeX    = cinfo->image_width;
	pImageData->sizeY    = cinfo->image_height;

	// Get the row span in chars for each row
	int rowSpan = cinfo->image_width * cinfo->num_components;
	
	// Allocate memory for the pixel buffer
	pImageData->data = new unsigned char [rowSpan*pImageData->sizeY];
			
	// Create an array of row pointers
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	
	for (int i = 0; i < pImageData->sizeY; i++)
	{
		//rowPtr[i] = &(pImageData->data[i * rowSpan]);
		rowPtr[pImageData->sizeY-i-1] = &(pImageData->data[i * rowSpan]); 
	}

	// Now comes the juice of our work, here we extract all the pixel data
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		// Read in the current row of pixels and increase the rowsRead count
		rowsRead += jpeg_read_scanlines(cinfo, 
										&rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	
	// Delete the temporary row pointers
	//delete [] rowPtr;
	SafeDeleteArray(rowPtr);
	
	// Finish decompressing the data
	jpeg_finish_decompress(cinfo);
}
*/

///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the JPG file and returns it's data in a tImage struct
/////
///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

tImage *CTexture_GL::LoadJPG(const char *strFileName)
{
	// removed support due to:
	// 1. Desire to move to pk3 support (pak)
	// 2. Moving to visual studio 2010

	return NULL;

/*	struct jpeg_decompress_struct cinfo;
	tImage *pImageData = NULL;
	FILE *pFile = NULL;
	char caError[200];

	//	assert(_CrtCheckMemory());
	//BREAKPOINT(!_CrtCheckMemory());
	
	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		sprintf(caError,"Unable to load JPG File:%s",strFileName);
		//BREAKPOINT(1==1);
		assert(1);
		DWORD dwError = GetLastError();
		
		//MessageBox(g_pWindow->hWnd, caError, "Error", MB_OK);
		return NULL;
	}
	
	// Create an error handler
	jpeg_error_mgr jerr;

	// Have our compression info object point to the error handler address
	cinfo.err = jpeg_std_error(&jerr);

	
	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);
	
	// Specify the data source (Our file pointer)	
	jpeg_stdio_src(&cinfo, pFile);
	
	// Allocate the structure that will hold our eventual jpeg data (must free it!)
	//pImageData = new tImage[1];
	pImageData = new tImage;

	// Decode the jpeg file and fill in the image data structure to pass back
	DecodeJPG(&cinfo, pImageData);
	
	// This releases all the stored memory for reading and decoding the jpeg
	jpeg_destroy_decompress(&cinfo);
	
	// Close the file pointer that opened the file
	if (pFile)
		fclose(pFile);

	// Return the jpeg data (remember, you must free this data after you are done)
	return pImageData;
	*/
}


///////////////////////////////// LOAD TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a texture in OpenGL that we can texture map
/////
///////////////////////////////// LOAD TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*


//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This manually changes the gamma of an image
/////
//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/*
void CTexture_GL::ChangeGamma(char *pImage, int size, float factor)
{
//	unsigned int top,r,g,b; 

	int inten=3; 

	char* t_pRGB;

	//if (inten) 
	//{ 
	//	for (int p=0;p<size / 3;p++) 
	//	{ 
	//		t_pRGB = &pImage[p*3];

	//		top=r=t_pRGB[0]<<inten; 
	//		g=t_pRGB[1]<<inten; 
	//		b=t_pRGB[2]<<inten; 
	//		if (g>top) top=g; 
	//		if (b>top) top=b; 

	//		if (top>255) 
	//		{ 
	//			top=(255<<8)/top; 
	//			r=(r*top)>>8; 
	//			g=(g*top)>>8; 
	//			b=(b*top)>>8; 
	//		} 
	//		t_pRGB[0]=(char)r; 
	//		t_pRGB[1]=(char)g; 
	//		t_pRGB[2]=(char)b; 
	//	} 
	//} 

		
	// Go through every pixel in the lightmap
	for(int i = 0; i < size / 3; i++, pImage += 3) 
	{
		float scale = 1.0f, temp = 0.0f;
		float r = 0, g = 0, b = 0;
		
		r = (float)pImage[0];
		g = (float)pImage[1];
		b = (float)pImage[2];
		
		r = r * factor / 255.0f;
		g = g * factor / 255.0f;
		b = b * factor / 255.0f;

		if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
		if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
		if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

		// Get the scale for this pixel and multiply it by our pixel values
		scale*=255.0f;		
		r*=scale;	g*=scale;	b*=scale;

		// Assign the new gamma'nized RGB values
		pImage[0] = (char)r;
		pImage[1] = (char)g;
		pImage[2] = (char)b;
	}


}


////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a texture map from the light map image bits
/////
////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CTexture_GL::LoadLightmapTexture(char *pImageBits, int width, int height)
{
	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &m_uiTextureID);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);

	// Change the lightmap gamma values by our desired gamma
	//ChangeGamma(pImageBits, width*height*3, g_fGamma);
	ChangeGamma(pImageBits, width*height*3, 7.f);

	glTexImage2D(GL_TEXTURE_2D,0,3, width,height,0,GL_RGB,GL_UNSIGNED_BYTE,pImageBits);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
}
*/
/*
bool CTexture::LoadSkyBoxTextures(const char* strFileName)
{
	int t_strLen = strlen(strFileName);

	m_TexName = new char[t_strLen+1];
	strcpy(m_TexName,strFileName);

//	"env/test_rt.tga", "env/test_lf.tga",
//	"env/test_up.tga", "env/test_dn.tga"
//	"env/test_ft.tga", "env/test_bk.tga", 

	char* cppSidesNames[6];


	for (int i=0;i<6;i++)
	{
		cppSidesNames[i] = new char[t_strLen+8];
		strcpy(cppSidesNames[i],strFileName);
	}
	

	cppSidesNames[0][t_strLen] = '_';
	cppSidesNames[0][t_strLen+1] = 'r';
	cppSidesNames[0][t_strLen+2] = 't';
	cppSidesNames[0][t_strLen+3] = NULL;

	cppSidesNames[1][t_strLen] = '_';
	cppSidesNames[1][t_strLen+1] = 'l';
	cppSidesNames[1][t_strLen+2] = 'f';
	cppSidesNames[1][t_strLen+3] = NULL;

	cppSidesNames[2][t_strLen] = '_';
	cppSidesNames[2][t_strLen+1] = 'u';
	cppSidesNames[2][t_strLen+2] = 'p';
	cppSidesNames[2][t_strLen+3] = NULL;

	cppSidesNames[3][t_strLen] = '_';
	cppSidesNames[3][t_strLen+1] = 'd';
	cppSidesNames[3][t_strLen+2] = 'n';
	cppSidesNames[3][t_strLen+3] = NULL;

	cppSidesNames[4][t_strLen] = '_';
	cppSidesNames[4][t_strLen+1] = 'f';
	cppSidesNames[4][t_strLen+2] = 't';
	cppSidesNames[4][t_strLen+3] = NULL;

	cppSidesNames[5][t_strLen] = '_';
	cppSidesNames[5][t_strLen+1] = 'b';
	cppSidesNames[5][t_strLen+2] = 'k';
	cppSidesNames[5][t_strLen+3] = NULL;

	for (int i=0;i<6;i++)
		FindTextureExtension(cppSidesNames[i]);


	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &m_uiTextureID);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_uiTextureID);


	// Define a pointer to a tImage
	tImage *pImage = NULL;

	GLenum t_dw;

	
	for (int i=0;i<6;i++)
	{
		
		// If the file is a jpeg, load the jpeg and store the data in pImage
		if(strstr(cppSidesNames[i], ".jpg"))
		{
			pImage = LoadJPG(cppSidesNames[i]);
		}
		// If the file is a tga, load the tga and store the data in pImage
		else if(strstr(cppSidesNames[i], ".tga"))
		{
			pImage = LoadTGA(cppSidesNames[i]);
		}
		// If the file is a bitmap, load the bitmap and store the data in pImage
		else if(strstr(cppSidesNames[i], ".bmp"))
		{
			pImage = LoadBMP(cppSidesNames[i]);
		}
		// Else we don't support the file format that is trying to be loaded
		else
			return false;

		// Make sure valid image data was given to pImage, otherwise return false
		if(pImage == NULL)								
			return false;

		

		// Assume that the texture is a 24 bit RGB texture (We convert 16-bit ones to 24-bit)
		int textureType = GL_RGB;

		// If the image is 32-bit (4 channels), then we need to specify GL_RGBA for an alpha
		if(pImage->channels == 4)
			textureType = GL_RGBA;

		if (i==0)
			t_dw=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
		else
		if (i==1)
			t_dw=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
		else
		if (i==2)
			t_dw=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
		else
		if (i==3)
			t_dw=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
		else
		if (i==4)
			t_dw=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
		else
		if (i==5)
			t_dw=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
		


		GLenum t_OpenGLError = glGetError();

		glTexImage2D(t_dw,0,pImage->channels, pImage->sizeX,pImage->sizeY,0,textureType,GL_UNSIGNED_BYTE,pImage->data);
		
		
	
		t_OpenGLError = glGetError();
		char* cpTempError = (char*) gluErrorString(t_OpenGLError);

		
		
	

		if (pImage)										// If we loaded the image
		{
			if (pImage->data)							// If there is texture data
			{
				//free(pImage->data);						// Free the texture data, we don't need it anymore
				SafeDeleteArray(pImage->data);
			}
	
			//free(pImage);								// Free the image structure
			SafeDelete(pImage);
		}

		pImage=NULL;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	//FIXME: remember to check if the card supports that...

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);



	return true;

}

*/


/*bool CTexture::Generate(int iWidth, int iHeight)
{
	glGenTextures(1, &m_uiTextureID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	char temp[3*iWidth*iHeight];

	glTexImage2D(GL_TEXTURE_2D,0,3, iWidth,iHeight,0,GL_RGB,GL_UNSIGNED_BYTE,temp);
}*/


//TODO: lower case name
bool CTexture_GL::Load(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress)
{
	// i need to avoid all the string copies 
	// which i'm doing because of the find-extension process (for q3 maps)...

	//don't compress normal maps

	if(!strFileName) 
		return false;

	static char ExtendedName[500];
	
	//strlwr(


	strcpy(ExtendedName,strFileName);

	if (!FindTextureExtension(ExtendedName))
		return false;

	
	m_TexName = new char[strlen(ExtendedName)+1];
	strcpy(m_TexName,ExtendedName);

	// Define a pointer to a tImage
	tImage *pImage = NULL;

	// If the file is a jpeg, load the jpeg and store the data in pImage
	if(strstr(ExtendedName, ".jpg"))
	{
		pImage = LoadJPG(ExtendedName);
	}
	// If the file is a tga, load the tga and store the data in pImage
	else if(strstr(ExtendedName, ".tga"))
	{
		pImage = LoadTGA(ExtendedName);
	}
	// If the file is a bitmap, load the bitmap and store the data in pImage
	else if(strstr(ExtendedName, ".bmp"))
	{
		pImage = LoadBMP(ExtendedName);
	}
	// Else we don't support the file format that is trying to be loaded
	else
	{
		g_pLogFile->OutPutPlainText("image format not supported!",LOG_MESSAGE_WARNING_LVL_0);
		
		return false;
	}

	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage == NULL)		
	{
		char message[350];
		sprintf_s(message,350,"Texture file:""%s"" not found!",ExtendedName);
		g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_WARNING_LVL_0);
		return false;
	}


	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &m_uiTextureID);
	

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);

	// Assume that the texture is a 24 bit RGB texture (We convert 16-bit ones to 24-bit)
	int textureType = GL_RGB;

	// If the image is 32-bit (4 channels), then we need to specify GL_RGBA for an alpha
	if(pImage->channels == 4)
		textureType = GL_RGBA;
		
	// Build Mipmaps
    if (bMipMap==true)
	{
			// Set a filtering value of GL_LINEAR_MIPMAP_LINEAR to take advantage of
		// anisotropic texture filtering

		//trilinear
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// bilinear
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

		if (bCompress)
		{
		  if (pImage->channels==4) 
          { 
               pImage->channels=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
               pImage->channels=GL_COMPRESSED_RGBA_ARB; 
 
          } 
          else if (pImage->channels==3) 
          { 
               pImage->channels=GL_COMPRESSED_RGB_S3TC_DXT1_EXT; 
               pImage->channels=GL_COMPRESSED_RGB_ARB; 
          } 
		}

		//gluBuild2DMipmaps( GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, void *data ) 
		gluBuild2DMipmaps(GL_TEXTURE_2D, pImage->channels, pImage->sizeX, 
			pImage->sizeY, textureType, GL_UNSIGNED_BYTE, pImage->data);

	


	}
	else
	{		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

				if (bCompress)
		{
		  if (pImage->channels==4) 
          { 
               pImage->channels=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
               pImage->channels=GL_COMPRESSED_RGBA_ARB; 
 
          } 
          else if (pImage->channels==3) 
          { 
               pImage->channels=GL_COMPRESSED_RGB_S3TC_DXT1_EXT; 
               pImage->channels=GL_COMPRESSED_RGB_ARB; 
          } 
		}


		//glTexImage2D( GLenum target, GLint level, GLint components, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) 
		glTexImage2D(GL_TEXTURE_2D,0,pImage->channels, pImage->sizeX,pImage->sizeY,0,textureType,GL_UNSIGNED_BYTE,pImage->data);
		
	}


	/*

	       // trilinear 
          if (iOptions&TEXOPT_TRILINEAR) 
          { 
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
          } 
          else 
          { 
               // bilinear 
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
          }
 
     if (!(iOptions&TEXOPT_NOCOMPRESS)) 
     { 
          if (iFormat==4) 
          { 
               iFormat=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
               iFormat=GL_COMPRESSED_RGBA_ARB; 
 
          } 
          else if (iFormat==3) 
          { 
               iFormat=GL_COMPRESSED_RGB_S3TC_DXT1_EXT; 
               iFormat=GL_COMPRESSED_RGB_ARB; 
          } 
     } 
 
     // Build Mipmaps (builds different versions of the picture for distances - looks better) 
     gluBuild2DMipmaps(GL_TEXTURE_2D, iFormat , pImage->iSizeX,  
          pImage->iSizeY, iTextureType, GL_UNSIGNED_BYTE, pImage->pData);
	*/

	
	if (bClamp==true)
	{
		/*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);*/
		// remember this option for sky boxes ( BUT CHECK IF THE CARD SUPPORTS THIS!)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

	
	// Largest Anistropic Texture Ratio
	GLfloat fLargestSupportedAnisotropy;
	
	// Determine largest supported anistropic texture ratio
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargestSupportedAnisotropy);

	// Set anisotropic texture filter ratio 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &fLargestSupportedAnisotropy);


	if (pImage)										// If we loaded the image
	{
		/*if (pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}*/
		SafeDeleteArray(pImage->data);

		SafeDelete(pImage);								// Free the image structure
	}

	return true;
}












