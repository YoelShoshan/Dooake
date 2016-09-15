#include "stdafx.h"
#include "ShaderParser.h"


#include <GL/gl.h>

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include <fstream>

#include "FileManager.h"
CFileManager g_FileManager;


#include "Singleton.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

CShaderParser::CShaderParser()
{
}

CShaderParser::~CShaderParser()
{
}


void CShaderParser::BuildFileList(char* Path, char* What) // for now only search one path
{
	g_pLogFile->OutPutPlainText("Building File List:","3","000000",true,LOG_MESSAGE_INFORMATIVE);
	m_ShaderCacheFileVector.reserve(300);
	g_FileManager.FillFileList(Path,What,m_ShaderCacheFileVector);
	
}



void CShaderParser::CacheFile(const char* FileName,int filenum)
{

	char s1[264],s2[264],s3[64],s4[64],s5[64],s6[64],s7[64],s8[64],s9[64],s10[64];

	SCacheObject TempCacheObj;

	std::streamoff sStreamOff;
	std::fpos<mbstate_t> t_FilePos;

		
	std::ifstream t_ScriptFile;

	t_ScriptFile.open(FileName);


	if(t_ScriptFile.fail())
	{	
		//BREAKPOINT(1==1);
		assert(1);
		// LOG: log here
		//cout << "ERROR: Could not find Stats.txt!\n";
		return;										
	}	

	int t_iPos = 0;
	int num_args =0;
	int iCounter = 0;


	std::string t_CurrentLine;


	while(std::getline(t_ScriptFile, t_CurrentLine))
	{

		t_iPos++;

		if (t_CurrentLine[0]=='/')  // also checking before because of that old bug...
			continue;

		num_args=sscanf(t_CurrentLine.c_str(),"%s %s %s %s %s %s %s %s %s %s",s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);

		if (num_args<=0) 
		{
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
			continue;
		}
	
	
		if (s1[0]=='/')  // only supporting c comments for now
		{
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
			continue;
		}

		if ( (strcmp(s1,"{") == 0)) // a shader/shader pass is starting
		{
			iCounter++;
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
			continue;
		}
		else
		if ( (strcmp(s1,"}") == 0)) // a shader/shader pass is ending
		{
			iCounter--;
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
			continue;
		}


		/// if it's 0 then it means that the next line (if ignoring white spaces and comments)
		//  will be the next shader name.
		//  TODO: check if it's ok (maybe first or last aren't treated well???)
		if (iCounter==0)
		{
			
			TempCacheObj.iFileNum = filenum;
										
			t_FilePos = t_ScriptFile.tellg();		

			TempCacheObj.iPos = (int) t_FilePos;

			strlwr(s1);
			m_ShaderCacheMap.insert(SHADER_CACHE_MAP::value_type(s1,TempCacheObj));

			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
		}

		s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
	}

	t_ScriptFile.close();

}

/*void CShaderParser::DebugOutPutCacheMapToFile(void)
{
	std::ofstream t_File;
	t_File.open("EntireCache.txt");

	if(t_File.fail())
	{
		BREAKPOINT(1==1);

	}

	m_ShaderCacheMapIT = m_ShaderCacheMap.begin();

	while (m_ShaderCacheMapIT != m_ShaderCacheMap.end())
	{

		t_File << m_ShaderCacheMapIT->first.c_str();
		t_File << ":";
		t_File << m_ShaderCacheMapIT->second.iFileNum;
		t_File << ",";
		t_File << m_ShaderCacheMapIT->second.iPos;
		t_File << ".";
		t_File << "\n";

		m_ShaderCacheMapIT++;
	}

	t_File.close();	
}*/

void CShaderParser::BuildCacheMap(void)
{

	for (int i=0;i<m_ShaderCacheFileVector.size();i++)
	{
		g_pLogFile->OutPutPlainText("<li> ","3","000000",false,LOG_MESSAGE_INFORMATIVE);
		g_pLogFile->OutPutPlainText("Caching ","3","000000",false,LOG_MESSAGE_INFORMATIVE);
		g_pLogFile->OutPutPlainText(m_ShaderCacheFileVector[i].c_str(),"3","000000",false,LOG_MESSAGE_INFORMATIVE);
		g_pLogFile->OutPutPlainText(".","3","009900",true,LOG_MESSAGE_INFORMATIVE);
		CacheFile(m_ShaderCacheFileVector[i].c_str(),i);
		
	}

}

bool  CShaderParser::FindInCache(const char* find)
{
	m_ShaderCacheMapIT = m_ShaderCacheMap.begin();


	char* findLowered;

	findLowered = new char[strlen(find)+1];
	strcpy(findLowered,find);
    
	//
	strlwr(findLowered);
	//

		
	m_ShaderCacheMapIT = m_ShaderCacheMap.find(findLowered);
	
	if (m_ShaderCacheMapIT!=m_ShaderCacheMap.end())
	{
		return true;  // it was found in the cache
	}
	else
	{		
        return false; // it wasn't found in the cache
	}
}

void CShaderParser::DebugDrawCacheMap(void)
{
	m_ShaderCacheMapIT = m_ShaderCacheMap.begin();

	int TextY=4;
	int iLocation=0;

    
}

void CShaderParser::FindTextureExtension(char *strFileName)
{

	std::string strJPGPath;
	std::string strTGAPath;
	std::string strBMPPath;

	FILE *fp = NULL;

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
		return;
	}

	if((fp = fopen(strTGAPath.c_str(), "rb")) != NULL)
	{
		strcat(strFileName, ".tga");
		fclose(fp);
		return;
	}

	if((fp = fopen(strBMPPath.c_str(), "rb")) != NULL)
	{
		strcat(strFileName, ".bmp");
		fclose(fp);
		return;
	}



	// TODO: maybe return a filename of an image with text "NO EXTENSION FOUND"
}


CQ3A_Shader* CShaderParser::ParseShader(char* ShaderName)
{
	std::ifstream t_ScriptFile;

	CQ3A_Shader*     pShader;
	CQ3A_ShaderPass* pShaderPass;

	std::fpos<mbstate_t> t_FilePos;


	strlwr(ShaderName);


	m_ShaderCacheMapIT = m_ShaderCacheMap.begin();
	m_ShaderCacheMapIT = m_ShaderCacheMap.find(ShaderName);

	bool bShaderInCache=false;

	if (m_ShaderCacheMapIT!=m_ShaderCacheMap.end())
		bShaderInCache=true;
	
    
	if (bShaderInCache==true) // the shader IS FOUND in the cache
	{
		int iFileNum = (*m_ShaderCacheMapIT).second.iFileNum;
		int iPos     = (*m_ShaderCacheMapIT).second.iPos;
		

		assert(iFileNum>=0 && iPos>=0);

			t_ScriptFile.open(m_ShaderCacheFileVector[iFileNum].c_str());

			if(t_ScriptFile.fail())
			{	
				//BREAKPOINT(1==1)
				assert(1==1);
				// LOG: log here
				//cout << "ERROR: Could not find Stats.txt!\n";
				// maybe return here an "error" shader
				//return;										
			}

			int iLineNum = 0;
			
			std::string t_CurrentLine;


			t_FilePos = iPos;
			t_ScriptFile.seekg(t_FilePos);


			// HERE WAS MY PROBLEM!!!!!!!!!

			pShader = new CQ3A_Shader;

			pShader->SetScripted(true);

			/////////////////////////
			//gex_iDebugSPNewSHADERCalls++;
			/////////////////////////


			pShader->SetName(ShaderName);

//			BREAKPOINT(strcmp(ShaderName,"textures/sfx/fanfx")==0);
						
			// default values
			pShader->SetShaderSort(SORT_NOT_INIT);

			char s1[200],s2[200],s3[200],s4[200],s5[200],s6[200],s7[200],s8[200],s9[200],s10[200];

			int iCounter = 0;
			int num_args = 0;


			//////////////////////////////
			// Parsing the shader globals
			///////////////////////////////


			DWORD t_dwShaderFlags=FLAG_CULLFRONT;
			DWORD t_dwSurfaceParm=SURFACEPARM_NONE;

			bool  bForceDepthWrite=false;
			bool  bNoMipMaps=false;

			bool bNoShaderPasses=false;

			UINT t_uiWaveType=WF_SIN;


			int iTempSkyPassNum=0;
			

			
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;

			while(std::getline(t_ScriptFile,t_CurrentLine))
			{
			iLineNum++;

			if (t_CurrentLine[0]=='/')  // also checking before because of that old bug...
			continue;

			num_args=sscanf(t_CurrentLine.c_str(),"%s %s %s %s %s %s %s %s %s %s",s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);

			if (num_args<=0) continue;

			strlwr(s1);strlwr(s2);strlwr(s3);strlwr(s4);strlwr(s5);
			strlwr(s6);strlwr(s7);strlwr(s8);strlwr(s9);strlwr(s10);

			//BREAKPOINT(strcmp(s1,"cull")==0);

			/// this used to cause a bug when the comment was too long
			if (s1[0]=='/')  // only supporting c comments,
			{
				s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				continue;
			}

			if ( (strcmp(s1,"{") == 0))
			{
				s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				iCounter++;
				//break;
			}

			if ( (strcmp(s1,"}") == 0))
			{
				// if a '}' is found then this file has NO SHADER PASSES!!!!!
				s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				bNoShaderPasses=true;
				iCounter++;
				//break;
			}

			if (iCounter>1)
				break;


			//TODO: I'm not doing this two correctly - don't quite understand them

			if (strcmp(s1,"nomipmaps")==0)     // nomipmap
			{
				bNoMipMaps = true;
			}

			if (strcmp(s1,"nopicmip")==0)      //nopicmip
			{
				bNoMipMaps = true;
			}

			if (strcmp(s1,"skyparms")==0)
			{				
				t_dwSurfaceParm|=SURFACEPARM_SKY;
				pShader->SetShaderSort(SORT_OPAQUE+SORT_ONE);

				// TODO - Handle Sky
				/*g_pSky->m_strSkyBoxShaderName = new char [strlen(ShaderName)+1];
				strcpy(g_pSky->m_strSkyBoxShaderName,ShaderName);

				//int iTempSide=0;
				
				// This is causing a leak because the textures that i get fom LoadTexAndPush aren't
				// stored in a regular instead of a smart pointer
				// therefore nothing is causing it to be deleted.
				
				if (strcmp(s2,"-")!=0 && strcmp(s2,"full")!=0)
				{
					ITexture* pTempTex=NULL;
					int iTempLength = strlen(s2);
					s2[iTempLength] = '_';

					s2[iTempLength+1] = 'l';s2[iTempLength+2] = 'f';s2[iTempLength+3] = NULL;					
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(0,pTempTex);

					s2[iTempLength+1] = 'u';s2[iTempLength+2] = 'p';s2[iTempLength+3] = NULL;
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(1,pTempTex);

					s2[iTempLength+1] = 'b';s2[iTempLength+2] = 'k';s2[iTempLength+3] = NULL;
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(2,pTempTex);

					s2[iTempLength+1] = 'r';s2[iTempLength+2] = 't';s2[iTempLength+3] = NULL;
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(3,pTempTex);
													
					s2[iTempLength+1] = 'd';s2[iTempLength+2] = 'n';s2[iTempLength+3] = NULL;
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(4,pTempTex);

					s2[iTempLength+1] = 'f';s2[iTempLength+2] = 't';s2[iTempLength+3] = NULL;
					FindTextureExtension(s2);
					pTempTex = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(s2,false,true,true);
					g_pSky->SetSideTexture(5,pTempTex);
													
					pShader->SetFarBoxUsed(true);
					iTempSkyPassNum = 1;
					
				}*/

			}

			if (strcmp(s1,"surfaceparm")==0)
			{

				// add elses

				/*if (strcmp(s2,"alphashadow")==0)
					t_dwSurfaceParm|=SURFACEPARM_ALPHASHADOW;
				else

				if (strcmp(s2,"areaportal")==0)
					t_dwSurfaceParm|=SURFACEPARM_AREAPORTAL;
				else

				if (strcmp(s2,"clusterportal")==0)
					t_dwSurfaceParm|=SURFACEPARM_CLUSTERPORTAL;
				else

				if (strcmp(s2,"donotenter")==0)
					t_dwSurfaceParm|=SURFACEPARM_DONOTENTER;
				else

				if (strcmp(s2,"flesh")==0)
					t_dwSurfaceParm|=SURFACEPARM_FLESH;
				else

				if (strcmp(s2,"fog")==0)
					t_dwSurfaceParm|=SURFACEPARM_FOG;
				else

				if (strcmp(s2,"lava")==0)
					t_dwSurfaceParm|=SURFACEPARM_LAVA;
				else

				if (strcmp(s2,"metalsteps")==0)
					t_dwSurfaceParm|=SURFACEPARM_METALSTEPS;
				else

				if (strcmp(s2,"nodamage")==0)
					t_dwSurfaceParm|=SURFACEPARM_NODAMAGE;
				else

				if (strcmp(s2,"nodlight")==0)
					t_dwSurfaceParm|=SURFACEPARM_NODLIGHT;
				else

				
				if (strcmp(s2,"nodrop")==0)
					t_dwSurfaceParm|=SURFACEPARM_NODROP;
				else

				if (strcmp(s2,"noimpact")==0)
					t_dwSurfaceParm|=SURFACEPARM_NOIMPACT;
				else

				if (strcmp(s2,"nomarks")==0)
					t_dwSurfaceParm|=SURFACEPARM_NOMARKS;
				else

				if (strcmp(s2,"nolightmap")==0)
					t_dwSurfaceParm|=SURFACEPARM_NOLIGHTMAP;
				else

				if (strcmp(s2,"nosteps")==0)
					t_dwSurfaceParm|=SURFACEPARM_NOSTEPS;
				else

				if (strcmp(s2,"nonsolid")==0)
					t_dwSurfaceParm|=SURFACEPARM_NONSOLID;
				else

				if (strcmp(s2,"origin")==0)
					t_dwSurfaceParm|=SURFACEPARM_ORIGIN;
				else

				if (strcmp(s2,"playerclip")==0)
					t_dwSurfaceParm|=SURFACEPARM_PLAYERCLIP;
				else

				if (strcmp(s2,"slick")==0)
					t_dwSurfaceParm|=SURFACEPARM_SLICK;
				else

				if (strcmp(s2,"slime")==0)
					t_dwSurfaceParm|=SURFACEPARM_SLIME;
				else

				if (strcmp(s2,"structural")==0)
					t_dwSurfaceParm|=SURFACEPARM_STRUCTURAL;
				else*/

				if (strcmp(s2,"trans")==0)
				{
					t_dwSurfaceParm|=SURFACEPARM_TRANS;
					//pShader->SetShaderSort(SORT_ADDITIVE);
				}
				else
				if (strcmp(s2,"water")==0)
				{
					t_dwSurfaceParm|=SURFACEPARM_WATER;
				}
				else
				if (strcmp(s2,"sky")==0)
				{
					// all this is already done in skyparms

					/*t_dwSurfaceParm|=SURFACEPARM_SKY;
					pShader->SetShaderSort(SORT_OPAQUE+SORT_ONE);

					g_Sky.m_strSkyBoxShaderName = new char [strlen(ShaderName)+1];
					strcpy(g_Sky.m_strSkyBoxShaderName,ShaderName);*/

				}
				else
				if (strcmp(s2,"nodraw")==0)
					t_dwSurfaceParm|=SURFACEPARM_NODRAW;
			}
			else
			if (strcmp(s1,"cull")==0)     // cull
			{

				if (strcmp(s2,"disable")==0 || strcmp(s2,"none")==0)
				{
					t_dwShaderFlags|=FLAG_NOCULL;
					t_dwShaderFlags&=~FLAG_CULLBACK;
					t_dwShaderFlags&=~FLAG_CULLFRONT;
				}

				if (strcmp(s2,"front")==0)
				{
					t_dwShaderFlags|=FLAG_CULLFRONT;
					t_dwShaderFlags&=~FLAG_CULLBACK;
					t_dwShaderFlags&=~FLAG_NOCULL;
				}

				if (strcmp(s2,"back")==0)
				{
					t_dwShaderFlags|=FLAG_CULLBACK;
					t_dwShaderFlags&=~FLAG_NOCULL;
					t_dwShaderFlags&=~FLAG_CULLFRONT;
				}
			}
			else
			if (strcmp(s1,"sort")==0)     // sort
			{				
				if (strcmp(s2,"1")==0 || strcmp(s2,"portal")==0)
					pShader->SetShaderSort(SORT_PORTAL);
				else
				if (strcmp(s2,"2")==0 || strcmp(s2,"sky")==0)
					pShader->SetShaderSort(SORT_SKY);
				else
				if (strcmp(s2,"3")==0 || strcmp(s2,"opaque")==0)
				{
					pShader->SetShaderSort(SORT_OPAQUE);
					t_dwShaderFlags|=SPF_DEPTHWRITE;
				}
				else
				if (strcmp(s2,"6")==0 || strcmp(s2,"banner")==0)
					pShader->SetShaderSort(SORT_BANNER);
				else
				if (strcmp(s2,"8")==0 || strcmp(s2,"underwater")==0)
					pShader->SetShaderSort(SORT_UNDERWATER);
				else
				if (strcmp(s2,"9")==0 || strcmp(s2,"additive")==0)
				{
					pShader->SetShaderSort(SORT_ADDITIVE);
					t_dwShaderFlags&=~SPF_DEPTHWRITE;
				}
				else
				if (strcmp(s2,"16")==0 || strcmp(s2,"nearest")==0)
					pShader->SetShaderSort(SORT_NEAREST);				
			}
			else
			if (strcmp(s1,"deformvertexes")==0)     // deformvertexes
			{
				if (strcmp(s2,"move")==0)
				{
					if (strcmp(s6,"sin")==0)
						t_uiWaveType = WF_SIN;
					else
					if (strcmp(s6,"triangle")==0)
						t_uiWaveType = WF_TRIANGLE;
					else
					if (strcmp(s6,"square")==0)
						t_uiWaveType = WF_SQUARE;
					else
					if (strcmp(s6,"sawtooth")==0)
						t_uiWaveType = WF_SAWTOOTH;
					else
					if (strcmp(s6,"inversesawtooth")==0)
						t_uiWaveType = WF_INVERSESAWTOOTH;

					pShader->AddDeformVertexesMove(atof(s3),atof(s5),-atof(s4),t_uiWaveType,atof(s7),atof(s8),atof(s9),atof(s10));
				}

				if (strcmp(s2,"wave")==0)
				{
					if (strcmp(s4,"sin")==0)
						t_uiWaveType = WF_SIN;
					else
					if (strcmp(s4,"triangle")==0)
						t_uiWaveType = WF_TRIANGLE;
					else
					if (strcmp(s4,"square")==0)
						t_uiWaveType = WF_SQUARE;
					else
					if (strcmp(s4,"sawtooth")==0)
						t_uiWaveType = WF_SAWTOOTH;
					else
					if (strcmp(s4,"inversesawtooth")==0)
						t_uiWaveType = WF_INVERSESAWTOOTH;

					pShader->AddDeformVertexesWave(atof(s3),t_uiWaveType,atof(s5),atof(s6),atof(s7),atof(s8));
				}

				if (strcmp(s2,"autosprite")==0)
				{
					pShader->AddDeformVertexesAutoSprite();
				}

				if (strcmp(s2,"bulge")==0)
				{
					pShader->AddDeformVertexesBulge(atof(s3),atof(s4),atof(s5));
				}

				//TODO: autosprite2

			}


			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
			}
			
			s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;




			
			if (iTempSkyPassNum==1)
			{
				pShaderPass = new CQ3A_ShaderPass;
				//pShaderPass->SetAlreadyAllocatedNoAnimTexture(g_Sky.m_puiTexturesIndex[0]);
				//pShaderPass->SetAlreadyAllocatedNoAnimTexture(g_pSky->m_spTexturesIndex[0]->GetID());
				pShaderPass->SetAlreadyAllocatedNoAnimTexture(0/*"sky not implemented yet"*/);
				
				pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);
				pShaderPass->SetTCGen(TCGEN_SKY_CUBEMAP);
				pShaderPass->SetShaderPassFlags(0);
				pShader->ChangePass(pShaderPass,0);
			}


			/////////////////////////////
			// parsing the shader passes
			////////////////////////////

			if (bNoShaderPasses==false)
			{
			
				bool bEndOfShaderPasses = false;			
				int iTemp=0;
			
				iCounter = 1;
				num_args = 0;

				// shader pass helpers
            
				DWORD t_dwSrcBlend=GL_ONE;
				DWORD t_dwDstBlend=GL_ZERO;

				
				float t_f1=0.f,t_f2=0.f,t_f3=0.f,t_f4=0.f;
				int iNumOfFrames=1;
				int iCurrentPassNum=0;

				bool bWasFirst=true;

				DWORD t_dwShaderPassFlags;

				
		

				while (iCounter>=0)
				{
				
					while(std::getline(t_ScriptFile,t_CurrentLine))
					{
						iLineNum++;
						if (t_CurrentLine[0]=='/')  // also checking before because of that old bug...
							continue;

						num_args=sscanf(t_CurrentLine.c_str(),"%s %s %s %s %s %s %s %s %s %s",s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);

						if (num_args<=0) continue;

						strlwr(s1);strlwr(s2);strlwr(s3);strlwr(s4);strlwr(s5);
						strlwr(s6);strlwr(s7);strlwr(s8);strlwr(s9);strlwr(s10);

						if (s1[0]=='/')  // only supporting c comments for now
							continue;

						if (bWasFirst==true)
						{
							bWasFirst=false;
							//
							pShaderPass = new CQ3A_ShaderPass;
							pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);							
							pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
							t_dwShaderPassFlags = SPF_DEPTHWRITE; // default flags
							//
						}

						if ( (strcmp(s1,"{") == 0))
							{
								iCounter++;
								//
								pShaderPass = new CQ3A_ShaderPass;
								pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);							
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								t_dwShaderPassFlags = SPF_DEPTHWRITE; // default flags
								//
								continue;
							}
						else
						if ( (strcmp(s1,"}") == 0))
							{
								iCounter--;
								if (iCounter==0)
								{
									iCurrentPassNum++;
									pShaderPass->SetShaderPassFlags(t_dwShaderPassFlags);

									if (!(t_dwSurfaceParm & SURFACEPARM_SKY))
									{
										pShader->AddPass(pShaderPass);
									}
									else
									{
										pShader->ChangePass(pShaderPass,iTempSkyPassNum++); 
									}

								}
								continue;
							}

						if (iCounter<=0)
							break;



					/// keywords ragarding WHICH texture to use
					
					if (strcmp(s1,"map")==0)
					{
						//BREAKPOINT(strcmp(s2,"textures/skies/tim_hell.")==0);

						if (strcmp(s2,"$lightmap")==0)
						{
							if (iCurrentPassNum==0) // TODO:ASK SOMEONE IF THIS IS HACKY OR THE SOLUTION!!!
							{								
								pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								//pShaderPass->AddNoAnimTexture("ceImages/error.jpg",!bNoMipMaps,false);								
								pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);// 
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								t_dwShaderPassFlags |= SPF_ISLIGHTMAP; // default

							}
							else
							{
							    pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								//pShaderPass->AddNoAnimTexture("ceImages/error.jpg",!bNoMipMaps,false);								
								pShaderPass->SetBlendValues(GL_DST_COLOR,GL_ZERO);// modulate
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								t_dwShaderPassFlags |= SPF_ISLIGHTMAP; // default					
							}
						}
						else
						if (strcmp(s2,"$whiteimage")==0)
						{
							if (iCurrentPassNum==0) // TODO:ASK SOMEONE IF THIS IS HACKY OR THE SOLUTION!!!
							{
								//Restore this
								//pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								//DEBUG ONLY!
								//pShaderPass->AddNoAnimTexture("ceImages/debug.jpg",!bNoMipMaps,false);
								pShaderPass->AddNoAnimTexture("ceData/white.jpg",!bNoMipMaps,false);
								pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);// modulate
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
							}
							else
							{
								//Restore this
								//pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed								
								//DEBUG ONLY!
								//pShaderPass->AddNoAnimTexture("ceImages/debug.jpg",!bNoMipMaps,false);
								pShaderPass->AddNoAnimTexture("ceData/white.jpg",!bNoMipMaps,false);
								// TODO:if get == one,zero
								pShaderPass->SetBlendValues(GL_ONE,GL_ONE);// modulate
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY); // maybe I shouldn't do all those?
							}

						}
						else  // map texturename
						{

							/*BREAKPOINT(strcmp(s2,"/models/mapobjects/hand/handgood.bmp")==0);
							BREAKPOINT(strcmp(s2,"/models/mapobjects/hand/handgood.jpg")==0);
							BREAKPOINT(strcmp(s2,"/models/mapobjects/hand/handgood.tga")==0);*/
							
							s2[strlen(s2)-4]=NULL;
							FindTextureExtension(s2);

							/*BREAKPOINT(strcmp(s2,"/models/mapobjects/hand/handgood")==0);*/
							
							//if (bNoMipMaps==false)
							pShaderPass->AddNoAnimTexture(s2,!bNoMipMaps,false);

						}
					}
					else
					if (strcmp(s1,"clampmap")==0)
					{
						if (strcmp(s2,"$lightmap")==0)
						{
							if (iCurrentPassNum==0) // TODO:ASK SOMEONE IF THIS IS HACKY OR THE SOLUTION!!!
							{								
								pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);// 
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								t_dwShaderPassFlags |= SPF_ISLIGHTMAP; // default

							}
							else
							{
								pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								pShaderPass->SetBlendValues(GL_DST_COLOR,GL_ZERO);// modulate
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								t_dwShaderPassFlags |= SPF_ISLIGHTMAP; // default					
							}
						}
						else
							if (strcmp(s2,"$whiteimage")==0)
							{
								if (iCurrentPassNum==0) // TODO:ASK SOMEONE IF THIS IS HACKY OR THE SOLUTION!!!
								{
								//Restore this
								//pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
								//DEBUG ONLY!
								//pShaderPass->AddNoAnimTexture("ceImages/debug.jpg",!bNoMipMaps,false);
									pShaderPass->AddNoAnimTexture("ceData/white.jpg",!bNoMipMaps,false);
								pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);// modulate
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								}
								else
								{
								//Restore this
								//pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed								
								//DEBUG ONLY!
								//pShaderPass->AddNoAnimTexture("ceImages/debug.jpg",!bNoMipMaps,false);
								pShaderPass->AddNoAnimTexture("ceData/white.jpg",!bNoMipMaps,false);
								pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
								}

							}
							else  // map texturename
							{
								s2[strlen(s2)-4]=NULL;
								FindTextureExtension(s2);
								pShaderPass->AddNoAnimTexture(s2,!bNoMipMaps,true);						
							}
					}
					else
					if (strcmp(s1,"animmap")==0)// TODO:I should add AddFrameTexture(1,s2,false/true,false/true);
					{
						
						//TODO: maybe there are comments amongst them
						//      actually - that's a global check that should be checked for eveything
						iNumOfFrames=num_args-2;

						if (iNumOfFrames<1)
							iNumOfFrames=1;

						pShaderPass->SetAnim(iNumOfFrames,atof(s2));						


						///////
						// TODO: THIS SHOULDN'T BE HARD CODED...
						//       The strings should be in some sort of array
						/////

						
						if (iNumOfFrames>0)
						{
							iTemp = strlen(s3)-4;
							s3[iTemp]=NULL;
							FindTextureExtension(s3);
							pShaderPass->AddFrameTexture(0,s3);
						}
						if (iNumOfFrames>1)
						{
							s4[strlen(s4)-4]=NULL;
							FindTextureExtension(s4);
							pShaderPass->AddFrameTexture(1,s4);
						}
						if (iNumOfFrames>2)
						{
							s5[strlen(s5)-4]=NULL;
							FindTextureExtension(s5);
							pShaderPass->AddFrameTexture(2,s5);
						}
						if (iNumOfFrames>3)
						{
							s6[strlen(s6)-4]=NULL;
							FindTextureExtension(s6);
							pShaderPass->AddFrameTexture(3,s6);
						}
						if (iNumOfFrames>4)
						{
							s7[strlen(s7)-4]=NULL;
							FindTextureExtension(s7);
							pShaderPass->AddFrameTexture(4,s7);
						}
						if (iNumOfFrames>5)
						{
							s8[strlen(s8)-4]=NULL;
							FindTextureExtension(s8);
							pShaderPass->AddFrameTexture(5,s8);
						}
						if (iNumOfFrames>6)
						{
							s9[strlen(s9)-4]=NULL;
							FindTextureExtension(s9);
							pShaderPass->AddFrameTexture(6,s9);
						}
						if (iNumOfFrames>7)
						{
							s10[strlen(s10)-4]=NULL;
							FindTextureExtension(s10);
							pShaderPass->AddFrameTexture(7,s10);
						}
					}

					//depthfunc

					if (strcmp(s1,"depthfunc")==0)
					{
						if (strcmp(s2,"equal")==0)
							pShaderPass->SetDEPTHFunc(DEPTHFUNC_EQUAL);
						else
						if (strcmp(s2,"lequal")==0)
							pShaderPass->SetDEPTHFunc(DEPTHFUNC_LEQUAL);
					}

					//SetALPHAFunc


					// alphafunc

					if (strcmp(s1,"alphafunc")==0)
					{
						//BREAKPOINT(1==1);
						if (strcmp(s2,"gt0")==0)
						{
							pShaderPass->SetALPHAFunc(ALPHAFUNC_GT0);
						}
						else
						if (strcmp(s2,"lt128")==0)
						{
							pShaderPass->SetALPHAFunc(ALPHAFUNC_LT128);
						}
						else
						if (strcmp(s2,"ge128")==0)
						{
							pShaderPass->SetALPHAFunc(ALPHAFUNC_GE128);
						}
					}


					// blendfunc

					if (strcmp(s1,"blendfunc")==0)
					{
						if (strcmp(s2,"add")==0)
							pShaderPass->SetBlendValues(GL_ONE,GL_ONE);
						else
						if (strcmp(s2,"filter")==0)
							pShaderPass->SetBlendValues(GL_DST_COLOR,GL_ZERO);
						else
						if (strcmp(s2,"blend")==0)
							pShaderPass->SetBlendValues(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
						else
						{
							if (strcmp(s2,"gl_one")==0)
								t_dwSrcBlend=GL_ONE;
							else
							if (strcmp(s2,"gl_zero")==0)
								t_dwSrcBlend=GL_ZERO;
							else
							if (strcmp(s2,"gl_dst_color")==0)
								t_dwSrcBlend=GL_DST_COLOR;
							else
							if (strcmp(s2,"gl_src_color")==0)
								t_dwSrcBlend=GL_SRC_COLOR;
							else
							if (strcmp(s2,"gl_one_minus_dst_color")==0)
								t_dwSrcBlend=GL_ONE_MINUS_DST_COLOR;
							else
							if (strcmp(s2,"gl_one_minus_src_color")==0)
								t_dwSrcBlend=GL_ONE_MINUS_SRC_COLOR;
							else
							if (strcmp(s2,"gl_src_alpha")==0)
								t_dwSrcBlend=GL_SRC_ALPHA;
							else
							if (strcmp(s2,"gl_one_minus_src_alpha")==0)
								t_dwSrcBlend=GL_ONE_MINUS_SRC_ALPHA;

							if (strcmp(s3,"gl_one")==0)
								t_dwDstBlend=GL_ONE;
							else
							if (strcmp(s3,"gl_zero")==0)
								t_dwDstBlend=GL_ZERO;
							else
							if (strcmp(s3,"gl_dst_color")==0)
								t_dwDstBlend=GL_DST_COLOR;
							else
							if (strcmp(s3,"gl_src_color")==0)
								t_dwDstBlend=GL_SRC_COLOR;
							else
							if (strcmp(s3,"gl_one_minus_dst_color")==0)
								t_dwDstBlend=GL_ONE_MINUS_DST_COLOR;
							else
							if (strcmp(s3,"gl_one_minus_src_color")==0)
								t_dwDstBlend=GL_ONE_MINUS_SRC_COLOR;
							else
							if (strcmp(s3,"gl_src_alpha")==0)
								t_dwDstBlend=GL_SRC_ALPHA;
							else
							if (strcmp(s3,"gl_one_minus_src_alpha")==0)
								t_dwDstBlend=GL_ONE_MINUS_SRC_ALPHA;		

							pShaderPass->SetBlendValues(t_dwSrcBlend,t_dwDstBlend);
						}
					}

					if (strcmp(s1,"depthwrite")==0)
					{
						bForceDepthWrite=true;
						t_dwShaderFlags|=FLAG_DEPTHWRITE;
						//t_dwShaderFlags|=SPF_DEPTHWRITE;
					}

					if (strcmp(s1,"tcmod")==0)
					{
						if (strcmp(s2,"scroll")==0)
						{
							pShaderPass->AddTCModScroll(-atof(s3),-atof(s4));
						}
						else
						if (strcmp(s2,"turb")==0)
						{
							pShaderPass->AddTCModTurb(atof(s3),atof(s4),atof(s5),atof(s6));
						}
						else
						if (strcmp(s2,"rotate")==0)
						{
							pShaderPass->AddTCModRotate(atof(s3));
						}
						else
						if (strcmp(s2,"scale")==0)
						{
							pShaderPass->AddTCModScale(atof(s3),atof(s4));
						}
						else
						if (strcmp(s2,"stretch")==0)
						{
							if (strcmp(s3,"sin")==0)
								t_uiWaveType = WF_SIN;
							else
							if (strcmp(s3,"triangle")==0)
								t_uiWaveType = WF_TRIANGLE;
							else
							if (strcmp(s3,"square")==0)
								t_uiWaveType = WF_SQUARE;
							else
							if (strcmp(s3,"sawtooth")==0)
								t_uiWaveType = WF_SAWTOOTH;
							else
							if (strcmp(s3,"inversesawtooth")==0)
								t_uiWaveType = WF_INVERSESAWTOOTH;
							//else
							//if (strcmp(s3,"noise")==0)
							//t_uiWaveType = WF_NOISE;
							pShaderPass->AddTCModStretch(t_uiWaveType,atof(s4),atof(s5),atof(s6),atof(s7));
						}
						else
						if (strcmp(s2,"transform")==0)
						{
							pShaderPass->AddTCModTransform(atof(s3),atof(s4),atof(s5),atof(s6),atof(s7),atof(s8));
						}
						
					}

					if (strcmp(s1,"tcgen")==0)
					{
						if (strcmp(s2,"base")==0)
						{
							pShaderPass->SetTCGen(TCGEN_BASE);
						}
						if (strcmp(s2,"lightmap")==0)
						{
							pShaderPass->SetTCGen(TCGEN_LIGHTMAP);
						}
						else
						if (strcmp(s2,"environment")==0)
						{
							pShaderPass->SetTCGen(TCGEN_ENVIRONMENT);
						}
						else
						if (strcmp(s2,"vector")==0)
						{
							pShaderPass->SetTCGen(TCGEN_VECTOR);
						}
						else
						if (strcmp(s2,"lightingdiffuse")==0)
						{
							pShaderPass->SetTCGen(TCGEN_LIGHTINGDIFFUSE);
						}
					}

					if (strcmp(s1,"rgbgen")==0)
					{

						t_f1=0.f;
						t_f2=0.f;
						t_f3=0.f;
						t_f4=0.f;

						if (strcmp(s2,"identity")==0)
							pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
						else
						if (strcmp(s2,"identityLighting")==0)
							pShaderPass->SetRGBGen(RGBGEN_IDENTITYLIGHTING);
						else
						if (strcmp(s2,"vertex")==0)
							pShaderPass->SetRGBGen(RGBGEN_VERTEX);
						else
						if (strcmp(s2,"exactvertex")==0)
							pShaderPass->SetRGBGen(RGBGEN_EXACTVERTEX);
						else
						if (strcmp(s2,"oneminusvertex")==0)
							pShaderPass->SetRGBGen(RGBGEN_ONEMINUSVERTEX);
						else 
						if (strcmp(s2,"wave")==0)
						{
							//pShader->SetShaderFlags(FLAG_NOCULL);

							pShaderPass->SetRGBGen(RGBGEN_WAVE);

							if (strcmp(s3,"sin")==0)
								t_uiWaveType = WF_SIN;
							else
							if (strcmp(s3,"triangle")==0)
								t_uiWaveType = WF_TRIANGLE;
							else
							if (strcmp(s3,"square")==0)
								t_uiWaveType = WF_SQUARE;
							else
							if (strcmp(s3,"sawtooth")==0)
								t_uiWaveType = WF_SAWTOOTH;
							else
							if (strcmp(s3,"inversesawtooth")==0)
								t_uiWaveType = WF_INVERSESAWTOOTH;
							//else
							//if (strcmp(s3,"noise")==0)
								//t_uiWaveType = WF_NOISE;

							t_f1=atof(s4);
							t_f2=atof(s5);
							t_f3=atof(s6);
							t_f4=atof(s7);
                   
							pShaderPass->SetRGBGenWaveValues(t_uiWaveType,t_f1,t_f2,t_f3,t_f4);						
						}
						else 
						if (strcmp(s2,"const")==0)
						{
							pShaderPass->SetRGBGen(RGBGEN_CONST);
							t_uiWaveType = WF_NONE;

							t_f1=atof(s4);
							t_f2=atof(s5);
							t_f3=atof(s6);
							
							pShaderPass->SetRGBGenWaveValues(t_uiWaveType,t_f1,t_f2,t_f3,0.f);

						}

					}

					if (strcmp(s1,"alphagen")==0)
					{

						t_f1=0.f;
						t_f2=0.f;
						t_f3=0.f;
						t_f4=0.f;

						if (strcmp(s2,"identity")==0)
							pShaderPass->SetALPHAGen(ALPHAGEN_IDENTITY);
						else
						if (strcmp(s2,"identityLighting")==0)
							pShaderPass->SetALPHAGen(ALPHAGEN_IDENTITYLIGHTING);
						else
						if (strcmp(s2,"vertex")==0)
							pShaderPass->SetALPHAGen(ALPHAGEN_VERTEX);
						else
						if (strcmp(s2,"oneminusvertex")==0)
							pShaderPass->SetALPHAGen(ALPHAGEN_ONEMINUSVERTEX);
						else 
						if (strcmp(s2,"wave")==0)
							{
							pShaderPass->SetALPHAGen(ALPHAGEN_WAVE);

							if (strcmp(s3,"sin")==0)
								t_uiWaveType = WF_SIN;
							else
							if (strcmp(s3,"triangle")==0)
								t_uiWaveType = WF_TRIANGLE;
							else
								if (strcmp(s3,"square")==0)
							t_uiWaveType = WF_SQUARE;
							else
								if (strcmp(s3,"sawtooth")==0)
								t_uiWaveType = WF_SAWTOOTH;
							else
							if (strcmp(s3,"inversesawtooth")==0)
							t_uiWaveType = WF_INVERSESAWTOOTH;
							//else
							//if (strcmp(s3,"noise")==0)
							//t_uiWaveType = WF_NOISE;

							t_f1=atof(s4);
							t_f2=atof(s5);
							t_f3=atof(s6);
							t_f4=atof(s7);

							pShaderPass->SetALPHAGenWaveValues(t_uiWaveType,t_f1,t_f2,t_f3,t_f4);
             			}
					}

								

						s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				}
			}

			} // end of "if (bNoShaderPasses==false)


			//BREAKPOINT(strcmp(ShaderName,"textures/sfx/computer_blocks17")==0);
			
			t_ScriptFile.close();

			bool bOpaque=false;
			bool t_bALPHAFunc=false;

			DWORD t_dwSort = pShader->GetShaderSort();

			
			{

			
				for (int i=0;i<pShader->GetPassCount();i++)
				{
					if (pShader->GetPass(i)->IsBlended()!=true)
					{
						bOpaque=true;						
					}

					if (pShader->GetPass(i)->GetALPHAFunc()!=ALPHAFUNC_NONE)
					{
						t_bALPHAFunc=true;
					}
				}

				pShader->SetAllBlended(!bOpaque);


				if (bOpaque==true)
					{
						if (t_dwSort == SORT_NOT_INIT)
							pShader->SetShaderSort(SORT_OPAQUE);
						t_dwShaderFlags|=FLAG_DEPTHWRITE;
					}
				else
				{
					if (t_dwSort == SORT_NOT_INIT)
					{
						pShader->SetShaderSort(SORT_ADDITIVE);
						t_dwShaderFlags&=~FLAG_DEPTHWRITE;
					}					
				}

				
				if (t_bALPHAFunc==true)
				{
					DWORD t_Sort = pShader->GetShaderSort();
					t_Sort+=SORT_ONE;
					pShader->SetShaderSort(t_Sort);
				}

				if (bForceDepthWrite==true)
					t_dwShaderFlags|=FLAG_DEPTHWRITE;

			}
			

			pShader->SetShaderFlags(t_dwShaderFlags);
			pShader->SetSurfaceParm(t_dwSurfaceParm);



			return pShader;

	}
	else                       // this is a simple texture
		{
			char* ShaderNamePlusExt = new char[strlen(ShaderName)+5]; // .BLA+NULL = 5
			strcpy(ShaderNamePlusExt,ShaderName);

			FindTextureExtension(ShaderNamePlusExt);

			// If no shader with this name was found in our manager, and also none was found in the cache
			// this is the default shader, Regular Texture+Lightmap

			pShader = new CQ3A_Shader;

			pShader->SetScripted(false);

			/////////////////////////
			//gex_iDebugSPNewSHADERCalls++;
			/////////////////////////

			pShader->SetName(ShaderName);
			pShader->SetShaderSort(SORT_OPAQUE);
			pShader->SetShaderFlags(FLAG_CULLFRONT | FLAG_DEPTHWRITE);
 
			pShaderPass = new CQ3A_ShaderPass;

			pShaderPass->AddNoAnimTexture(ShaderNamePlusExt,true,false); 
			pShaderPass->SetBlendValues(GL_ONE,GL_ZERO);
			pShaderPass->SetRGBGen(RGBGEN_IDENTITY);// THIS IS CORRECT
			pShaderPass->SetShaderPassFlags(SPF_DEPTHWRITE);

			pShader->AddPass(pShaderPass);

			pShaderPass = new CQ3A_ShaderPass;

			pShaderPass->SetAlreadyAllocatedNoAnimTexture(0); // not needed
			pShaderPass->SetBlendValues(GL_DST_COLOR,GL_ZERO);// modulate
			pShaderPass->SetRGBGen(RGBGEN_IDENTITY);
			pShaderPass->SetShaderPassFlags(SPF_ISLIGHTMAP);

			pShader->AddPass(pShaderPass);

			delete [] ShaderNamePlusExt;

			return pShader;

		}
	
}

