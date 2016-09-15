// ceEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "resource.h"

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <sys/stat.h> 
#include <direct.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include "glext_decl.inl"

// user options
#include "ce.h"
#include "EngineOptions.h"

int  g_iApiUsed = CR_API_GL;

CEngineOptions* g_pEngineOptions = NULL;

DWORD g_dwForcedCullingMethod = CE_DONT_FORCE_CULLING_METHOD;

string GetCurrentPath() 
{
	char cCurrentPath[FILENAME_MAX];

	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		return "";
	}

	string ans = cCurrentPath;
	return ans;
}

struct tEngineExports
{
	 class ILogFile*                pLogFile;
	 class IGraphicsAPI_Export*     pGraphicsAPI;
	 class IRendererBackend_Export* pRendererBack;
	 class CShaderUniforms*         pAutoUniforms; 
	 class ITimer*                  pTimer;
	 class IProfiler*               pProfiler;
	 IEngineOptions*                pEngineOptions; 
};


tEngineExports g_Exports;


#include "LogFile.h"
ILogFile* g_pLogFile;

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "d3d9.h"
#include "d3dx9.h"

#include "RendererBackend.h"
CRendererBackend* g_pRendererBack;

LPDIRECT3DDEVICE9			g_pDirect3DDevice;

//#include "RendererBackend_GL.h"
//#include "RendererBackend_D3D.h"




#include "CTimer.h"
ITimer*  g_pTimer;
//CTimer*  g_pTimer;

#include "Profiler.h"
IProfiler* g_pProfiler;


CGcontext g_cgContext;
CGprofile g_VertexProfile,g_FragmentProfile;

#include "ShaderUniforms.h"
CShaderUniforms* g_pAutoUniforms;


#include "GraphicsAPI_GL.h"
#include "GraphicsAPI_D3D.h"


IGraphicsAPI* g_pGraphicsAPI;



#include "TextureManager.h"
#include "ShaderProgramManager.h"
#include "MaterialManager.h"




void handleCgError()
{ 
	
	const char *pchListing = cgGetLastListing(g_cgContext);  
	//CLog::Inst()->Error("CCGWrapper::ErrorCallback %s",cgGetErrorString(cgLastError));  
	//CLog::Inst()->Error("CCGWrapper::ErrorCallback %s",pchListing);
	
	g_pLogFile->OutPutPlainText(pchListing,"3","990000",true,LOG_MESSAGE_ERROR);

	static char buffer[1024];
	sprintf(buffer, "Cg error: %s",cgGetErrorString(cgGetError()));
	g_pLogFile->OutPutPlainText(buffer,"3","990000",true,LOG_MESSAGE_ERROR);
	//g_LogFile.OutPutPlainText(":","3","000000",false);
	sprintf(buffer, "Cg error: %s",cgGetLastListing(g_cgContext));
	//g_LogFile.OutPutPlainText(buffer,"3","000099",true);
}




// StartUp Dialog
LRESULT CALLBACK StartupDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	int iRes;

	static int modes[1000][3];

	/*char* mapNames[] = 
	{
		"Maps/q3dm17.bsp",
		"Maps/q3dm1.bsp",
		"Maps/q3dm7.bsp",
		"Maps/q3tourney2.bsp",
	};*/

	switch (message)
	{
		case WM_INITDIALOG:
			{
		
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_SETCHECK ,BST_CHECKED,0);			
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_SETCHECK ,BST_UNCHECKED,0);

			SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_SETCHECK ,BST_UNCHECKED,0);			
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_SETCHECK ,BST_CHECKED,0);

			SendMessage(GetDlgItem(hDlg,IDC_CHECK_WINDOWED),BM_SETCHECK ,BST_CHECKED,0);

			// find map files in directory \maps

			 char desOfile[80]; //destination of file, load to a LPDIRECT3DTEXTURE9
			 WIN32_FIND_DATA findFileData;
			 HANDLE hFind = FindFirstFile(".\\maps\\*.*", &findFileData);
			 FindNextFile(hFind,&findFileData);
			
			 char relativePathName[300];
			 
			 while (FindNextFile(hFind,&findFileData)!=0)
			 {	 
				 if (strstr(findFileData.cFileName,".bsp")==NULL)
					 continue;
				 assert(strlen(findFileData.cFileName)<270);

				 sprintf(relativePathName,"maps\\%s",findFileData.cFileName);

				 SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_ADDSTRING ,0,(LPARAM)relativePathName);
			 } 

			/*SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_ADDSTRING ,0,(LPARAM)mapNames[0]);
			SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_ADDSTRING ,0,(LPARAM)mapNames[1]);
			SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_ADDSTRING ,0,(LPARAM)mapNames[2]);
			SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_ADDSTRING ,0,(LPARAM)mapNames[3]);*/

			SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_SETCURSEL ,(WPARAM)0,0);

			char default_map_str[256];
			string ini_file_path = GetCurrentPath()+string("\\ceConfig.ini");
			DWORD dwSize = GetPrivateProfileString("CONNECTION_SETTINGS","DEFAULT_GAME_MAP",NULL,default_map_str,255,ini_file_path.c_str());

			if (dwSize>0)
			{
				LRESULT res = SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_FINDSTRING ,(WPARAM)-1,(LPARAM)&default_map_str[0]);

				if (CB_ERR!=res)
				{
					SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_SETCURSEL ,(WPARAM)res,0);
				}		
			}

			//SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETITEMHEIGHT, 0, (LPARAM) 32);

			/*BOOL EnumDisplaySettings(
				LPCTSTR lpszDeviceName,  // display device
				DWORD iModeNum,          // graphics mode
				LPDEVMODE lpDevMode      // graphics mode settings
				);*/

			{				
				DEVMODE dm;
				int iMode = 0;

				char temp[0xFF];		

				int iBestMode = 0;
				int iBestWidth = 0;
				int iBestHeight = 0;
				int iBestFreq = 0;


				while (EnumDisplaySettings(0,iMode,&dm) > 0)
				{
					modes[iMode][0] = dm.dmPelsWidth;
					modes[iMode][1] = dm.dmPelsHeight;
					modes[iMode][2] = dm.dmDisplayFrequency;

					sprintf(temp,"%d x %d %dhz",modes[iMode][0],modes[iMode][1],modes[iMode][2]);
					SendMessage(GetDlgItem(hDlg,IDC_COMBO1),CB_ADDSTRING ,0,(LPARAM)temp);

					if (iBestWidth <= dm.dmPelsWidth)
					{
						iBestMode = iMode; 
						iBestWidth = dm.dmPelsWidth; 
						iBestHeight = dm.dmPelsHeight; 
						iBestFreq = dm.dmDisplayFrequency;
					}

					/*if (iBestFreq < dm.dmDisplayFrequency)
					{
						iBestMode = iMode; iBestWidth = dm.dmPelsWidth; iBestHeight = dm.dmPelsHeight; iBestFreq = dm.dmDisplayFrequency;
					}*/


					iMode++;
				}

				SendMessage(GetDlgItem(hDlg,IDC_COMBO1),CB_SETCURSEL ,(WPARAM)iBestMode,0);

				int iRes = SendMessage(GetDlgItem(hDlg,IDC_COMBO1),CB_GETCURSEL ,0,0);

				iRes++;
				iRes--;

				// add - default select primary native mode
			}


			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_SETCHECK ,BST_CHECKED,0);			
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_SETCHECK ,BST_UNCHECKED,0);			
			
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_DOF),BM_SETCHECK ,BST_UNCHECKED,0);
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_DOF),BM_SETCarghHECK ,BST_CHECKED,0);


			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_SETCHECK ,BST_CHECKED,0);

			SendMessage(GetDlgItem(hDlg,IDC_FULL_FALLBACK_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			//SendMessage(GetDlgItem(hDlg,IDC_FULL_FALLBACK_BOX),BM_SETCHECK ,BST_CHECKED,0);

			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR0_BOX),BM_SETCHECK ,BST_CHECKED,0);			
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR0_BOX),BM_SETCHECK ,BST_UNCHECKED,0);			
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR1_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR2_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			//SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR3_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			
		}
		break;

		case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		//assert(_CrtCheckMemory());
		// Parse the menu selections:
		switch (wmId)
		{

		/*case IDC_CHECK_REND_CHAR0_BOX:
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR1_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR2_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR3_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
		break;

		case IDC_CHECK_REND_CHAR1_BOX:
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR0_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR2_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR3_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
		break;

		case IDC_CHECK_REND_CHAR2_BOX:
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR1_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR0_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR3_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
		break;

		case IDC_CHECK_REND_CHAR3_BOX:
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR1_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR2_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_CHAR0_BOX),BM_SETCHECK ,BST_UNCHECKED,0);
		break;*/


			case IDC_CHECK_OPENGL:

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_GETCHECK ,0,0);

				if (iRes==BST_CHECKED)
					SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_SETCHECK ,BST_UNCHECKED,0);
				else
					SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_SETCHECK ,BST_CHECKED,0);


				break;

			case IDC_CHECK_D3D:

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_GETCHECK ,0,0);

				if (iRes==BST_CHECKED)
					SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_SETCHECK ,BST_UNCHECKED,0);
				else
					SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_SETCHECK ,BST_CHECKED,0);

				//assert(_CrtCheckMemory());

				break;


			case IDOK:
				{

				//assert(_CrtCheckMemory());

				
				iRes = SendMessage(GetDlgItem(hDlg,IDC_COMBO1),CB_GETCURSEL ,0,0);
				g_pEngineOptions->uiScreenWidth = modes[iRes][0];
				g_pEngineOptions->uiScreenHeight = modes[iRes][1];
				g_pEngineOptions->uiScreenFrequency = modes[iRes][2];

				//assert(_CrtCheckMemory());


				///////////////////////////////////////////////////////////////////////////////////
				// Read selected map

				iRes = SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_GETCURSEL ,0,0);

				int iLength = SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_GETLBTEXTLEN  ,(WPARAM)iRes,0);

				g_pEngineOptions->m_pcDesiredLevel = new char[ iLength+1];
				
				//assert(_CrtCheckMemory());
				iRes = SendMessage(GetDlgItem(hDlg,IDC_COMBO2),CB_GETLBTEXT ,(WPARAM)iRes,(LPARAM)g_pEngineOptions->m_pcDesiredLevel);								
				//assert(_CrtCheckMemory());	


				//update config file

				string ini_file_path = GetCurrentPath()+string("\\ceConfig.ini");
				WritePrivateProfileString("CONNECTION_SETTINGS","DEFAULT_GAME_MAP",g_pEngineOptions->m_pcDesiredLevel,ini_file_path.c_str());

				/////////////////////////////////////////////////////////////////////////////////////////

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_OPENGL),BM_GETCHECK ,0,0);
				if (iRes==BST_CHECKED)
					g_iApiUsed = CR_API_GL;

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_D3D),BM_GETCHECK ,0,0);
				if (iRes==BST_CHECKED)
					g_iApiUsed = CR_API_D3D;

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_WINDOWED),BM_GETCHECK ,0,0);
				if (iRes==BST_CHECKED)
					g_pEngineOptions->bWindowed = true;
				else
					g_pEngineOptions->bWindowed = false;

				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_DOF),BM_GETCHECK ,0,0);
				if (iRes==BST_CHECKED)
					g_pEngineOptions->bDepthOfField = true;
				else
					g_pEngineOptions->bDepthOfField = false;


				iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_GETCHECK ,0,0);
				
				if (iRes==BST_CHECKED)
					g_pEngineOptions->bRenderRTTBox = true;
				else
					g_pEngineOptions->bRenderRTTBox = false;


				iRes = SendMessage(GetDlgItem(hDlg,IDC_FULL_FALLBACK_BOX),BM_GETCHECK ,0,0);
				
				if (iRes==BST_CHECKED)
					g_pEngineOptions->bFullFallback = true;
				else
					g_pEngineOptions->bFullFallback = false;
				

				//assert(_CrtCheckMemory());
				

				/*iRes = SendMessage(GetDlgItem(hDlg,IDC_CHECK_REND_RTT_BOX),BM_GETCHECK ,0,0);
				if (iRes==BST_CHECKED)
					g_pEngineOptions->bRenderRTTBox = true;
				else
					g_pEngineOptions->bRenderRTTBox = false;*/


				
				


				EndDialog(hDlg, LOWORD(wParam));

				//assert(_CrtCheckMemory());

				return TRUE;
				}
			break;

		}
		break;
	}

	//assert(_CrtCheckMemory());
	return FALSE;
}


BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	
	//MessageBox(HWND_DESKTOP, "blah!!!", "Error", MB_OK);

	//DialogBox(hInstance,(LPCTSTR)IDD_STARTUP_DIALOG,0,(DLGPROC)StartupDialog);
	if (ul_reason_for_call==DLL_PROCESS_ATTACH)
	{
		g_pEngineOptions = new CEngineOptions();
		DialogBox((HINSTANCE)hModule,(LPCTSTR)IDD_STARTUP_DIALOG,0,(DLGPROC)StartupDialog);
	}


	

    return TRUE;
}

void EngineShutdown()
{
	CMaterialManager::Destroy();
	CShaderProgramManager::Destroy();
	CTextureManager::Destroy();

	
	SAFE_RELEASE(g_pDirect3DDevice);

	SafeDelete(g_pGraphicsAPI);

	SafeDelete(g_pRendererBack);

	SafeDelete(g_pAutoUniforms);

}

tEngineExports* GetEngineExports()
{

	g_Exports.pEngineOptions = g_pEngineOptions;

	


	g_pLogFile = new CLogFile();


	g_pAutoUniforms = new CShaderUniforms();


	// debugging

	// tweakable0
	tUniform* tweakable0 = new tUniform(CR_FLOAT1);
	char * pchName = new char[strlen("tweakable0")+1];
	sprintf(pchName,"tweakable0");
	tweakable0->m_pchName = pchName;
	tweakable0->m_pfValue[0] = 0.0000;
	g_pAutoUniforms->Add(pchName,tweakable0);

	// tweakable1
	tUniform* tweakable1 = new tUniform(CR_FLOAT1);
	pchName = new char[strlen("tweakable1")+1];
	sprintf(pchName,"tweakable1");
	tweakable1->m_pchName = pchName;
	tweakable1->m_pfValue[0] = 0.0000;
	g_pAutoUniforms->Add(pchName,tweakable1);

	



	///////////////////////////////////////////////////////////////////////
	// MATRICES


	////////////////////////////////////////////////////////////////////
	// worldViewProjection
	tUniform* lightViewProjMat = new tUniform(CR_FLOAT4X4);
	lightViewProjMat->m_pfValue = NULL;
	pchName = new char[strlen("lightViewProjMat")+1];
	sprintf(pchName,"lightViewProjMat");
	lightViewProjMat->m_pchName = pchName;
	lightViewProjMat->m_pfValue = new float[16];
	g_pAutoUniforms->Add(pchName,lightViewProjMat);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// worldViewProjection
	tUniform* worldViewProjection = new tUniform(CR_FLOAT4X4);
	worldViewProjection->m_pfValue = NULL;
	pchName = new char[strlen("worldViewProjection")+1];
	sprintf(pchName,"worldViewProjection");
	worldViewProjection->m_pchName = pchName;
	worldViewProjection->m_pfValue = new float[16];
	g_pAutoUniforms->Add(pchName,worldViewProjection);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// world
	tUniform* world = new tUniform(CR_FLOAT4X4);
	world->m_pfValue = NULL;
	pchName = new char[strlen("world")+1];
	sprintf(pchName,"world");
	world->m_pchName = pchName;
	world->m_pfValue = new float[16];
	g_pAutoUniforms->Add(pchName,world);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// view
	tUniform* view = new tUniform(CR_FLOAT4X4);
	view->m_pfValue = NULL;
	pchName = new char[strlen("view")+1];
	sprintf(pchName,"view");
	view->m_pchName = pchName;
	view->m_pfValue = new float[16];
	g_pAutoUniforms->Add(pchName,view);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// projection
	tUniform* proj = new tUniform(CR_FLOAT4X4);
	proj->m_pfValue = NULL;
	pchName = new char[strlen("projection")+1];
	sprintf(pchName,"projection");
	proj->m_pchName = pchName;
	proj->m_pfValue = new float[16];
	g_pAutoUniforms->Add(pchName,proj);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// viewProjection
	tUniform* viewProjection = new tUniform(CR_FLOAT4X4);
	viewProjection->m_pfValue = NULL;
	pchName = new char[strlen("viewProjection")+1];
	sprintf(pchName,"viewProjection");
	viewProjection->m_pchName = pchName;
	viewProjection->m_pfValue  = new float[16];
	g_pAutoUniforms->Add(pchName,viewProjection);
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// worldView
	tUniform* worldView = new tUniform(CR_FLOAT4X4);
	worldView->m_pfValue = NULL;
	pchName = new char[strlen("worldView")+1];
	sprintf(pchName,"worldView");
	worldView->m_pchName = pchName;
	worldView->m_pfValue  = new float[16];
	g_pAutoUniforms->Add(pchName,worldView);
	////////////////////////////////////////////////////////////////////

	// end of MATRICES
	///////////////////////////////////////////////////////////////////////
	

	
	tUniform* time = new tUniform(CR_FLOAT1);
	time->m_pfValue[0] = 0.f;	

	pchName = new char[strlen("time")+1];
	sprintf(pchName,"time");
	time->m_pchName = pchName;

	g_pAutoUniforms->Add(pchName,time);



	// ambient color

	tUniform* ambientColor = new tUniform(CR_FLOAT3);
	
	/*ambientColor->m_pfValue[0] = 0.8f;	
	ambientColor->m_pfValue[1] = 0.8f;	
	ambientColor->m_pfValue[2] = 0.8f;	*/

	ambientColor->m_pfValue[0] = 0.3f;	
	ambientColor->m_pfValue[1] = 0.3f;	
	ambientColor->m_pfValue[2] = 0.3f;

	pchName = new char[strlen("ambientColor")+1];
	sprintf(pchName,"ambientColor");
	ambientColor->m_pchName = pchName;

	g_pAutoUniforms->Add(pchName,ambientColor);


	////////////////////////////// camera position

	tUniform* cameraPos = new tUniform(CR_FLOAT3);
	cameraPos->m_pfValue[0] = 0.f;	
	cameraPos->m_pfValue[1] = 0.f;	
	cameraPos->m_pfValue[2] = 0.f;	

	pchName = new char[strlen("cameraPos")+1];
	sprintf(pchName,"cameraPos");
	cameraPos->m_pchName = pchName;

	g_pAutoUniforms->Add("cameraPos",cameraPos);
	//////////////////////////

	/////////////////////////// object space camera position
	tUniform* objCameraPos = new tUniform(CR_FLOAT3);
	objCameraPos->m_pfValue[0] = 0.f;	
	objCameraPos->m_pfValue[1] = 0.f;	
	objCameraPos->m_pfValue[2] = 0.f;	

	pchName = new char[strlen("objCameraPos")+1];
	sprintf(pchName,"objCameraPos");
	objCameraPos->m_pchName = pchName;

	g_pAutoUniforms->Add("objCameraPos",objCameraPos);
	

	/////////////////////////////// light pos

	tUniform* lightPos = new tUniform(CR_FLOAT3);
	lightPos->m_pfValue[0] = 0.f;	
	lightPos->m_pfValue[1] = 0.f;	
	lightPos->m_pfValue[2] = 0.f;	

	pchName = new char[strlen("lightPos")+1];
	sprintf(pchName,"lightPos");
	lightPos->m_pchName = pchName;

	g_pAutoUniforms->Add("lightPos",lightPos);

		
	////////////////////////// object space light pos

	tUniform* objLightPos = new tUniform(CR_FLOAT3);
	objLightPos->m_pfValue[0] = 0.f;	
	objLightPos->m_pfValue[1] = 0.f;	
	objLightPos->m_pfValue[2] = 0.f;	

	pchName = new char[strlen("objLightPos")+1];
	sprintf(pchName,"objLightPos");
	objLightPos->m_pchName = pchName;

	g_pAutoUniforms->Add("objLightPos",objLightPos);

	/////////////////////////// light color

	tUniform* lightCol = new tUniform(CR_FLOAT3);
	lightCol->m_pfValue[0] = 1.f;	
	lightCol->m_pfValue[1] = 1.f;	
	lightCol->m_pfValue[2] = 1.f;	

	pchName = new char[strlen("lightCol")+1];
	sprintf(pchName,"lightCol");
	lightCol->m_pchName = pchName;

	g_pAutoUniforms->Add("lightCol",lightCol);

	////////////////////////// light radius
	tUniform* lightRadius = new tUniform(CR_FLOAT1);
	lightRadius->m_pfValue[0] = 400.f;	
	
	pchName = new char[strlen("lightRadius")+1];
	sprintf(pchName,"lightRadius");
	lightRadius->m_pchName = pchName;

	g_pAutoUniforms->Add("lightRadius",lightRadius);


	////////////////////////// light look at
	tUniform* lightLookAt = new tUniform(CR_FLOAT3);
	lightLookAt->m_pfValue[0] = 0.f;	
	lightLookAt->m_pfValue[0] = 0.f;	
	lightLookAt->m_pfValue[0] = -1.f;	
	
	pchName = new char[strlen("lightLookAt")+1];
	sprintf(pchName,"lightLookAt");
	lightLookAt->m_pchName = pchName;

	g_pAutoUniforms->Add("lightLookAt",lightLookAt);

	////////////////////////// object space light look at
	tUniform* objLightLookAt = new tUniform(CR_FLOAT3);
	objLightLookAt->m_pfValue[0] = 0.f;	
	objLightLookAt->m_pfValue[0] = 0.f;	
	objLightLookAt->m_pfValue[0] = -1.f;	
	
	pchName = new char[strlen("objLightLookAt")+1];
	sprintf(pchName,"objLightLookAt");
	objLightLookAt->m_pchName = pchName;

	g_pAutoUniforms->Add("objLightLookAt",objLightLookAt);


	



	g_pLogFile->OutPutPlainText("////CEngine::Init::Adding AutoUniforms Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Timer...",LOG_MESSAGE_INFORMATIVE);

	CTimer* pTimer = new CTimer();

	//pTimer->TimerInit();

	g_pTimer =  pTimer;
	g_pTimer->TimerInit();
	g_Exports.pTimer =g_pTimer;
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Timer Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);

	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Profiler...",LOG_MESSAGE_INFORMATIVE);
	g_pProfiler = new CProfiler();
	g_Exports.pProfiler = g_pProfiler;
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Profiler Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);

	//CTimer::CreateInstance();
	


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing API:",LOG_MESSAGE_INFORMATIVE);

	if (g_iApiUsed == CR_API_GL)
	{
		g_pGraphicsAPI = new CGraphicsAPI_GL();
		g_pLogFile->OutPutPlainText("CEngine::Init::Graphics API Used: OpenGL",LOG_MESSAGE_INFORMATIVE);
	
		//g_pRendererBack = new CRendererBackend_GL();
	}
	else if (g_iApiUsed == CR_API_D3D)
	{
	    g_pGraphicsAPI = new CGraphicsAPI_D3D();
		g_pLogFile->OutPutPlainText("CEngine::Init::Graphics API Used: Direct3D",LOG_MESSAGE_INFORMATIVE);
	
		//g_pRendererBack = new CRendererBackend_D3D();
	}
	else
	{
		g_pLogFile->OutPutPlainText("CEngine::Init::No Graphcis API was chosen! Quitting.",LOG_MESSAGE_ERROR);
		return false;
	}


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Renderer Backend:",LOG_MESSAGE_INFORMATIVE);
	g_pRendererBack = new CRendererBackend();
	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Renderer done.:",LOG_MESSAGE_INFORMATIVE);




	g_pLogFile->OutPutPlainText("////CEngine::Init::Initing API Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


		
	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Texture Manager...",LOG_MESSAGE_INFORMATIVE);
	CTextureManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Texture Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);
	

	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Shader Program Manager...",LOG_MESSAGE_INFORMATIVE);
	CShaderProgramManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Shader Program ManagerFinished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);


	g_pLogFile->OutPutPlainText("CEngine::Init::Initing Material Manager...",LOG_MESSAGE_INFORMATIVE);
	CMaterialManager::CreateInstance();
	g_pLogFile->OutPutPlainText("////CEngine::Init:: Inititialization of Material Manager Finished.","4","009900",true,LOG_MESSAGE_INFORMATIVE);
	
	
	g_Exports.pLogFile = g_pLogFile;

	g_Exports.pGraphicsAPI = g_pGraphicsAPI;

	g_Exports.pRendererBack = g_pRendererBack;

	g_Exports.pAutoUniforms = g_pAutoUniforms;



		


	return &g_Exports;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

