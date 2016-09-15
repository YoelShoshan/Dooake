#include "stdafx.h"
#include "q3amap.h"


#include "Singleton.h"

#include "EntitiesParser.h"

#include "MD5Manager.h"

#include "ce.h"

#include "ceParser.h"

#include "Bezier.h"

#include "Character.h"

#include "Input.h"

#include "Q3A_ShaderManager.h"

#include "Server.h"
extern IServer*    g_pServer;

int CurrentCameraCluster;

extern float g_fDistanceBetweenPlayer;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "Profiler.h"

/*#include "Frustum.h"
extern CFrustum g_OpenglCameraFrustum;*/
#include "Frustum.h"
//extern CFrustum g_Frustum;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

extern int g_iUserDefinedBezierPatchTessLevel;

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;


#include "BSP_PhysicsModel.h"
#include "BasicPhysics.h"
#include "StaticBasicPhysics.h"
#include "ControllablePhysics.h"
#include "NetControllablePhysics.h"

#include "StaticEntity.h"
#include "CollectableEntity.h"
#include "BotEntity.h"
#include "HumanPlayerEntity.h"
#include "NetPlayerEntity.h"
#include "JumpPadEntity.h"
#include "TeleporterEntity.h"
#include "ProjectileEntity.h"
#include "ProjectileEntity_Server.h"
#include "HurtEntity.h"
#include "ParticleSystemEntity.h"
#include "BasicPhysics.h"
#include "DummyPhysicsModel.h"
#include "ParticleSystemManager.h"
//#include "SoundManager.h"

extern unsigned int g_uiControlledHumanPlayer;

extern int g_idbgJumppadAddForceCount;

//#include <crtdbg.h>
#include <assert.h>

extern int g_idbgEntitiesDrawnNum;
extern int g_idbgEntitiesOnlyNecessaryRMUpdate;

extern bool g_bDeadReckoning;

extern bool g_bRenderableRequirePostProcessing;

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////



//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// Constructer
//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
CQuake3BSP::CQuake3BSP()
{

	m_iCurrentCollidedPlanes = 0;

	m_bPrebatching = false;
	m_bUpdateEntitiesRenderModels = true;
	m_bDepthFirstMode = false;
	
	m_iVertsNum    = 0;	
	m_iFacesNum    = 0;	
	m_iTexturesNum = 0;

	
	m_pcMapFileName = NULL;
    	
	m_pVerts = NULL;
	m_pFaces = NULL;	
	m_strEntities = NULL; 
	//m_pNodes = NULL;
	m_pLeafs = NULL;
//	m_pPlanesORIGINAL = NULL;
	m_pPlanes = NULL;
	m_pLeafFaces = NULL;
	m_pTextures = NULL;
	m_pUsedTextures = NULL;
	//m_pLeafBrushes = NULL;
//	m_pBrushesORIGINAL = NULL;
	//m_pBrushes = NULL;
	//m_pBrushSides = NULL;

	m_pFrustum = NULL;

	m_clusters.pBitsets = NULL;

	m_iMapModelsNum = 0;
	m_pMapModels = NULL;

	m_iStartPositionsNum=0;
	m_pStartPositions = NULL;
	m_fStartAngles    = NULL;


	m_pRenderables = NULL;
	m_pSharedMaterialRenderables = NULL;

	m_pIndexBuffer_Bezier = NULL;
	m_pIndexBuffer_Dynamic = NULL;

	m_pIndicesData_Bezier = NULL;

	// lights
	m_ppLights = NULL;
	m_iLightsNum = 0;

	// physics model

	m_pPhysicsModel = new CBSP_PhysicsModel();
	
	
	//m_bRenderHumanModel = true;
	m_bRenderHumanModel = false;

	m_bRenderShadowCastersMode = false;

	// touching


	/*m_spSoundsTOUCHING[0] = st_SoundManager->Load("Sound/jumppad.wav");
	m_spSoundsTOUCHING[1] = st_SoundManager->Load("Sound/teleout.wav");
	m_spSoundsTOUCHING[2] = st_SoundManager->Load("Sound/rock1x1a.wav");
	m_spSoundsTOUCHING[3] = st_SoundManager->Load("Sound/blue_launch.wav");
	m_spSoundsTOUCHING[4] = st_SoundManager->Load("Sound/rock1fla.wav");*/

	//m_pRocketLaunchSound_TOUCH = st_SoundManager
	
	/*g_SoundAPI.LoadSample("Sound/rocklf1a.wav",m_pRocketLaunchSound_TOUCH);

	m_pRocketExplotionSound_TOUCH = NULL;
	g_SoundAPI.LoadSample("Sound/rocklx1a.wav",m_pRocketExplotionSound_TOUCH);

	m_pBlueLaunch_TOUCH = NULL;
	g_SoundAPI.LoadSample("Sound/blue_launch.wav",m_pBlueLaunch_TOUCH);*/
	


}




//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// Destructer
//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
CQuake3BSP::~CQuake3BSP()
{
	
/*	if (m_pcMapFileName)
	
	{*/


		g_pLogFile->OutPutPlainText("Freeing Quake 3 Arena BSP File:","3","000099",false,LOG_MESSAGE_INFORMATIVE);	
		g_pLogFile->OutPutPlainText(m_pcMapFileName,"3","000000",false,LOG_MESSAGE_INFORMATIVE);
		g_pLogFile->OutPutPlainText(" ","3","000099",false,LOG_MESSAGE_INFORMATIVE);
/*	}
	else
	{
		///
	}*/

	
	SafeDeleteArray(m_pStartPositions);
	SafeDeleteArray(m_fStartAngles);



	
	SafeDeleteArray(m_pFaces);		// The faces information of the object
	
	SafeDeleteArray(m_strEntities);            // our Entities String

	//SafeDeleteArray(m_pNodes);
	SafeDeleteArray(m_pLeafs);
	//SafeDeleteArray(m_pPlanesORIGINAL);
	SafeDeleteArray(m_pPlanes);
	SafeDeleteArray(m_pLeafFaces);


	SafeDeleteArray(m_pTextures);
	SafeDeleteArray(m_pUsedTextures);
	//SafeDeleteArray(m_pLeafBrushes);           // The index into the brush array
//	SafeDeleteArray(m_pBrushesORIGINAL);
	//SafeDeleteArray(m_pBrushes);
	//SafeDeleteArray(m_pBrushSides);

	SafeDeleteArray(m_pVerts);


	SafeDeleteArray(m_pcMapFileName);

	SafeDeleteArray(m_pMapModels);

	SafeDeleteArray(m_clusters.pBitsets);



	g_pLogFile->OutPutPlainText("Successful.","3","009900",true,LOG_MESSAGE_INFORMATIVE);


}

void CQuake3BSP::ConvertFromQuakeSpaceToD3DSpace(CVec3f& vec)
{
	float temp;

	temp = vec.z;
	vec.z = -vec.y;
	vec.y = temp;
}


//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// load the q3 bsp map from file
//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CQuake3BSP::LoadBSP(const char *strFileName)
{

	FILE *fp = NULL;
	int i = 0;
	
	g_pLogFile->OutPutPlainText("Attempting to load:","3","000099",false,LOG_MESSAGE_INFORMATIVE);
	g_pLogFile->OutPutPlainText(strFileName,"3","000000",false,LOG_MESSAGE_INFORMATIVE);
	g_pLogFile->OutPutPlainText(".","3","000000",true,LOG_MESSAGE_INFORMATIVE);
	
	g_pLogFile->OutPutPlainText(strFileName,"3","000000",false,LOG_MESSAGE_INFORMATIVE);
	g_pLogFile->OutPutPlainText(" is ","3","000000",false,LOG_MESSAGE_INFORMATIVE);

	
	if((fp = fopen(strFileName, "rb")) == NULL)
	{
		g_pLogFile->OutPutPlainText("NOT FOUND!!!","3","990000",true,LOG_MESSAGE_ERROR);
		//MessageBox(g_pWindow->hWnd, "Could not find BSP file!", "Error", MB_OK);
		return false;
	}
	else
		g_pLogFile->OutPutPlainText("found.","3","009900",true,LOG_MESSAGE_INFORMATIVE);


	g_pLogFile->OutPutPlainText("Creating Temp BSP data arrays:","3","000000",false,LOG_MESSAGE_INFORMATIVE);

	// Initialize the header and lump structures
	tBSPHeader header = {0};
	tBSPLump lumps[kMaxLumps] = {0};

	// Read in the header and lump data
	fread(&header, 1, sizeof(tBSPHeader), fp);
	fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);


	// Allocate the vertex memory
	m_iVertsNum = lumps[kVertices].length / sizeof(tBSPVertex);
	m_pVerts     = new tBSPVertex [m_iVertsNum];

	// Allocate the face memory
	m_iFacesNum = lumps[kFaces].length / sizeof(tBSPFace);
	m_pFaces     = new tBSPFace [m_iFacesNum];

	// Allocate memory to read in the texture information.
	m_iTexturesNum = lumps[kTextures].length / sizeof(tBSPTexture);
	m_pTextures = new tBSPTexture [m_iTexturesNum];

	m_pUsedTextures = new bool[m_iTexturesNum];

	/*// Allocate memory to read in the lightmap data.  
	m_iLightmapsNum = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap *pLightmaps = new tBSPLightmap [m_iLightmapsNum ];*/


	
	//m_sppLightMaps = new CSmartPointer<CTexture>[m_numOfLightmaps];
	

	/*m_ppLightMaps = new CTexture*[m_numOfLightmaps];
	for (int iCount=0;iCount<m_numOfLightmaps;iCount++)
		m_ppLightMaps[iCount] = new CTexture;*/


	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	float temp=0.f;
	float temp2=0.f;
	
	// Go through all of the vertices that need to be read and swap axises
	for(i = 0; i < m_iVertsNum; i++)
	{
		// Read in the current vertex
		fread(&m_pVerts[i], 1, sizeof(tBSPVertex), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		temp = m_pVerts[i].vPosition.v[1];
		m_pVerts[i].vPosition.v[1] = m_pVerts[i].vPosition.v[2];
		m_pVerts[i].vPosition.v[2] = -temp;

		// Swap the y and z values, and negate the new z so Y is up.
		temp2 = m_pVerts[i].vNormal.v[1];
		m_pVerts[i].vNormal.v[1] = m_pVerts[i].vNormal.v[2];
		m_pVerts[i].vNormal.v[2] = -temp2;
		
		// Negate the V texture coordinate because it is upside down otherwise...
		//m_pVerts[i].vTextureCoord.v[1] = 1 - m_pVerts[i].vTextureCoord.v[1];
	}	

	assert(_CrtCheckMemory());

	// Seek to the position in the file that stores the face information
	fseek(fp, lumps[kFaces].offset, SEEK_SET);

	// Read in all the face information
	fread(m_pFaces, m_iFacesNum, sizeof(tBSPFace), fp);

	
	// Seek to the position in the file that stores the texture information
	fseek(fp, lumps[kTextures].offset, SEEK_SET);
	
	// Read in all the texture information
	fread(m_pTextures, m_iTexturesNum, sizeof(tBSPTexture), fp);

	CBSP_PhysicsModel* pBSPPhysicsModel = (CBSP_PhysicsModel*) m_pPhysicsModel;

	pBSPPhysicsModel->SetTextures(m_iTexturesNum,m_pTextures);



	///////////////////////////////////
	// find the max same tris using the same material -- for q3 batching

	int* m_piMaterialTris = new int[m_iTexturesNum];

	// clear array

	for (int i=0;i<m_iTexturesNum;i++)
		m_piMaterialTris[i] = 0;

	// fill array

	for (int i=0;i<m_iFacesNum;i++)
	{
		m_piMaterialTris[m_pFaces[i].textureID] += m_pFaces[i].numMeshVerts;
	}

	m_iMaxTrisSharingMaterial = -9999;

	for (int i=0;i<m_iTexturesNum;i++)
		if (m_piMaterialTris[i]>m_iMaxTrisSharingMaterial)
			m_iMaxTrisSharingMaterial = m_piMaterialTris[i];



	//////////////////////////////////////////////////////////


	// DEBUG
/*
	int iDebugContents=0;
	int iDebugFlags=0;

	char* cpDebugText = new char[400];


	for (int t=0;t<m_iTexturesNum;t++)
	{
		iDebugContents = m_pTextures[t].contents;
		iDebugFlags = m_pTextures[t].flags;

		sprintf(cpDebugText,"Texture #%d: Contents:%d, Flags:%d",t,iDebugContents,iDebugFlags);
		g_LogFile.OutPutPlainText(cpDebugText,"3","000099",true);	


	}


	SafeDeleteArray(cpDebugText);*/
	////////

	

	char paImageName[200];
		

	// Seek to the position in the file that stores the lightmap information
	fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

/*	// Go through all of the lightmaps and read them in
	for(i = 0; i < m_numOfLightmaps ; i++)
	{
		// Read in the RGB data for each lightmap
		fread(&pLightmaps[i], 1, sizeof(tBSPLightmap), fp);

		// Create a texture map for each lightmap that is read in.  The lightmaps
		// are always 128 by 128.


		////assert(_CrtCheckMemory());
		m_sppLightMaps[i] = CSingleton<CTextureManager>::Inst()->LoadLightMapAndPush((unsigned char *)pLightmaps[i].imageBits, 128, 128);
		////assert(_CrtCheckMemory());

	}


	assert(_CrtCheckMemory());

	// Delete the image bits because we are already done with them
	delete [] pLightmaps;*/

	// the BSP nodes, the leafs, the leaf faces, BSP splitter planes and
	// visibility data (clusters).
	/****************************************************************/

	// Store the number of nodes and allocate the memory to hold them
	m_iNodesNum = lumps[kNodes].length / sizeof(tBSPNode);
	m_pNodes     = new tBSPNode [m_iNodesNum];

	// Seek to the position in the file that hold the nodes and store them in pNodes
	fseek(fp, lumps[kNodes].offset, SEEK_SET);
	fread(m_pNodes, m_iNodesNum, sizeof(tBSPNode), fp);

	for(i = 0; i < m_iNodesNum; i++)
	{
		// Swap the y and z values, and negate the new z so Y is up.
		float temp = m_pNodes[i].max.y;
		m_pNodes[i].max.y = m_pNodes[i].max.z;
		m_pNodes[i].max.z = -temp;

		temp = m_pNodes[i].min.y;
		m_pNodes[i].min.y = m_pNodes[i].min.z;
		m_pNodes[i].min.z = -temp;


		///// DEBUG DEBUG DEBUG

		temp = m_pNodes[i].max.z;
		m_pNodes[i].max.z = m_pNodes[i].min.z;
		m_pNodes[i].min.z = temp;
	}	


	

	pBSPPhysicsModel->SetNodes(m_iNodesNum,m_pNodes);



	tBSPLeafORIGINAL* pTempLeafs;

	// Store the number of leafs and allocate the memory to hold them
	m_iLeafsNum = lumps[kLeafs].length / sizeof(tBSPLeafORIGINAL);
	pTempLeafs     = new tBSPLeafORIGINAL [m_iLeafsNum];

	//check how many extra brushes will be needed for the bezier patches

	int iNeededBezierBrushes = 0;
	int iNeededBezierBrushesPlanes = 0;

	for (int i=0;i<m_iFacesNum;i++)
	{
		if (m_pFaces[i].type!=Q3FACE_BEZIER_PATCH)
			continue;

		m_BezierFaceToBezierBrush[i] = iNeededBezierBrushes;
		
		iNeededBezierBrushes+= GetRequiredBrushesNum(i);	

		iNeededBezierBrushesPlanes+= GenerateBrushPlanesFromBezierPatch(i,NULL);
	}

	/////////////////


	m_pLeafs       = new tBSPLeaf [m_iLeafsNum];


	assert(_CrtCheckMemory());

	// Seek to the position in the file that holds the leafs and store them in pTempLeafs
	fseek(fp, lumps[kLeafs].offset, SEEK_SET);

	assert(_CrtCheckMemory());

	fread(pTempLeafs, m_iLeafsNum, sizeof(tBSPLeafORIGINAL), fp);

	assert(_CrtCheckMemory());

	// Now we need to go through and convert all the leaf bounding boxes
	// to the normal OpenGL Y up axis.
	for(i = 0; i < m_iLeafsNum; i++)
	{
		// Swap the min y and z values, then negate the new Z
		float temp = pTempLeafs[i].min.y;
		pTempLeafs[i].min.y = pTempLeafs[i].min.z;
		pTempLeafs[i].min.z = -temp;

		// Swap the max y and z values, then negate the new Z
		temp = pTempLeafs[i].max.y;
		pTempLeafs[i].max.y = pTempLeafs[i].max.z;
		pTempLeafs[i].max.z = -temp;


		temp = pTempLeafs[i].max.z;
		pTempLeafs[i].max.z = pTempLeafs[i].min.z;
		pTempLeafs[i].min.z = temp;

	}


	assert(_CrtCheckMemory());



	for (int l=0;l<m_iLeafsNum;l++)
	{
		m_pLeafs[l].cluster = pTempLeafs[l].cluster;
		m_pLeafs[l].area = pTempLeafs[l].area;
		m_pLeafs[l].min.x = pTempLeafs[l].min.x;
		m_pLeafs[l].min.y = pTempLeafs[l].min.y;
		m_pLeafs[l].min.z = pTempLeafs[l].min.z;
		m_pLeafs[l].max.x = pTempLeafs[l].max.x;
		m_pLeafs[l].max.y = pTempLeafs[l].max.y;
		m_pLeafs[l].max.z = pTempLeafs[l].max.z;
		m_pLeafs[l].leafface = pTempLeafs[l].leafface;
		m_pLeafs[l].numOfLeafFaces = pTempLeafs[l].numOfLeafFaces;
		m_pLeafs[l].leafBrush = pTempLeafs[l].leafBrush;
		m_pLeafs[l].numOfLeafBrushes = pTempLeafs[l].numOfLeafBrushes;		
	}


	SafeDeleteArray(pTempLeafs);

	pBSPPhysicsModel->SetLeafs(m_iLeafsNum,m_pLeafs);

	// Store the number of leaf faces and allocate the memory for them
	m_iLeafFacesNum = lumps[kLeafFaces].length / sizeof(int);
	m_pLeafFaces     = new int [m_iLeafFacesNum];

	// Seek to the leaf faces lump, then read it's data
	fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
	fread(m_pLeafFaces, m_iLeafFacesNum, sizeof(int), fp);
	

	// Store the number of planes, then allocate memory to hold them
	m_iPlanesNum_Original = lumps[kPlanes].length / sizeof(tBSPPlane);
	tBSPPlane* pPlanesORIGINAL     = new tBSPPlane [m_iPlanesNum_Original];
	m_pPlanes             = new CPlane [m_iPlanesNum_Original+iNeededBezierBrushesPlanes];

	// Seek to the planes lump in the file, then read them into pPlanesORIGINAL
	fseek(fp, lumps[kPlanes].offset, SEEK_SET);
	fread(pPlanesORIGINAL, m_iPlanesNum_Original, sizeof(tBSPPlane), fp);

	// Go through every plane and convert it's normal to the Y-axis being up
	for(i = 0; i < m_iPlanesNum_Original; i++)
	{
		m_pPlanes[i].n.v[0] = pPlanesORIGINAL[i].vNormal.v[0];
		m_pPlanes[i].n.v[1] = pPlanesORIGINAL[i].vNormal.v[2];
		m_pPlanes[i].n.v[2] = -pPlanesORIGINAL[i].vNormal.v[1];

		m_pPlanes[i].d = pPlanesORIGINAL[i].d;

	}

	// generate the planes needed for the bezier brushes
	int iCurrentPlane = 0;

	for (int i=0;i<m_iFacesNum;i++)
	{
		if (m_pFaces[i].type!=Q3FACE_BEZIER_PATCH)
			continue;

		/*CVec3f min,max;
		min.Set(9999.f,9999.f,9999.f);
		max.Set(-9999.f,-9999.f,-9999.f);		

		// calculate bbox
		for (int ind=m_pFaces[i].startVertIndex;ind<m_pFaces[i].startVertIndex+m_pFaces[i].numOfVerts;ind++)
		{
			if (m_pVerts[ind].vPosition.x > max.x)
				max.x = m_pVerts[ind].vPosition.x;

			if (m_pVerts[ind].vPosition.y > max.y)
				max.y = m_pVerts[ind].vPosition.y;

			if (m_pVerts[ind].vPosition.z > max.z)
				max.z = m_pVerts[ind].vPosition.z;


			if (m_pVerts[ind].vPosition.x < min.x)
				min.x = m_pVerts[ind].vPosition.x;

			if (m_pVerts[ind].vPosition.y < min.y)
				min.y = m_pVerts[ind].vPosition.y;

			if (m_pVerts[ind].vPosition.z < min.z)
				min.z = m_pVerts[ind].vPosition.z;			
		}

		BBox bbox;
		bbox.min = min;
		bbox.max = max;*/
		
		// generate the required planes planes for the bezier brush
		int iPlanes = GenerateBrushPlanesFromBezierPatch(i,NULL);

		GenerateBrushPlanesFromBezierPatch(i,&m_pPlanes[m_iPlanesNum_Original+iCurrentPlane]);
		iCurrentPlane+= iPlanes;
	}



	SafeDeleteArray(pPlanesORIGINAL);


	pBSPPhysicsModel->SetPlanes(m_iPlanesNum_Original+iNeededBezierBrushesPlanes,m_pPlanes);

	// Seek to the position in the file that holds the visibility lump
	fseek(fp, lumps[kVisData].offset, SEEK_SET);

	// Check if there is any visibility information first
	if(lumps[kVisData].length) 
	{
		// Read in the number of vectors and each vector's size
		fread(&(m_clusters.numOfClusters),	 1, sizeof(int), fp);
		fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);

		// Allocate the memory for the cluster bitsets
		int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
		m_clusters.pBitsets = new unsigned char [size];

		// Read in the all the visibility bitsets for each cluster
		fread(m_clusters.pBitsets, 1, sizeof(unsigned char) * size, fp);
	}
	// Otherwise, we don't have any visibility data (prevents a crash)
	else
		m_clusters.pBitsets = NULL;

	// BRUSHES
	int iBezierLeafBrushesNum=0;
	int iCurrentFace = -1;
	for (int l=0;l<m_iLeafsNum;l++)
	{
		//check through all of the leaf faces, to see which are bezier
		for (int f=0;f<m_pLeafs[l].numOfLeafFaces;f++)
		{
			iCurrentFace = m_pLeafFaces[m_pLeafs[l].leafface+f];
			if (m_pFaces[iCurrentFace].type == Q3FACE_BEZIER_PATCH)
			{
				iBezierLeafBrushesNum+= GetRequiredBrushesNum(iCurrentFace);
			}
		}		
	}

	// Read in index to leaf brushes
	int iLeafBrushesNum = lumps[kLeafBrushes].length / sizeof (int);
	int* pLeafBrushes = new int[iLeafBrushesNum+iBezierLeafBrushesNum];
	fseek (fp, lumps[kLeafBrushes].offset, SEEK_SET);
	fread (pLeafBrushes, iLeafBrushesNum, sizeof (int), fp);	
	pBSPPhysicsModel->SetLeafBrushes(iLeafBrushesNum,pLeafBrushes);

	/*for (int i=0;i<iNeededBezierBrushes;i++)
	{
		pLeafBrushes[iLeafBrushesNum+i] = iBrushesNum_Original+i;
	}*/

	// read the models
	m_iMapModelsNum = lumps[kModels].length / sizeof (tBSPModel);
	m_pMapModels = new tBSPModel [m_iMapModelsNum];



	// Seek to the position in the file that stores the models
	fseek(fp, lumps[kModels].offset, SEEK_SET);
	//fread (m_pMapModels, m_iMapModelsNum, sizeof (tBSPModel), fp);

	
	for (i=0;i<m_iMapModelsNum;i++)
	{
		// Read in the current vertex
		fread(&m_pMapModels[i], 1, sizeof(tBSPModel), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		temp = m_pMapModels[i].min.v[1];
		m_pMapModels[i].min.v[1] = m_pMapModels[i].min.v[2];
		m_pMapModels[i].min.v[2] = -temp;

		temp = m_pMapModels[i].max.v[1];
		m_pMapModels[i].max.v[1] = m_pMapModels[i].max.v[2];
		m_pMapModels[i].max.v[2] = -temp;


		temp = m_pMapModels[i].max.v[2];
		m_pMapModels[i].max.v[2] = m_pMapModels[i].min.v[2];
		m_pMapModels[i].min.v[2] = temp;


	}
	


	// Read in the brushes
	int iBrushesNum_Original = lumps[kBrushes].length / sizeof (tBSPBrushORIGINAL);
	tBSPBrushORIGINAL * pBrushesORIGINAL = new tBSPBrushORIGINAL[iBrushesNum_Original];
	fseek (fp, lumps[kBrushes].offset, SEEK_SET);
	fread (pBrushesORIGINAL, iBrushesNum_Original, sizeof (tBSPBrushORIGINAL), fp);

	// convert from tBSPBrushORIGINAL to tBSPBrush

	tBSPBrush* pBrushes = new tBSPBrush[iBrushesNum_Original+iNeededBezierBrushes];

	int iBrushSidesNum_Original =	lumps[kBrushSides].length / sizeof (tBSPBrushORIGINALSide);

	for (int b=0;b<iBrushesNum_Original;b++)
	{
		pBrushes[b].numOfBrushSides = pBrushesORIGINAL[b].numOfBrushSides;
		pBrushes[b].brushSide = pBrushesORIGINAL[b].brushSide;
		pBrushes[b].textureID = pBrushesORIGINAL[b].textureID;
		pBrushes[b].m_pPlanes = NULL;
	}

	int iCurrentBezierBrush=0;
	int iRequiredBrushes=-1;

	for (int i=0;i<m_iFacesNum;i++)
	{
		if (m_pFaces[i].type!=Q3FACE_BEZIER_PATCH)
			continue;		

		iRequiredBrushes = GetRequiredBrushesNum(i);
		
		for (int b=0;b<iRequiredBrushes;b++)
		{
			pBrushes[iBrushesNum_Original+iCurrentBezierBrush+b].numOfBrushSides = PLANES_PER_BEZIER_BRUSH;
			pBrushes[iBrushesNum_Original+iCurrentBezierBrush+b].brushSide = iBrushSidesNum_Original+((iCurrentBezierBrush+b)*PLANES_PER_BEZIER_BRUSH); // i need to add offset to after the end of the original brush side array
			pBrushes[iBrushesNum_Original+iCurrentBezierBrush+b].textureID = pBrushesORIGINAL[0].textureID; // i need to change this into a correct textureID
			pBrushes[iBrushesNum_Original+iCurrentBezierBrush+b].m_pPlanes = NULL;
		}
		
		iCurrentBezierBrush+= iRequiredBrushes;		
	}


	/*
	// add the bezier generated brushes
	for (int b=0;b<iNeededBezierBrushes;b++)
	{
		pBrushes[iBrushesNum_Original+b].numOfBrushSides = 6;
		pBrushes[iBrushesNum_Original+b].brushSide = iBrushSidesNum_Original+(b*6); // i need to add offset to after the end of the original brush side array
		pBrushes[iBrushesNum_Original+b].textureID = pBrushesORIGINAL[0].textureID; // i need to change this into a correct textureID
		pBrushes[iBrushesNum_Original+b].m_pPlanes = NULL;
	}*/
	
	SafeDeleteArray(pBrushesORIGINAL);

	//add the bezier brush data for the leafs

	int iCurrentBrushLeaf=0;
	iCurrentFace=0;
	int iNeededBrushes = -1;
	for (int l=0;l<m_iLeafsNum;l++)
	{
		m_pLeafs[l].leafBezierBrush = iLeafBrushesNum+iCurrentBrushLeaf;

		//check through all of the leaf faces, to see which are bezier
		for (int f=0;f<m_pLeafs[l].numOfLeafFaces;f++)
		{
			iCurrentFace = m_pLeafFaces[m_pLeafs[l].leafface+f];
			if (m_pFaces[iCurrentFace].type == Q3FACE_BEZIER_PATCH)
			{
				int iBezierBrush = m_BezierFaceToBezierBrush[iCurrentFace];

				iNeededBrushes = GetRequiredBrushesNum(iCurrentFace);

				for (int b=0;b<iNeededBrushes;b++)
				{
					pLeafBrushes[iLeafBrushesNum+iCurrentBrushLeaf+b] = iBrushesNum_Original+iBezierBrush+b;
				}
				iCurrentBrushLeaf+= iNeededBrushes;
			}
		}		

		m_pLeafs[l].numOfBezierLeafBrushes = iLeafBrushesNum+iCurrentBrushLeaf-m_pLeafs[l].leafBezierBrush;
	}



	pBSPPhysicsModel->SetBrushes(iBrushesNum_Original+iNeededBezierBrushes,pBrushes);

	// Read in the brush sides
	
	tBSPBrushORIGINALSide * pBrushSides = new tBSPBrushORIGINALSide[iBrushSidesNum_Original+(iNeededBezierBrushes*PLANES_PER_BEZIER_BRUSH)];
	fseek (fp, lumps[kBrushSides].offset, SEEK_SET);
	fread (pBrushSides, iBrushSidesNum_Original, sizeof (tBSPBrushORIGINALSide), fp);

	for (int i=0;i<iNeededBezierBrushes*PLANES_PER_BEZIER_BRUSH;i++)
	{
		pBrushSides[iBrushSidesNum_Original+i].textureID = 0; // todo: fix this
		pBrushSides[iBrushSidesNum_Original+i].plane = m_iPlanesNum_Original+i;
	}
	
	pBSPPhysicsModel->SetBrushesSides(iBrushSidesNum_Original+(iNeededBezierBrushes*PLANES_PER_BEZIER_BRUSH),pBrushSides);




	m_iIndicesNum = lumps[kMeshVerts].length / sizeof(int);

	//m_pIndexArray = new int [m_iIndicesNum];

	fseek(fp, lumps[kMeshVerts].offset, SEEK_SET);

	//fread(m_pIndexArray, m_iIndicesNum, sizeof(int), fp);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// fill in the big Index buffer
	
	m_pIndexBuffer = g_pRendererBack->GenerateIndexBuffer();
	//m_pIndexBuffer->Allocate(m_iIndicesNum,IB_MEM_GPU | IB_STATIC);
	m_pIndexBuffer->Allocate(m_iIndicesNum,IB_MEM_GPU | IB_STATIC);
	
	m_pIndicesData = new UINT[m_iIndicesNum];
	

	
	
	//g_pLogFile->OutPutPlainText("BEFORE face renderable index buffer read","3","009900",true,LOG_MESSAGE_INFORMATIVE);
	m_pIndexBuffer->Lock(CR_LOCK_WRITE_ONLY,0,0);
	UINT* pIndices = (UINT*) m_pIndexBuffer->GetData();
	//g_pLogFile->OutPutPlainText("AFTER face renderable index buffer read","3","009999",true,LOG_MESSAGE_INFORMATIVE);

	fread(m_pIndicesData, m_iIndicesNum, sizeof(UINT), fp);	

	int iBufferVertsOffset = 0;

	for (int i=0;i<m_iFacesNum;i++)
	{	

		// i need to think what to do with faces with no vertices
		if (m_pFaces[i].numMeshVerts==0)
			continue;
		

		for (int j=m_pFaces[i].meshVertIndex;j< m_pFaces[i].meshVertIndex+m_pFaces[i].numMeshVerts;j++)
		{
			m_pIndicesData[j]+=m_pFaces[i].startVertIndex;
		}

		iBufferVertsOffset+= m_pFaces[i].numOfVerts;
	}

	memcpy(pIndices,m_pIndicesData,sizeof(UINT)*m_iIndicesNum);

	m_pIndexBuffer->Unlock();

	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// fill in the shared material index buffers

	if (m_bPrebatching)
	{
		m_ppIndexBuffersOfSharedMaterials = new IIndexBuffer*[m_iTexturesNum];
		for (int i=0;i<m_iTexturesNum;i++)
			m_ppIndexBuffersOfSharedMaterials[i] = NULL;	

		m_pSharedMaterialIndices_TEMP = new UINT*[m_iTexturesNum];



		for (int iCurrTexID=0;iCurrTexID<m_iTexturesNum;iCurrTexID++)
		{
			
			int iSharedIndicesNum = 0;
			
			// find out how many indices share this
			for (int i=0;i<m_iFacesNum;i++)
			{
				if (m_pFaces[i].textureID == iCurrTexID)
					iSharedIndicesNum+= m_pFaces[i].numMeshVerts;
			}		

			if (iSharedIndicesNum == 0)
			{
				m_ppIndexBuffersOfSharedMaterials[iCurrTexID] = NULL;
				m_pSharedMaterialIndices_TEMP[iCurrTexID] = NULL;
				continue;

			}

			m_pSharedMaterialIndices_TEMP[iCurrTexID] = new UINT[iSharedIndicesNum];
			UINT* pSharedMaterialIndices_TEMP = m_pSharedMaterialIndices_TEMP[iCurrTexID];
		
		
			for (int i=0;i<m_iFacesNum;i++)
			{
				if (m_pFaces[i].textureID == iCurrTexID)
				{
					memcpy(pSharedMaterialIndices_TEMP,&m_pIndicesData[m_pFaces[i].meshVertIndex],sizeof(UINT)*m_pFaces[i].numMeshVerts);
					pSharedMaterialIndices_TEMP+=m_pFaces[i].numMeshVerts;				
				}
			}		


			m_ppIndexBuffersOfSharedMaterials[iCurrTexID] = g_pRendererBack->GenerateIndexBuffer();
			m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Allocate(iSharedIndicesNum,IB_MEM_GPU | IB_STATIC);
			//m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Allocate(iSharedIndicesNum,IB_MEM_GPU | IB_STATIC);
			
			
			//g_pLogFile->OutPutPlainText("BEFORE shared material renderable index buffer read","3","009900",true,LOG_MESSAGE_INFORMATIVE);
			m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Lock(CR_LOCK_WRITE_ONLY,0,0);				
			UINT* pSharedMaterialIndices = (UINT*) m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->GetData();		
			//g_pLogFile->OutPutPlainText("AFTER shared material renderable index buffer read","3","009999",true,LOG_MESSAGE_INFORMATIVE);

			memcpy(pSharedMaterialIndices,m_pSharedMaterialIndices_TEMP[iCurrTexID],sizeof(UINT) * iSharedIndicesNum);
			m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Unlock();

		}
	
	}
		

	//////////////////////////////////////////////////////////////////////////////////////////





	// ADDED read entities into m_strEntities

	m_EntitiesStringLength = lumps[kEntities].length / sizeof(char);
	
	m_strEntities = new char [m_EntitiesStringLength];

	fseek(fp, lumps[kEntities].offset, SEEK_SET);

	fread(m_strEntities,m_EntitiesStringLength,sizeof(char), fp);

	// debug debug
	g_pLogFile->OutPutPlainText( m_strEntities    ,LOG_MESSAGE_INFORMATIVE);


	// Close the file
	if (fp)
	fclose(fp);

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_iFacesNum);


	g_pLogFile->OutPutPlainText("Successful.","3","009900",true,LOG_MESSAGE_INFORMATIVE);

	// convert faces into vertexbuffers and fill m_pVertexBuffer with them,
	// (textures and shaders are also loaded here)

	
	if (!g_pRendererFront->GetRoughFallBack())
		FillVBArray(RENDERABLE_GENERATE_PPL);
	else
		FillVBArray(0);

	

	LoadMapEntities();

	// temp hard coded lights list:

	m_iLightsNum = 1;
	m_ppLights = new ILight_Export*[m_iLightsNum];

	m_ppLights[0] = g_pRendererBack->GenerateLight();

	////(407.98f,508.14f,-765.58f) (0.30f,-0.29f,-0.91f) (-17.10f,-378.40f,0.00f)
	m_ppLights[0]->SetPos(CVec3f(407.98f,508.14f,-765.58f)); 
	m_ppLights[0]->SetLookAt(CVec3f(0.30f,-0.29f,-0.91f));

	
	// restore
	
	//m_ppLights[0]->SetColor(CVec3f(0.9f,0.6f,0.6f));
	m_ppLights[0]->SetColor(CVec3f(0.65f,0.65f,0.65f));
	//m_ppLights[0]->SetFrustumFarPlane(1000.f);
	//m_ppLights[0]->SetFrustumFarPlane(1500.f);

	m_ppLights[0]->SetFrustumFarPlane(10500.f);


	/*

	m_ppLights[1] = g_pRendererBack->GenerateLight();
	m_ppLights[1]->SetPos(CVec3f(499.84f,26.09f,195.15f));	
	m_ppLights[1]->SetLookAt(CVec3f(-0.08f,0.08f,-0.99f));
	m_ppLights[1]->SetColor(CVec3f(0.6f,0.6f,0.9f));
	m_ppLights[1]->SetFrustumFarPlane(1000.f);

	*/

/*


	m_ppLights[2] = g_pRendererBack->GenerateLight();
	m_ppLights[2]->SetPos(CVec3f(71.86f,816.74f,855.42f));	
	m_ppLights[2]->SetLookAt(CVec3f(0.07f,-0.92f,-0.38f));
	m_ppLights[2]->SetColor(CVec3f(0.6f,0.9f,0.6f));
	m_ppLights[2]->SetFrustumFarPlane(1000.f);



	m_ppLights[3] = g_pRendererBack->GenerateLight();
	m_ppLights[3]->SetPos(CVec3f(3.92f,949.36f,-664.60f));	
	m_ppLights[3]->SetLookAt(CVec3f(-0.41f,-0.78f,-0.47f));
	m_ppLights[3]->SetColor(CVec3f(0.6f,0.9f,0.9f));
	m_ppLights[3]->SetFrustumFarPlane(1000.f);

	*/

	for (int i=0;i<m_iLightsNum;i++)
		m_ppLights[i]->Update();

	LinkRenderablesToLights();
	LinkLevelEntitiesToLights(false);

	return true;
}



bool CQuake3BSP::DoPointsOnlyChangeInOneDimention(CVec3f& v1, CVec3f& v2, CVec3f& v3)
{
	bool sameX = false;
	bool sameY = false;
	bool sameZ = false;

	//TODO: Add epsilon
	if ( (v1.x == v2.x) && (v1.x == v3.x))
		sameX = true;

	if ( (v1.y == v2.y) && (v1.y == v3.y))
		sameY = true;

	if ( (v1.z == v2.z) && (v1.z == v3.z))
		sameZ = true;

	if (sameX && sameY)
		return true;

	if (sameX && sameZ)
		return true;

	if (sameY && sameZ)
		return true;

	return false;
}

CVec3f CQuake3BSP::FindMin(CVec3f* pVecs, UINT* pIndices, UINT count)
{
	CVec3f min;
	min.Set(9999.f,9999.f,9999.f);

	for (UINT i=0;i<count;i++)
		{			
			if (pVecs[pIndices[i]].x < min.x)
				min.x = pVecs[pIndices[i]].x;

			if (pVecs[pIndices[i]].y < min.y)
				min.y = pVecs[pIndices[i]].y;

			if (pVecs[pIndices[i]].z < min.z)
				min.z = pVecs[pIndices[i]].z;	
		}

		return min;
}

CVec3f CQuake3BSP::FindMax(CVec3f* pVecs, UINT* pIndices, UINT count)
{
	CVec3f max;
	max.Set(-9999.f,-9999.f,-9999.f);

	for (UINT i=0;i<count;i++)
	{			
		if (pVecs[pIndices[i]].x > max.x)
			max.x = pVecs[pIndices[i]].x;

		if (pVecs[pIndices[i]].y > max.y)
			max.y = pVecs[pIndices[i]].y;

		if (pVecs[pIndices[i]].z > max.z)
			max.z = pVecs[pIndices[i]].z;	
	}

	return max;
}

int CQuake3BSP::GetRequiredBrushesNum(int iFaceNum)
{
	BREAKPOINT(m_pFaces[iFaceNum].type != Q3FACE_BEZIER_PATCH);

	int iSizeX = m_pFaces[iFaceNum].size[0];
	int iSizeY = m_pFaces[iFaceNum].size[1];

	//int iRes = ((iSizeX-1)*(iSizeY-1))/4;

	int iPostTess_Width = ((iSizeX-1)/2)*(COLLISION_BEZIER_PATCH_TESS+1);
	int iPostTess_Height = ((iSizeY-1)/2)*(COLLISION_BEZIER_PATCH_TESS+1);

	int iRes = (iPostTess_Width-1)*(iPostTess_Height-1)*2;

	return iRes;
	//return 1;

}

int CQuake3BSP::GenerateBrushPlanesFromBezierPatch(int iFaceNum, OUT CPlane* pPlanes)
{
	BREAKPOINT(m_pFaces[iFaceNum].type != Q3FACE_BEZIER_PATCH);

	int iSizeX = m_pFaces[iFaceNum].size[0];
	int iSizeY = m_pFaces[iFaceNum].size[1];

	// Tesselate a rough version of the bezier patch in order to collide against it

	static CVec3f* pControlPointsVerts	= new CVec3f[27*27];
	static CVec2f* pControlPointsTCs	= new CVec2f[27*27];

	static CVec3f* pTargetVerts = new CVec3f[1000];
	static CVec2f* pTargetTCs = new CVec2f[1000];

	static UINT*	pIndices_Bezier_Travel = new UINT[5000];

	for (int j=0;j<m_pFaces[iFaceNum].numOfVerts;j++)
	{
		memcpy(&pControlPointsVerts[j],&m_pVerts[m_pFaces[iFaceNum].startVertIndex+j].vPosition,sizeof(CVec3f));
		memcpy(&pControlPointsTCs[j],&m_pVerts[m_pFaces[iFaceNum].startVertIndex+j].vTextureCoord,sizeof(CVec2f));
	}

	for (int i=0;i<100;i++)
	{
		pTargetVerts[i].x = 123.f;
		pIndices_Bezier_Travel[i] = 123;
	}

	//TODO: add m_pVerts[m_pFaces[i].startVertIndex+j].vNormal,lmc,color etc.

	EvaluateQuadricBezierPatch(pControlPointsVerts,NULL,pControlPointsTCs,NULL,
		COLLISION_BEZIER_PATCH_TESS,COLLISION_BEZIER_PATCH_TESS,
		pTargetVerts,NULL,pControlPointsTCs,NULL,
		m_pFaces[iFaceNum].size[0],m_pFaces[iFaceNum].size[1],
		pIndices_Bezier_Travel,
		0);

	int iPostTess_Width = ((iSizeX-1)/2)*(COLLISION_BEZIER_PATCH_TESS+1);
	int iPostTess_Height = ((iSizeY-1)/2)*(COLLISION_BEZIER_PATCH_TESS+1);


	/*for (int i=0;i<iPostTess_Width*iPostTess_Height;i++)
	{
		ConvertFromQuakeSpaceToD3DSpace(pTargetVerts[i]);
	}*/

	int iPlanesNum = 0;

	iPlanesNum+= GetRequiredBrushesNum(iFaceNum)*PLANES_PER_BEZIER_BRUSH;

	if (!pPlanes)
		return iPlanesNum;

	//int iIndicesNum = iPostTess_Width

	
	CVec3f* pVerts = pTargetVerts;

	int iPlaneNum = 0;


	for (int i=0;i<GetRequiredBrushesNum(iFaceNum)*3;i+=3)
	{
		CVec3f min,max;
		min = FindMin(pVerts,&pIndices_Bezier_Travel[i],3);
		max = FindMax(pVerts,&pIndices_Bezier_Travel[i],3);
		
		//min = FindMin(pVerts,pIndices_Bezier_Travel,(iPostTess_Width-1)*(iPostTess_Height-1)*6);
		//max = FindMax(pVerts,pIndices_Bezier_Travel,(iPostTess_Width-1)*(iPostTess_Height-1)*6);

		//ConvertFromQuakeSpaceToD3DSpace(min);
		//ConvertFromQuakeSpaceToD3DSpace(max);

		pPlanes[iPlaneNum].FindPlane(min,CVec3f(-1.f,0.f,0.f));
		iPlaneNum++;

		pPlanes[iPlaneNum].FindPlane(min,CVec3f(0.f,-1.f,0.f));
		iPlaneNum++;

		pPlanes[iPlaneNum].FindPlane(min,CVec3f(0.f,0.f,-1.f));
		iPlaneNum++;

		pPlanes[iPlaneNum].FindPlane(max,CVec3f(1.f,0.f,0.f));
		iPlaneNum++;

		pPlanes[iPlaneNum].FindPlane(max,CVec3f(0.f,1.f,0.f));
		iPlaneNum++;

		pPlanes[iPlaneNum].FindPlane(max,CVec3f(0.f,0.f,1.f));
		iPlaneNum++;
		
	}

	/*for (int i=0;i<iPostTess_Width-1;i++)
	{
		for (int j=0;j<iPostTess_Height-1;j++)
		{
			CVec3f min,max;

			min = FindMin(pVerts,i,j,2,2,iPostTess_Width);
			max = FindMax(pVerts,i,j,2,2,iPostTess_Width);

			pPlanes[iPlaneNum].FindPlane(min,CVec3f(-1.f,0.f,0.f));
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(min,CVec3f(0.f,-1.f,0.f));
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(min,CVec3f(0.f,0.f,-1.f));
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(max,CVec3f(1.f,0.f,0.f));
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(max,CVec3f(0.f,1.f,0.f));
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(max,CVec3f(0.f,0.f,1.f));
			iPlaneNum++;*/
			
			/*
			//CW

			pPlanes[iPlaneNum].FindPlane(pVerts[i1].vPosition,pVerts[i2].vPosition,pVerts[i5].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i2].vPosition,pVerts[i5].vPosition,pVerts[i4].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i2].vPosition,pVerts[i3].vPosition,pVerts[i6].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i3].vPosition,pVerts[i6].vPosition,pVerts[i5].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i4].vPosition,pVerts[i5].vPosition,pVerts[i8].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i5].vPosition,pVerts[i8].vPosition,pVerts[i7].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i5].vPosition,pVerts[i6].vPosition,pVerts[i9].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i6].vPosition,pVerts[i9].vPosition,pVerts[i8].vPosition);
			iPlaneNum++;

			//CCW

			pPlanes[iPlaneNum].FindPlane(pVerts[i1].vPosition,pVerts[i9].vPosition,pVerts[i3].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i1].vPosition,pVerts[i7].vPosition,pVerts[i9].vPosition);
			iPlaneNum++;
			*/

			//CCW

			/*pPlanes[iPlaneNum].FindPlane(pVerts[i5].vPosition,pVerts[i2].vPosition,pVerts[i1].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i4].vPosition,pVerts[i5].vPosition,pVerts[i2].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i6].vPosition,pVerts[i3].vPosition,pVerts[i2].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i5].vPosition,pVerts[i6].vPosition,pVerts[i3].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i8].vPosition,pVerts[i5].vPosition,pVerts[i4].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i7].vPosition,pVerts[i8].vPosition,pVerts[i5].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i9].vPosition,pVerts[i6].vPosition,pVerts[i5].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i8].vPosition,pVerts[i9].vPosition,pVerts[i6].vPosition);
			iPlaneNum++;

			//CW

			pPlanes[iPlaneNum].FindPlane(pVerts[i3].vPosition,pVerts[i9].vPosition,pVerts[i1].vPosition);
			iPlaneNum++;

			pPlanes[iPlaneNum].FindPlane(pVerts[i9].vPosition,pVerts[i7].vPosition,pVerts[i1].vPosition);
			iPlaneNum++;


			if (DoPointsOnlyChangeInOneDimention(pVerts[i3].vPosition,pVerts[i2].vPosition,pVerts[i1].vPosition))
			{
				//CCW
				pPlanes[iPlaneNum].FindPlane(pVerts[i7].vPosition,pVerts[i4].vPosition,pVerts[i1].vPosition);
				iPlaneNum++;

				//CW
				pPlanes[iPlaneNum].FindPlane(pVerts[i3].vPosition,pVerts[i6].vPosition,pVerts[i9].vPosition);
				iPlaneNum++;

			} else
			{
				//CCW
				pPlanes[iPlaneNum].FindPlane(pVerts[i3].vPosition,pVerts[i2].vPosition,pVerts[i1].vPosition);
				iPlaneNum++;

				//CW
				pPlanes[iPlaneNum].FindPlane(pVerts[i7].vPosition,pVerts[i8].vPosition,pVerts[i9].vPosition);
				iPlaneNum++;
			}*/
/*
		}
	}*/

	BREAKPOINT(iPlanesNum!=iPlaneNum);
	return iPlaneNum;
}

//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// Fill the VertexBuffer array
//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::FillVBArray(DWORD dwGenerate)
{
	char tempTexName[500];

	m_pRenderables = new CRenderable[m_iFacesNum];

	
	for (int i=0;i<m_iFacesNum;i++)
		m_pRenderables[i].SetTransform(NULL);


	/*m_ppVBArray = new IVertexBuffer_Export*[m_iFacesNum];

	for (int i=0;i<m_iFacesNum;i++)
		m_ppVBArray[i] = NULL;*/

	// Init the Vertex Buffer

	m_pVertexBuffer = g_pRendererBack->GenerateVertexBuffer();


	m_pVertexBuffer->AddStream(STREAM_POSITION,CR_FLOAT3);
	m_pVertexBuffer->AddStream(STREAM_TEXCOORD0,CR_FLOAT2);		

	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		m_pVertexBuffer->AddStream(STREAM_TEXCOORD1,CR_FLOAT3);	
		m_pVertexBuffer->AddStream(STREAM_TEXCOORD2,CR_FLOAT3);	
		m_pVertexBuffer->AddStream(STREAM_TEXCOORD3,CR_FLOAT3);	
	}


	m_iTotalVerticesNum_Simple = 0;
	for (int i=0;i<m_iFacesNum;i++)
	{	
		m_iTotalVerticesNum_Simple+=m_pFaces[i].numOfVerts;
	}

	int iBezierTessLevel = 7;

	// Calculate how many vertices/indices are required to hold the bezier surfaces

	int iBezierVertsNum_Total = 0;
	int iBezierIndicesNum_Total = 0;
	for (int i=0;i<m_iFacesNum;i++)
	{
		if (m_pFaces[i].type!=Q3FACE_BEZIER_PATCH)
			continue;

		iBezierVertsNum_Total += (m_pFaces[i].size[0]/2)*(m_pFaces[i].size[1]/2)*(iBezierTessLevel+1)*(iBezierTessLevel+1);
		iBezierIndicesNum_Total += ((iBezierTessLevel+1)*(m_pFaces[i].size[0]/2)-1)*((iBezierTessLevel+1)*(m_pFaces[i].size[1]/2)-1)*6;		
	}


	m_pBigVertexBuffer_TEMP = new CVec3f[m_iTotalVerticesNum_Simple+iBezierVertsNum_Total];
	CVec3f* pBigVertexBuffer_TEMP_travel = &m_pBigVertexBuffer_TEMP[0];

		
	m_pVertexBuffer->AllocateStreams(m_iTotalVerticesNum_Simple+iBezierVertsNum_Total,VB_MEM_GPU|VB_SINGLE,CR_STREAM_STATIC);

	//m_pVertexBuffer->Lock(CR_LOCK_READ_WRITE);	
	m_pVertexBuffer->Lock(CR_LOCK_WRITE_ONLY);	

	CVec3f* pVertices = (CVec3f*) m_pVertexBuffer->GetStream(STREAM_POSITION);	
	CVec2f* pTexCoords = (CVec2f*) m_pVertexBuffer->GetStream(STREAM_TEXCOORD0);

	//CVec3f* pVertTravel = pVertices;
	CVec2f* pTCTravel = pTexCoords;


	int uiShaderPassLoc=0;
	int uiShaderLoc=0;


	g_pLogFile->OutPutPlainText("Convertion to Vertex Buffers:","3","000000",false,LOG_MESSAGE_INFORMATIVE);
	



	
	//m_pVertexBuffer_Bezier->AllocateStreams(iBezierVertsNum_Total,VB_MEM_GPU|VB_SINGLE,CR_STREAM_STATIC);

	int iCurrentVert_Bezier = m_iTotalVerticesNum_Simple;

	//m_pVertexBuffer_Bezier->Lock(CR_LOCK_WRITE_ONLY);	


	//CVec3f* pVertices_Bezier = (CVec3f*) m_pVertexBuffer_Bezier->GetStream(STREAM_POSITION);	
	//CVec2f* pTexCoords_Bezier = (CVec2f*) m_pVertexBuffer_Bezier->GetStream(STREAM_TEXCOORD0);

	CVec3f* pVertices_Bezier = & pVertices[m_iTotalVerticesNum_Simple];
	CVec2f* pTexCoords_Bezier =  & pTexCoords[m_iTotalVerticesNum_Simple];


	CVec3f* pVertices_Bezier_Travel		= pVertices_Bezier;
	CVec2f* pTexCoords_Bezier_Travel	= pTexCoords_Bezier;



	//////////////////////////////////////////////////////////////////////////////////
	// Create a dynamic index buffer which we'll update in order to draw in batches

	//m_pIndexBuffer_Dynamic = g_pRendererBack->GenerateIndexBuffer();
	//m_pIndexBuffer_Dynamic->Allocate(m_iIndicesNum+iBezierIndicesNum_Total,IB_MEM_GPU | IB_DYNAMIC);
	



	/////////////////////////////////////////////////////
	// Handle Indices

	m_pIndexBuffer_Bezier = g_pRendererBack->GenerateIndexBuffer();


	m_pIndicesData_Bezier = new UINT[iBezierIndicesNum_Total];
		
	UINT*	pIndices_Bezier = m_pIndicesData_Bezier;//(UINT*) m_pIndexBuffer_Bezier->GetData();
	UINT*	pIndices_Bezier_Travel = pIndices_Bezier;
	


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	g_pLogFile->OutPutPlainText("Convertion to Vertex Buffers:","3","000000",false,LOG_MESSAGE_INFORMATIVE);

	// Go through all the faces
	for (int i=0;i<m_iFacesNum;i++)
	{	
		//////////////////////////////////////////////////////////////////////
		tBSPTexture* pTex = &m_pTextures[m_pFaces[i].textureID];
		if (!g_pRendererFront->GetRoughFallBack())
			sprintf(tempTexName,"@%s",pTex->strName);
		else
			sprintf(tempTexName,"#%s",pTex->strName);

		if (strcmp("textures/skies/blacksky",pTex->strName)==0)
			m_pRenderables[i].SetDontRender(true);

		if (strcmp("textures/skies/tim_hell",pTex->strName)==0)
			m_pRenderables[i].SetDontRender(true);

		if (strcmp("models/mapobjects/teleporter/energy",pTex->strName)==0)
			m_pRenderables[i].SetDontRender(true);

		if (strcmp("noshader",pTex->strName)==0)
			m_pRenderables[i].SetDontRender(true);
					

		//////////////////////////////////////////////////////////////////////

		// i need to think what to do with faces with no vertices
		if (m_pFaces[i].numOfVerts==0)
		{
			m_pRenderables[i].SetDontRender(true);
			continue;
		}


		/*// debugging
		for (int blah=0;blah<m_iTexturesNum;blah++)
		{
			char* pTemp = m_pTextures[blah].strName;
		}*/

		/*if (strcmp("textures/skies/blacksky",m_pTextures[m_pFaces[i].textureID].strName)==0)
		{
			continue;
		}*/


		//m_ppVBArray[i] = g_pRendererBack->GenerateVertexBuffer();

		if (m_pFaces[i].type == Q3FACE_BEZIER_PATCH)
		{

			int iBezierVertsNum		= (m_pFaces[i].size[0]/2)*(m_pFaces[i].size[1]/2)*(iBezierTessLevel+1)*(iBezierTessLevel+1);
			int iBezierIndicesNum	= ((iBezierTessLevel+1)*(m_pFaces[i].size[0]/2)-1)*((iBezierTessLevel+1)*(m_pFaces[i].size[1]/2)-1)*6;		

			// create and fill the control points

			CVec3f* pControlPointsVerts	= new CVec3f[m_pFaces[i].numOfVerts];
			CVec2f* pControlPointsTCs	= new CVec2f[m_pFaces[i].numOfVerts];

			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
			{
				memcpy(&pControlPointsVerts[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vPosition,sizeof(CVec3f));
				memcpy(&pControlPointsTCs[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vTextureCoord,sizeof(CVec2f));
			}
				
			//TODO: add m_pVerts[m_pFaces[i].startVertIndex+j].vNormal,lmc,color etc.
			
			EvaluateQuadricBezierPatch(pControlPointsVerts,NULL,pControlPointsTCs,NULL,
				iBezierTessLevel,iBezierTessLevel,
				pVertices_Bezier_Travel,NULL,pTexCoords_Bezier_Travel,NULL,
				m_pFaces[i].size[0],m_pFaces[i].size[1],
				pIndices_Bezier_Travel,
				iCurrentVert_Bezier);
								
			delete[] pControlPointsVerts;
			delete[] pControlPointsTCs;
				
			/////////////////////////////////////////////////////
			// Handle Vertices

			m_pRenderables[i].SetVertexBuffer(m_pVertexBuffer,false);
			m_pRenderables[i].SetIndexBuffer(m_pIndexBuffer_Bezier,false);
	

			//TODO update minIndex
			m_pRenderables[i].SetDrawCallParameters(pIndices_Bezier_Travel-pIndices_Bezier,iBezierIndicesNum,0,iCurrentVert_Bezier,iBezierVertsNum);			
			m_pRenderables[i].SetIndices_Data(m_pIndicesData_Bezier);


			iCurrentVert_Bezier+=		iBezierVertsNum;
			pVertices_Bezier_Travel+=	iBezierVertsNum;
			pTexCoords_Bezier_Travel+=	iBezierVertsNum;

			pIndices_Bezier_Travel += iBezierIndicesNum;

		}
		else // Q3FACE_SIMPLE_POLYGON (billboards aren't properly handled yet)
		{
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
			{
				//memcpy(&pVertTravel[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vPosition,
				memcpy(&pBigVertexBuffer_TEMP_travel[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vPosition,sizeof(CVec3f));
			}

			//pVertTravel+=m_pFaces[i].numOfVerts;
			
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTCTravel[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vTextureCoord,
				sizeof(CVec2f));

			
			m_pRenderables[i].SetVertexBuffer(m_pVertexBuffer,false);
			m_pRenderables[i].SetIndexBuffer(m_pIndexBuffer,false);

			m_pRenderables[i].SetDrawCallParameters(m_pFaces[i].meshVertIndex,m_pFaces[i].numMeshVerts,0,0,m_iTotalVerticesNum_Simple);
			m_pRenderables[i].SetIndices_Data(m_pIndicesData);

			//m_pRenderables[i].SetIndices_Data()
		}
				
	
		pBigVertexBuffer_TEMP_travel+=m_pFaces[i].numOfVerts;
		pTCTravel+=m_pFaces[i].numOfVerts;
		
		// compute the BBox

		if (m_pFaces[i].numMeshVerts > 0)
		{
			//BREAKPOINT(i==140);

			int iIndicesStart = m_pRenderables[i].GetIndicesStart();
			int iIndicesCount = m_pRenderables[i].GetIndicesCount();


			BBox bbox = ComputeBBox(m_pBigVertexBuffer_TEMP,&m_pIndicesData[iIndicesStart],iIndicesCount);
			m_pRenderables[i].SetBBox(bbox);
		}
		
		pTex = &m_pTextures[m_pFaces[i].textureID];

		CSmartPointer<IMaterial_Export> pMaterial = NULL;
	
		if (strcmp(pTex->strName,"models/mapobjects/teleporter/energy")==0)
		//if (strstr(pTex->strName,"teleporter")!=0)
		{
			sprintf(tempTexName,"Materials/PP/HeatHaze.mat");
			m_pRenderables[i].SetShadowCaster(false);
		}
		else
		{
			/*if (!g_pRendererFront->GetRoughFallBack())
			{

				sprintf(tempTexName,"@%s",pTex->strName); 
				
			}
			else*/
			{
				//sprintf(tempTexName,"#%s",pTex->strName); //drop down to just image (based on q3a material name)

				//if (strstr(pTex->strName,"gothic"))	 //debugging		
				//if (strstr(pTex->strName,"models/mapobjects/baph/bapholamp_fx"))	 //debugging		
				//if (pTex->strName[9] == 'p')
				//textures/base_wall/c
				//if (1) // For debugging - Helps isolate specific shaders for analysis
				{
					CQ3A_Shader* pQ3AShader = st_Q3A_ShaderManager->Load(pTex->strName);
					bool bScripted = false;
					pMaterial = g_pRendererBack->LoadMaterialFromQ3AShader(pQ3AShader->GetData(),bScripted);
					
					if (!g_pRendererFront->GetRoughFallBack() && !pQ3AShader->IsScripted())
					{
						pMaterial = NULL;
						sprintf(tempTexName,"@%s",pTex->strName); 
					}
					//m_pRenderables[i].SetMaterial(pMaterial);
				} /*else
				{
					char green_str[100];
					sprintf_s(green_str,100,"ceData/Error.bmp");
					//debugging
					CQ3A_Shader* pQ3AShader = st_Q3A_ShaderManager->Load(green_str);

					pMaterial = g_pRendererBack->LoadMaterialFromQ3AShader(pQ3AShader->GetData());
					m_pRenderables[i].SetMaterial(pMaterial);
				}*/

			}

		}

		if (pMaterial == NULL)
			pMaterial = g_pRendererBack->LoadMaterial(tempTexName);

		m_pRenderables[i].SetMaterial(pMaterial);

		
		//BREAKPOINT(strstr(tempTexName,"models/mapobjects/spotlamp/beam")!=0);

		

		


		//m_pRenderables[i].SetMaterial(g_pRendererBack->LoadMaterial("Materials/green.utm"));		
		//m_pRenderables[i].SetMaterial(g_pRendererBack->LoadMaterial("Materials/hellknight.utm"));
		
	} //loop on all faces



	memcpy( pVertices,m_pBigVertexBuffer_TEMP,sizeof(CVec3f) * m_iTotalVerticesNum_Simple);

	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		CVec3f* pTangents = (CVec3f*) m_pVertexBuffer->GetStream(STREAM_TEXCOORD1);
		CVec3f* pBinormals = (CVec3f*) m_pVertexBuffer->GetStream(STREAM_TEXCOORD2);
		CVec3f* pNormals = (CVec3f*) m_pVertexBuffer->GetStream(STREAM_TEXCOORD3);


		CVec3f* pTangents_Bezier	= &pTangents[m_iTotalVerticesNum_Simple];
		CVec3f* pBinormals_Bezier	= &pBinormals[m_iTotalVerticesNum_Simple];
		CVec3f* pNormals_Bezier		= &pNormals[m_iTotalVerticesNum_Simple];

		// RESTORE
		/*
		m_pVertexBuffer->CalculateTBN(m_iIndicesNum,
			m_pIndicesData,
			0,m_iTotalVerticesNum,
			pVertices,pTexCoords,pNormals,pTangents,pBinormals);	
			*/

		if (m_bPrebatching)
		{

			int iIndicesNum = 0;

			for (int iCurrTexID=0;iCurrTexID<m_iTexturesNum;iCurrTexID++)
				if (m_ppIndexBuffersOfSharedMaterials[iCurrTexID])
					iIndicesNum+=m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->GetLength();
				

			UINT* puiAllIndices = new UINT[iIndicesNum];
			UINT* puiAllIndicesTRAVEL = puiAllIndices;

			for (int iCurrTexID=0;iCurrTexID<m_iTexturesNum;iCurrTexID++)
				if (m_ppIndexBuffersOfSharedMaterials[iCurrTexID])
				{						
					
					/*m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Lock(CR_LOCK_READ_ONLY);
					UINT* puiIndices = (UINT*) m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->GetData();*/

					UINT* puiIndices = (UINT*) m_pSharedMaterialIndices_TEMP[iCurrTexID]; 

					memcpy(puiAllIndicesTRAVEL,puiIndices,sizeof(UINT) * m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->GetLength());	

					puiAllIndicesTRAVEL+=m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->GetLength();

					//m_ppIndexBuffersOfSharedMaterials[iCurrTexID]->Unlock();
				}
		}


		m_pVertexBuffer->CalculateTBN(m_iIndicesNum,
			m_pIndicesData,
			0,m_iTotalVerticesNum_Simple,
			pVertices,pTexCoords,
			pNormals,pTangents,pBinormals);


		m_pVertexBuffer->CalculateTBN(iBezierIndicesNum_Total,
			pIndices_Bezier,
			m_iTotalVerticesNum_Simple,iBezierVertsNum_Total,
			//pVertices_Bezier,pTexCoords_Bezier,
			//pNormals_Bezier,pTangents_Bezier,pBinormals_Bezier);
			pVertices,pTexCoords,
			pNormals,pTangents,pBinormals);


		//SafeDeleteArray(puiAllIndices);
	}


	// Bezier Index Buffer

	if (iBezierIndicesNum_Total > 0)
	{
		m_pIndexBuffer_Bezier->Allocate(iBezierIndicesNum_Total,IB_MEM_GPU | IB_STATIC);

		m_pIndexBuffer_Bezier->Lock(CR_LOCK_WRITE_ONLY,0,0);
		UINT* pIndicesLocked_Bezier = (UINT*) (UINT*) m_pIndexBuffer_Bezier->GetData();
		memcpy(pIndicesLocked_Bezier,m_pIndicesData_Bezier,sizeof(UINT)*iBezierIndicesNum_Total);
		m_pIndexBuffer_Bezier->Unlock();


		m_pVertexBuffer->Unlock();

	}
		
	////////////////////////////////////////////////////////////////////
	// generate the shared material renderables

	if (m_bPrebatching)
	{

		m_pSharedMaterialRenderables = new CRenderable[/*m_iFacesNum*/m_iTexturesNum];

		
		for (int i=0;i<m_iTexturesNum;i++)
		{
			tBSPTexture* pTex = &m_pTextures[i];

			if (strcmp("textures/skies/blacksky",pTex->strName)==0)
				continue;

			if (strcmp("textures/skies/tim_hell",pTex->strName)==0)
				continue;

			if (strcmp("models/mapobjects/teleporter/energy",pTex->strName)==0)
				continue;
			

			

			if (strcmp("noshader",pTex->strName)==0)
				continue;



			m_pSharedMaterialRenderables[i].SetTransform(NULL);
			m_pSharedMaterialRenderables[i].SetVertexBuffer(m_pVertexBuffer,false);
			m_pSharedMaterialRenderables[i].SetIndexBuffer(m_ppIndexBuffersOfSharedMaterials[i],false);
							

			// compute BBox
			if (m_pSharedMaterialIndices_TEMP[i])
			{
				BBox bbox = ComputeBBox(m_pBigVertexBuffer_TEMP,m_pSharedMaterialIndices_TEMP[i],m_ppIndexBuffersOfSharedMaterials[i]->GetLength());

				m_pSharedMaterialRenderables[i].SetBBox(bbox);

			}


			if (strcmp(pTex->strName,"models/mapobjects/teleporter/energy")==0)
			{
				sprintf(tempTexName,"Materials/PP/HeatHaze.mat");
				m_pSharedMaterialRenderables[i].SetShadowCaster(false);
			}
			else
			{
			
			if (!g_pRendererFront->GetRoughFallBack())
				sprintf(tempTexName,"@%s",pTex->strName);
			else
				sprintf(tempTexName,"#%s",pTex->strName);
			}

			// DEBUG DEBUG
			//sprintf(tempTexName,"@%s","Textures/sfx/launchpad_diamond");
			//sprintf(tempTexName,"@%s","Textures/skin/surface8");

			//BREAKPOINT(strstr(tempTexName,"models/mapobjects/spotlamp/beam")!=0);
			
			m_pSharedMaterialRenderables[i].SetMaterial(g_pRendererBack->LoadMaterial(tempTexName));
		}
	}

	////////////////////////////////////////////////////////////////////






	g_pLogFile->OutPutPlainText("Successful.","3","009900",true,LOG_MESSAGE_INFORMATIVE);

	m_FacesDrawn.Resize(m_iFacesNum);


	
	
}



CVec3f  CQuake3BSP::ClipVelocity (const CVec3f& in, const CVec3f& normal, float overbounce)
{
	//#define	STOP_EPSILON	0.002f

	float	backoff;
	float	change;
	int		i;
	CVec3f out;
	
	backoff = Dot (in, normal) * overbounce;

	for (i=0 ; i<3 ; i++)
	{
		change = normal.v[i]*backoff;
		out.v[i] = in.v[i] - change;
		//if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		//	out[i] = 0;
	}
	//#undef STOP_EPSILON

	return out;
}

DWORD CQuake3BSP::CollideAndResponse_helper(CEntity* pEntity, int iDepth)
{
	//if i am passive (for example, if i'm a collectable or jumppad) no need to test
	if (!pEntity->GetPhysicsModel()->IsActive())
		return 0;

	DWORD dwSurfacesHit = 0;

	IPhysicsModel* pPM = pEntity->GetPhysicsModel();
	CVec3f vDesiredMove = pPM->GetDesiredPos() - pPM->GetPreviousTimeStepPos();

	if (!iDepth)
		m_iCurrentCollidedPlanes=0;

	if (iDepth>4)
		return dwSurfacesHit;

	float fMoveTillHit_Min = 1.f;
	CVec3f vHitNormal_Min;


	float fMoveTillHit_Current;
	CVec3f vHitNormal_Current;
	CEntity* pFirstHitEntity = NULL;


	CVec3f vStart,vEnd;

	vStart = pEntity->GetPhysicsModel()->GetPreviousTimeStepPos();
	vEnd = pEntity->GetPhysicsModel()->GetDesiredPos();

	TraceEntity(TRIGGER_TOUCH_TWO_SIDES,vStart,vEnd,pEntity,fMoveTillHit_Min,vHitNormal_Min,pFirstHitEntity);

	// we didn't hit anything
	if (fMoveTillHit_Min>=1.f)
	{
		pPM->SetCurrentTimeStepPos(pPM->GetDesiredPos()); // make a move into the desired position
		return dwSurfacesHit;
	}

		

	m_CollidedPlanesNormals[m_iCurrentCollidedPlanes++] = vHitNormal_Min;




	// if we're here, then we must have hit something.

	
	/*// restore restore restore
	pFirstHitEntity->TouchedBy(pEntity); // touch the first entity i collided with
	pEntity->TouchedBy(pFirstHitEntity); // touch the first entity i collided with*/


	if (fMoveTillHit_Min == 0.f) // we're completely stuck
	{
		//BREAKPOINT(1);

		/*g_pGraphicsAPI->startPrinting();
		g_pGraphicsAPI->AddDebugText(100,100,"T=0 !!!!\n");
		g_pGraphicsAPI->AddDebugText(100,100,"T=0 !!!!\n");
		g_pGraphicsAPI->AddDebugText(100,100,"T=0 !!!!\n");		
		g_pGraphicsAPI->stopPrinting();*/
		

		CVec3f vCurrenedAchievedPosition = pPM->GetPreviousTimeStepPos() + vHitNormal_Min*0.1f;
	
		pPM->SetCurrentTimeStepPos(vCurrenedAchievedPosition);

		return dwSurfacesHit;

	}

	// here update the new position and perform a recursive call
	
	// debug debug
/*	if (dwSurfacesHit&SURF_HIT_JUMPPAD)
		return dwSurfacesHit;*/
	
	// debug debug debug
	/*if (fMoveTillHit_Min>0.00001f)
		fMoveTillHit_Min-=0.00001f;*/


	//CVec3 newPos=info.vStart+info.t*info.vVel+info.vNormal*0.0006f;


	// make the move and add a little push AWAY from surfaces we've hit, using the normal collected
	CVec3f vCurrenedAchievedPosition = pPM->GetPreviousTimeStepPos()+ (vDesiredMove*fMoveTillHit_Min) + (vHitNormal_Min*0.01f);
	
	pPM->SetCurrentTimeStepPos(vCurrenedAchievedPosition);





	// response
	if (pPM->IsSlider())
	{
		// ORIGINAL
		/*CVec3f newVel = ClipVelocity(  (pPM->GetDesiredPos() - vCurrenedAchievedPosition),
									vHitNormal_Min,1.02f);*/

		CVec3f newVel;
		CVec3f newVel2;
		int p;
		for (p=0;p<m_iCurrentCollidedPlanes;p++)
		{            
			//newVel2=ClipVelocity((pPM->GetDesiredPos() - vCurrenedAchievedPosition),m_CollidedPlanesNormals[p],1.02f);

			newVel2=ClipVelocity((pPM->GetDesiredPos() - vCurrenedAchievedPosition),m_CollidedPlanesNormals[p],1.1f);

			int j;
			for (j=0; j<m_iCurrentCollidedPlanes; j++)
			{
				if (j != p)
				{
					if (Dot(newVel2, m_CollidedPlanesNormals[j]*-1.f) < 0)
						break;	// not ok
				}
			}
			if (j == m_iCurrentCollidedPlanes)
				break;
		}
		if (p != m_iCurrentCollidedPlanes)
		{	// go along this plane
			newVel=newVel2;
		}
		else
		{	// go along the crease
			if (m_iCurrentCollidedPlanes != 2)
			{
				//return false;
				newVel = CVec3f(0.f,0.f,0.f);
			}
			else
			{
				CVec3f dir=Cross (m_CollidedPlanesNormals[0], m_CollidedPlanesNormals[1]);
				float d =  Dot (dir, (pPM->GetDesiredPos() - vCurrenedAchievedPosition));
				newVel=dir*d;
			}
		}


	
	
		// current becomes previous
		pPM->SetPreviousTimeStepPos(pPM->GetCurrentTimeStepPos());

		//pPM->SetCurrentTimeStepPos(vCurrenedAchievedPosition + vNewVec);
		pPM->SetDesiredPos(vCurrenedAchievedPosition + newVel);
	
		CollideAndResponse_helper(pEntity,iDepth+1);
	}

	return dwSurfacesHit;

}

DWORD CQuake3BSP::CollideAndResponse(CEntity* pEntity)
{		
	DWORD dwSurfaceHit = 0;
	
	IPhysicsModel* pPM = pEntity->GetPhysicsModel();
	CVec3f vOriginalMove = pPM->GetDesiredPos()-pPM->GetCurrentTimeStepPos();

	CVec3f vOriginalStart = pPM->GetCurrentTimeStepPos();

	if (pPM->IsSlider())
		if ((pPM->GetDesiredPos()-pPM->GetPreviousTimeStepPos()).Length()>0.5f)
		if (IsFacingStair(pPM->GetPreviousTimeStepPos(),pPM->GetDesiredPos()-pPM->GetPreviousTimeStepPos(),pEntity->GetPhysicsModel()))
		{
			// we're facing a stair, so we're helping to lift the entity off the ground a bit
			//pPM->SetDesiredPos(pPM->GetDesiredPos()+CVec3f(0.f,1.f,0.f));
			//g_pGraphicsAPI->startPrinting();
			//g_pGraphicsAPI->AddDebugText(100,100,"Facing STAIR!!!!!\n");
			//g_pGraphicsAPI->stopPrinting();

			// move up, direction, downtbh 
			

			/////////////////////////////////////////////////////////////////////////////////
			// move up
			//pPM->SetPreviousTimeStepPos( ...) <- no need -  first move
			pPM->SetDesiredPos(pPM->GetPreviousTimeStepPos()+CVec3f(0.f,STAIR_MOVE_Y,0.f));
			dwSurfaceHit!= CollideAndResponse_helper(pEntity,0);
			/////////////////////////////////////////////////////////////////////////////////

			

			/////////////////////////////////////////////////////////////////////////////////
			// move original move			
			pPM->SetPreviousTimeStepPos(pPM->GetCurrentTimeStepPos()); // current becomes previous

			pPM->SetDesiredPos(pPM->GetPreviousTimeStepPos()+vOriginalMove);
			dwSurfaceHit!= CollideAndResponse_helper(pEntity,0);
			/////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////
			// move back down		
			pPM->SetPreviousTimeStepPos(pPM->GetCurrentTimeStepPos()); // current becomes previous

			pPM->SetDesiredPos(pPM->GetPreviousTimeStepPos()+CVec3f(0.f,-STAIR_MOVE_Y,0.f));
			dwSurfaceHit!= CollideAndResponse_helper(pEntity,0);
			/////////////////////////////////////////////////////////////////////////////////



			pPM->SetPreviousTimeStepPos(vOriginalStart); // so that the blending between physics frame won't blend the last drop-down move

			
			return dwSurfaceHit;
		}

		dwSurfaceHit!= CollideAndResponse_helper(pEntity,0);

	
		return dwSurfaceHit;
	

}


void CQuake3BSP::TraceEntity(DWORD dwTriggerTouch,const CVec3f& vStart, const CVec3f& vEnd, CEntity* pMe, float& fMoveTillHit_Min, CVec3f& vHitNormal_Min, CEntity*& pFirstHitEntity)
{
	// debug
	//return 1.f;

	// set start value
	vHitNormal_Min.Set(0.f,1.f,0.f);


	fMoveTillHit_Min = 1.f;
	
	float fMoveTillHit_Current;
	CVec3f vHitNormal_Current;
	pFirstHitEntity = NULL;

	bool bHit=false;

	CEntity* pOtherEntity = NULL;


	if (pMe->IgnoreMeInPhysics())
	{
		BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );
		return;
	}

	for (ENTITIES_MAP_IR it=m_LevelEntities.rbegin();it!=m_LevelEntities.rend();++it)
			{
				pOtherEntity = (*it).second;

				if (pOtherEntity->GetID() == pMe->GetID()) // don't collide against self
					continue;

				if (pOtherEntity->IgnoreMeInPhysics())
					continue;


				/*vStart = pMe->GetPhysicsModel()->GetPosition();
				vEnd   = pMe->GetPhysicsModel()->GetDesiredPos();*/

				bHit = pOtherEntity->GetPhysicsModel()->CheckHitBy(pMe->GetPhysicsModel(),vStart,vEnd,fMoveTillHit_Current,vHitNormal_Current);
				BREAKPOINT(   *((unsigned long*)&vHitNormal_Current.x) == 0xffc00000 );

				if (pOtherEntity->GetPhysicsModel()->IsTriggerOnly() && bHit)
				{
					if (dwTriggerTouch==TRIGGER_TOUCH_ONE_SIDE || dwTriggerTouch==TRIGGER_TOUCH_TWO_SIDES)
						pOtherEntity->TouchedBy(pMe); 			

					if (dwTriggerTouch==TRIGGER_TOUCH_TWO_SIDES )
						pMe->TouchedBy(pOtherEntity);
				}

				if (fMoveTillHit_Current < fMoveTillHit_Min)
				{	
					fMoveTillHit_Min = fMoveTillHit_Current;
					vHitNormal_Min   = vHitNormal_Current;
					BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );
					pFirstHitEntity = pOtherEntity;
				}
			}


	if (!pFirstHitEntity)
	{
		BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );
		return;
	}


	BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );

	if (dwTriggerTouch==TRIGGER_TOUCH_ONE_SIDE || dwTriggerTouch==TRIGGER_TOUCH_TWO_SIDES)
		pFirstHitEntity->TouchedBy(pMe); // touch the first entity i collided with

	BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );

	if (dwTriggerTouch==TRIGGER_TOUCH_TWO_SIDES )
		// touch the first entity i collided with
		if (pMe->TouchedBy(pFirstHitEntity)) 
			// if request to ignore:
		{
			vHitNormal_Min.Set(0.f,1.f,0.f);
			fMoveTillHit_Min = 1.f;
			pOtherEntity = NULL;
			return;
		}

	BREAKPOINT(   *((unsigned long*)&vHitNormal_Min.x) == 0xffc00000 );

	//m_pPhysicsModel->TraceMove(&trace);

	//return trace.fFraction;

}

// because of a problem in the swept AABB test,
// i'm using only testing down 100 units!

float CQuake3BSP::GetDistanceToFloor(const CVec3f& vPos,CEntity* pEntity)
{
	// debug
	//return 1.f;


	//CVec3f vAdd(0.f,40.f,0.f);

	CVec3f vStart = vPos;
	//CVec3f vEnd   = vPos+CVec3f(0.f,-2000.f,0.f);
	CVec3f vEnd   = vPos+CVec3f(0.f,-100.f,0.f);

	float fT;
	CVec3f vHitNormal;
	CEntity* pHitEntity = NULL;

	TraceEntity(TRIGGER_TOUCH_NONE,vStart,vEnd,pEntity,fT,vHitNormal,pHitEntity);


	return ((vEnd-vStart)*fT).Length();
}

bool CQuake3BSP::IsFacingStair(const CVec3f& vPos, const CVec3f& vMove,IPhysicsModel* pPM)
{
	CVec3f vWithoutY = vMove;
	vWithoutY.y=0.f;

	if (vWithoutY.Length()<0.01f)
		return false;

	tTrace trace;

	trace.vBBoxRadius.x = max(abs(pPM->m_BBox.min.x),abs(pPM->m_BBox.max.x));
	trace.vBBoxRadius.y = max(abs(pPM->m_BBox.min.y),abs(pPM->m_BBox.max.y));
	trace.vBBoxRadius.z = max(abs(pPM->m_BBox.min.z),abs(pPM->m_BBox.max.z));
	trace.bAllSolid = false;
	trace.fFraction = 1.f;

	trace.vRayStart = vPos;
	trace.vRayEnd   = vPos + vMove;

	m_pPhysicsModel->TraceMove(&trace);
	
	CVec3f vFinalPositionWithoutAid = trace.vRayStart + (trace.vRayEnd - trace.vRayStart) * trace.fFraction;

	CVec3f vFinalMoveWithoutAid = vFinalPositionWithoutAid - vPos;

	/////////////////////////////////////////// Stair test



	/////////////////////////////////// Move Up

	float fUpMove = STAIR_MOVE_Y;


	trace.vRayStart = vPos;
	trace.vRayEnd   = vPos + CVec3f(0.f,fUpMove,0.f);
	

	trace.bAllSolid = false;
	trace.fFraction = 1.f;

	m_pPhysicsModel->TraceMove(&trace);

	if (trace.fFraction<1.f) // if move up encountered intersection, quickly return false for now
		return false;

	/////////////////////////////////// Move towards original move

	trace.vRayStart = trace.vRayEnd;
	trace.vRayEnd = trace.vRayEnd + vMove;
	trace.bAllSolid = false;
	trace.fFraction = 1.f;
	m_pPhysicsModel->TraceMove(&trace);

	if (trace.fFraction<1.f) // if towards original direction encountered intersection, quickly return false for now
		return false;

	/////////////////////////////////// Move towards original move

	trace.vRayStart = trace.vRayEnd;
	trace.vRayEnd = trace.vRayEnd + CVec3f(0.f,-fUpMove,0.f);
	trace.bAllSolid = false;
	trace.fFraction = 1.f;
	m_pPhysicsModel->TraceMove(&trace);

	CVec3f vPosWithAid = trace.vRayStart + (trace.vRayEnd - trace.vRayStart) * trace.fFraction;

	CVec3f vMoveWithAid = vPosWithAid - vPos;


	if (vMoveWithAid.Length() > vFinalMoveWithoutAid.Length() + 0.1f)
		return true;

	return false;
}

void CQuake3BSP::UpdateLevelEntitiesPhysics(float fDeltaTime)
{
	static float fAccumulator = 0.0f;

	const float fTimeStepSize = (1.f/ (float) PHYSICS_FRAMES_NUM)*1000.f;

	fAccumulator+= fDeltaTime;

	bool bOnlyBlending = true;

	//BREAKPOINT(fAccumulator<fTimeStepSize);

	//g_pGraphicsAPI->AddDebugText(100,100,"Accumulator:%f\n",fAccumulator);
	//g_pGraphicsAPI->AddDebugText(100,100,"DeltaTime:%f\n",fDeltaTime);

	
	CEntity* pEntity;

	
	CVec3f vOldPos,vNewPos;
	
	while (fAccumulator>=fTimeStepSize)
    {
		bOnlyBlending = false;
		
			for (ENTITIES_MAP_IR it=m_LevelEntities.rbegin();it!=m_LevelEntities.rend();++it)
			{
				pEntity = (*it).second;

				

				
				vOldPos = pEntity->GetPhysicsModel()->GetPreviousTimeStepPos();

				pEntity->FindPhysicsStepDesiredPosition(fTimeStepSize*0.001f);


				// store previous step position
				// because the recursive response process might alter things 
				CVec3f vPreviousStep_stored = pEntity->GetPhysicsModel()->GetPreviousTimeStepPos();

				int iCARRes = CollideAndResponse(pEntity);		

				

				vNewPos = pEntity->GetPhysicsModel()->GetCurrentTimeStepPos();
				pEntity->GetPhysicsModel()->SetPreviousTimeStepPos(vPreviousStep_stored);

				
				/*if (iCARRes == SURF_HIT_JUMPPAD)
				{
					// redo this 
					it--;
					continue;
				}*/

				pEntity->GetPhysicsModel()->ChangeVelocityBasedOnActualMovement(vNewPos,vOldPos,fTimeStepSize*0.001f);
			}


			// remove all entities requesting a clean-up

			for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();)
			{
				if (it->second->GetCleanMe())
				{
					UnLinkEntityFromLeafs(it->second);
					SafeDelete(it->second);
					it = m_LevelEntities.erase(it);

					
					continue;					
				}

				++it;
			}

			fAccumulator -= fTimeStepSize;
	}

	float fAlpha = fAccumulator / fTimeStepSize;
	
	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
		{
			pEntity = (*it).second;
			pEntity->BlendPhysicsState(fAlpha);
			
			/*if (pEntity->GetID()==g_uiControlledHumanPlayer)
				{
					g_pGraphicsAPI->startPrinting();

					if (g_bRenderableRequirePostProcessing)
						g_pGraphicsAPI->AddDebugText("FOUND renderables requiring post processing!\n");
					else
						g_pGraphicsAPI->AddDebugText("DIDN'T find renderables requiring post processing.\n");


					if (g_bDeadReckoning)
						g_pGraphicsAPI->AddDebugText("Dead Reckoning ON\n");
					else
						g_pGraphicsAPI->AddDebugText("Dead Reckoning OFF\n");

					g_pGraphicsAPI->AddDebugText("Score: %i\n",g_pServer->GetPlayerStateFromFinalVector(pEntity->GetID())->iScore);
						

					g_pGraphicsAPI->AddDebugText("Entity %d\n",g_uiControlledHumanPlayer);


					g_pGraphicsAPI->AddDebugText("g_idbgJumppadAddForceCount = %d\n",g_idbgJumppadAddForceCount);
					

					g_pGraphicsAPI->AddDebugText("Life: %d\n",((CControllableEntity*) pEntity)->GetLife());

					g_pGraphicsAPI->AddDebugText("Distance To Floor:%f\n",GetDistanceToFloor(pEntity->GetPhysicsModel()->GetPreviousTimeStepPos(),pEntity));

				

					g_pGraphicsAPI->AddDebugText("Blended Position:%f,%f,%f\n",
						pEntity->GetPhysicsModel()->GetPosition().x,
						pEntity->GetPhysicsModel()->GetPosition().y,
						pEntity->GetPhysicsModel()->GetPosition().z);
						
					g_pGraphicsAPI->AddDebugText("Velocity:%f,%f,%f\n",
						pEntity->GetPhysicsModel()->m_vVelocity.x,
						pEntity->GetPhysicsModel()->m_vVelocity.y,
						pEntity->GetPhysicsModel()->m_vVelocity.z);

					g_pGraphicsAPI->stopPrinting();
				}*/
		}

	/*if (bOnlyBlending)
		g_pGraphicsAPI->AddDebugText(100,100,"only blending...");
	else
		g_pGraphicsAPI->AddDebugText(100,100,"COMPUTING TIME STEP!");*/
	

	//BREAKPOINT(bOnlyBlending);


}

void CQuake3BSP::LinkLevelEntitiesToLights(bool bForceCheckAll)
{
	IRenderModel* pRenderModel = NULL;
	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
	{
		if (!bForceCheckAll && (*it).second->IsStatic())
			continue;

		pRenderModel = (*it).second->GetRenderModel();

		if (!pRenderModel)
			continue;

		pRenderModel->ClearEffectingLights();

		for (int l=0;l<m_iLightsNum;l++)
		{

			if (m_ppLights[l]->GetFrustum()->BoxInFrustum((*it).second->GetPhysicsModel()->m_BBox.min+(*it).second->GetPhysicsModel()->GetPosition(),
														(*it).second->GetPhysicsModel()->m_BBox.max+(*it).second->GetPhysicsModel()->GetPosition()))
			{
				// this light can effect this entity

				pRenderModel->AddEffectingLight(m_ppLights[l]);



			}
		}



	}

}


void CQuake3BSP::LinkRenderablesToLights(void)
{
	// TODO: remove one update if it's not needed!

	// relinking shared materials lights


	
		


	static int iDBG = 0;	

	if (m_bPrebatching)
	{

		for (int i=0;i<m_iTexturesNum;i++)
		{

			m_pSharedMaterialRenderables[i].ClearEffectingLights();
			

			for (int l=0;l<m_iLightsNum;l++)
			{
				if (
					(!m_pSharedMaterialRenderables[i].GetBBox()) ||
					
					/*CheckBBoxSphereIntersection((*m_pSharedMaterialRenderables[i].GetBBox()),
										m_ppLights[l]->GetPos(),
										m_ppLights[l]->GetRadius()))*/
										m_ppLights[l]->GetFrustum()->BoxInFrustum(
										(*m_pSharedMaterialRenderables[i].GetBBox()).min.x,
										(*m_pSharedMaterialRenderables[i].GetBBox()).min.y,
										(*m_pSharedMaterialRenderables[i].GetBBox()).min.z,
										(*m_pSharedMaterialRenderables[i].GetBBox()).max.x,
										(*m_pSharedMaterialRenderables[i].GetBBox()).max.y,
										(*m_pSharedMaterialRenderables[i].GetBBox()).max.z))


					m_pSharedMaterialRenderables[i].AddEffectingLight(m_ppLights[l]);	
				else
				{
					iDBG++;
				}
			}
		}
	}

	
// for prebatch<->batch go here



// relinking faces renderables lights

	for (int i=0;i<m_iFacesNum;i++)
		m_pRenderables[i].ClearEffectingLights();

	static int iDBG2 = 0;

	const BBox* pBBox;

	for (int i=0;i<m_iFacesNum;i++)
	{	
		if (m_pFaces[i].numOfVerts==0)
			continue;

		for (int l=0;l<m_iLightsNum;l++)
		{
			//BBox lightBBox = GetBBoxFromSphere(m_ppLights[l]->GetPos(),m_ppLights[l]->GetRadius());
			
			pBBox = m_pRenderables[i].GetBBox();

			if (!pBBox)
				m_pRenderables[i].AddEffectingLight(m_ppLights[l]);
			else
			if (m_ppLights[l]->GetFrustum()->BoxInFrustum(
									(*m_pRenderables[i].GetBBox()).min.x,
									(*m_pRenderables[i].GetBBox()).min.y,
									(*m_pRenderables[i].GetBBox()).min.z,
									(*m_pRenderables[i].GetBBox()).max.x,
									(*m_pRenderables[i].GetBBox()).max.y,
									(*m_pRenderables[i].GetBBox()).max.z))
				m_pRenderables[i].AddEffectingLight(m_ppLights[l]);
			else
			{
				iDBG2++;

			}
		}
	}

	
}

void CQuake3BSP::UnLinkEntityFromLeafs(CEntity* pEntity)
{

	BREAKPOINT(pEntity->IsControllable());

	//BREAKPOINT(!pEntity->IsProjectile());

	LEAFS_INDICES_MAP* pLeafsIndicesMap = pEntity->GetLinkedLeafsMap();

	int iCurrentIndex = -1;

	ENTITIES_MAP_I ent_it;

	for (LEAFS_INDICES_MAP_I it = pLeafsIndicesMap->begin(); it!=pLeafsIndicesMap->end() ; ++it)
	{
		iCurrentIndex = it->first;



		ent_it = m_pLeafs[iCurrentIndex].m_Entities.begin();
		ent_it = m_pLeafs[iCurrentIndex].m_Entities.find(pEntity->GetID());

		if (ent_it == m_pLeafs[iCurrentIndex].m_Entities.end())
		{
			BREAKPOINT(1); // not found!
			continue;
		}


		m_pLeafs[iCurrentIndex].m_Entities.erase(ent_it);


	}

}

void CQuake3BSP::LinkEntityToLeafs(CEntity* pEntity)
{
	CVec3f vSphereCenter;
	float  fSphereRadius;
	

	GetSphereFromBBox(pEntity->GetPhysicsModel()->m_BBox,vSphereCenter,fSphereRadius);

	// instead of brute force running through all of the leafs, just find immidiatly the colliding ones
	LEAFS_INDICES_VECTOR leafsIndicesVec;
	FindLeafs(leafsIndicesVec,vSphereCenter+pEntity->GetPhysicsModel()->GetPosition(),fSphereRadius);

	int iTemp = 0;

	for (int l=0;l<leafsIndicesVec.size();l++)
	{
		// inform leaf that this entity should be linked to him
		m_pLeafs[leafsIndicesVec[l]].m_Entities[pEntity->GetID()] = pEntity;

		// inform entity that it was linked to this leaf
		(*pEntity->GetLinkedLeafsMap())[leafsIndicesVec[l]] = leafsIndicesVec[l];

		iTemp++;
	}


}

void CQuake3BSP::ReLinkMapEntitiesToLeafs(bool bForceReLinkAll)
{	

	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
	{
		if (!bForceReLinkAll)
		{
			if (it->second->IsStatic())
				continue;

			if (!it->second->RelinkMe())
				continue;
		}

		

			// here
		LinkEntityToLeafs(it->second);

	}

}

CEntity* CQuake3BSP::GetEntity(unsigned int uiID)
{
	ENTITIES_MAP_I it = m_LevelEntities.find(uiID);

	if (it==m_LevelEntities.end())
	{
		BREAKPOINT(1); // entity id not found!
		return NULL;
	}

	return (*it).second;
}

/*void CQuake3BSP::RemoveEntity(DWORD dwEntityID)
{
	// this might cause problems, since erasing while looping

	ENTITIES_MAP_I it = m_LevelEntities.begin();

	it = m_LevelEntities.find(dwEntityID);

	if (it==m_LevelEntities.end())
	{
		BREAKPOINT(1); // not found !
		return;
	}

	delete it->second;

	m_LevelEntities.erase(it);
}*/

DWORD CQuake3BSP::SpawnParticleSystem(CParticleSystem* pPS, CEntity* pParent)
{
	CEntity* pEntity = NULL;

	pEntity = new CParticleSystemEntity();


	if (pParent)
		((CParticleSystemEntity*)pEntity)->SetParentEntity(pParent);

	pEntity->SetRenderModel(pPS);
	
	IPhysicsModel* pPM = new CDummyPhysicsModel();

	
	
	/*pPM->m_BBox.min.Set(-100.f,-100.f,-100.f);
	pPM->m_BBox.max.Set(100.f,100.f,100.f);*/

	pPM->m_BBox.min.Set(-10.f,-10.f,-10.f);
	pPM->m_BBox.max.Set(10.f,10.f,10.f);

	pPM->SetPosition(pPS->GetOrigin());

	pEntity->SetPhysicsModel(pPM);
		

	LinkEntityToEntitiesMap(pEntity);



	// debug
	//ReLinkMapEntitiesToLeafs(true);

	return pEntity->GetID();



}

DWORD CQuake3BSP::SpawnProjectile(DWORD dwProjectileType, const CVec3f& vPos,const CVec3f& vVel,DWORD dwTriggeringEntityID)
{
	CEntity* pEntity = NULL;

	if (g_pServer->m_dwGameType == SERVER_GAME_TYPE_NETWORKED_PURE_CLIENT) // if we're a pure client, we only want visual effects
	{
		pEntity = new CProjectileEntity();
		
	}
	else
	{
		pEntity = new CProjectileEntity_Server();

	}

	switch (dwProjectileType)
	{
	case WEAPON_TYPE_ROCKET:
		((CProjectileEntity*)pEntity)->Init(WEAPON_TYPE_ROCKET,"Models/earth.3ds","Materials/Characters/marine_body.utm",vPos,vVel,dwTriggeringEntityID);
		//((CProjectileEntity*)pEntity)->Init(PROJECTILE_TYPE_ROCKET,"Models/Weapons/rocket.3ds","Materials/Weapons/rocket.utm",vPos,vVel,dwTriggeringEntityID);
		//((CProjectileEntity*)pEntity)->Init(PROJECTILE_TYPE_ROCKET,"Models/Weapons/rocket.3ds","Materials/Characters/marine_body.utm",vPos,vVel,dwTriggeringEntityID);
		
		//st_SoundManager->PlaySound("Sound/rocklf1a.wav",vPos,255);
		st_SoundManager->Load("Sound/weapons/rocket/rocklf1a.wav")->Play(vPos,255);

		break;
	case WEAPON_TYPE_FIREBALL:
		((CProjectileEntity*)pEntity)->Init(WEAPON_TYPE_FIREBALL,"Models/shipping_crates_2.3ds","Materials/Characters/hellknight-tongue.utm",vPos,vVel,dwTriggeringEntityID);

		//st_SoundManager->PlaySound("Sound/rocklf1a.wav",vPos,255);
		st_SoundManager->Load("Sound/weapons/rocket/rocklf1a.wav")->Play(vPos,255);

		break;
	case WEAPON_TYPE_BLUE:
		((CProjectileEntity*)pEntity)->Init(WEAPON_TYPE_BLUE,"Models/earth.3ds","Materials/Characters/marine_body.utm",vPos,vVel,dwTriggeringEntityID);
		//g_SoundAPI.PlaySample3D(st_SoundManager->Load("Sound/blue_launch.wav")->GetSample(),vPos,255);

		//st_SoundManager->PlaySound("Sound/blue_launch.wav",vPos,255);
		st_SoundManager->Load( "sound/weapons/plasma/plasmx1a.wav")->Play(vPos,255);
		break;

	case WEAPON_TYPE_WHITE:
		((CProjectileEntity*)pEntity)->Init(WEAPON_TYPE_WHITE,"Models/earth.3ds","Materials/Characters/marine_body.utm",vPos,vVel,dwTriggeringEntityID);
		//g_SoundAPI.PlaySample3D(st_SoundManager->Load("Sound/blue_launch.wav")->GetSample(),vPos,255);

		//st_SoundManager->PlaySound("Sound/white_weapon.wav",vPos,255);
		st_SoundManager->Load("sound/weapons/lightning/lg_fire.wav")->Play(vPos,255);
		break;	

	default:
		BREAKPOINT(1); // unrecognized spawn projectile!
	break;
	};





	((CProjectileEntity*)pEntity)->UpdateServer();

	LinkEntityToEntitiesMap(pEntity);

	return pEntity->GetID();
}

void CQuake3BSP::LinkEntityToEntitiesMap(CEntity* pEntity)
{
	// link the entity to the entities map
	m_LevelEntities[pEntity->GetID()] = pEntity;
	LinkEntityToLeafs(pEntity);

}

void CQuake3BSP::RespawnPlayer(unsigned int uiPlayerEntityID,int iStartPos)
{
	CVec3f vStartPosition = m_pStartPositions[iStartPos];

	CControllableEntity* pContEnt = (CControllableEntity*) GetEntity(uiPlayerEntityID);
	pContEnt->GetPhysicsModel()->SetPosition(vStartPosition);

	pContEnt->Revive();

}

// must be done AFTER parser parsed the start positions
DWORD CQuake3BSP::AddPlayer(tPlayerGameState* pPlayer)
{
	static int iStartPos = 0;

	
	CEntity* pEntity = NULL;

	//////////////////////////////////////////////////////////////////////////
	
	switch (pPlayer->cControlType)
		{
		case PLAYER_CONTROL_TYPE_HUMAN:
			pEntity = new CHumanPlayerEntity();			
			break;
		case PLAYER_CONTROL_TYPE_BOT:
			pEntity = new CBotEntity();
			break;
		case PLAYER_CONTROL_TYPE_NETWORKED:
			//pEntity = new CControllableEntity();
			//BREAKPOINT(1); // no implementation of networked type yet...
			pEntity = new CNetPlayerEntity;
			break;
		default:
			BREAKPOINT(1); // error! unrecognized player type!
		}

	
	assert(_CrtCheckMemory());

	CVec3f vStartPosition = m_pStartPositions[iStartPos];

	
	((CControllableEntity*) pEntity)->SetSpeed(1.f);
	

	CCharacter* pRM = new CCharacter();		

	if (pPlayer->cControlType == PLAYER_CONTROL_TYPE_HUMAN)
		pRM->SetAnglesBlending(false);
	
		assert(_CrtCheckMemory());
		pRM->SetShared(st_MD5Manager->Load(pPlayer->pCharacterType),pEntity);
		
		pRM->SetScale(CVec3f(1.f,1.f,1.f));
		pRM->SetPosition(vStartPosition);	
		pRM->SetAngles(CVec3f(0.f,m_fStartAngles[iStartPos],0.f));
		pRM->Update(0.f,false);

		pEntity->SetRenderModel(pRM);

	// create a new physics model, and set it
		
		
		CBasicPhysics* pPM = NULL;

		if (pPlayer->cControlType==PLAYER_CONTROL_TYPE_NETWORKED)
			pPM = new CNetControlalblePhysics();
		else
			pPM = new CControllablePhysics();

		//pPM->SetParent(pEntity);
		// TODO - compute real BBOx here!
		//pPM->m_BBox = pRM->GetBBox();
		
		/*pPM->m_BBox.min.Set(-25.f,-40.f,-25.f);
		pPM->m_BBox.max.Set(25.f,40.f,25.f);*/

		pPM->m_BBox.min.Set(-20.f,-40.f,-25.f);
		pPM->m_BBox.max.Set(20.f,40.f,25.f);

		

		pPM->SetPosition(vStartPosition);	
		pEntity->SetPhysicsModel(pPM);

		//((CControllableEntity*) pEntity)->SetSpeed(5.f);


		//m_LevelEntities[pEntity->GetID()] = pEntity;
		LinkEntityToEntitiesMap(pEntity);


		pPlayer->uiEntityID = pEntity->GetID();

		iStartPos++; 

		if (iStartPos>=m_iStartPositionsNum)
			iStartPos = 0;

		ReLinkMapEntitiesToLeafs(false);



		return pEntity->GetID();
}

void CQuake3BSP::UnregisterEntity(unsigned int uiEntityID)
{
	ENTITIES_MAP_I it = m_LevelEntities.find(uiEntityID);

	if (it == m_LevelEntities.end())
	{
		BREAKPOINT(1); // didn't find entity! can't unregister.
		return;
	}

	m_LevelEntities.erase(it);

}

void CQuake3BSP::LoadMapEntities(void)
{
	CEntitiesParser entitiesParser;
	entitiesParser.SetEntitiesString(m_strEntities);
	entitiesParser.Parse();
	
	m_iStartPositionsNum = entitiesParser.FillPlayerPositions(m_pStartPositions,m_fStartAngles);


	// reset the counter, so that entities 1...N will be the N players
	CEntity::ResetIDCounter();

	m_pPhysicsModelEntity = new CEntity();
	m_pPhysicsModelEntity->SetPhysicsModel(m_pPhysicsModel);

	m_LevelEntities[m_pPhysicsModelEntity->GetID()] = m_pPhysicsModelEntity;

	// first of all, load players:
	CEntity* pEntity = NULL;

	/*int iStartPos = 3;
	//////////////////////////////////////////////////////////////////////////
	// player 0
	pEntity = new CControllableEntity();

	((CControllableEntity*) pEntity)->SetSpeed(1.5f);
	//((CControllableEntity*) pEntity)->SetRenderRenderModel(false);

	CCharacter* pRM = new CCharacter();		
		//pRM->SetShared(st_MD5Manager->Load("MD5/vagary/vagary.chr"));		
		//pRM->SetShared(st_MD5Manager->Load("MD5/
		bus/mancubus.chr"));		
		pRM->SetShared(st_MD5Manager->Load("MD5/marine/marine.chr"));
		//pRM->SetShared(st_MD5Manager->Load("MD5/vagary/vagary.chr"));
		//pRM->SetShared(st_MD5Manager->Load("MD5/hellknight/hellknight.chr"));
		
		pRM->SetScale(CVec3f(1.f,1.f,1.f));
		pRM->SetPosition(m_pStartPositions[iStartPos]);	
		pRM->SetAngles(CVec3f(0.f,m_fStartAngles[iStartPos],0.f));
		pRM->Update(0.f);

	pEntity->SetRenderModel(pRM);

	// create a new physics model, and set it
		CBasicPhysics* pPM = new CControllablePhysics();
		//pPM->SetParent(pEntity);
		// TODO - compute real BBOx here!
		//pPM->m_BBox = pRM->GetBBox();
		
		pPM->m_BBox.min.Set(-25.f,-40.f,-25.f);
		pPM->m_BBox.max.Set(25.f,40.f,25.f);

		pPM->SetPosition(m_pStartPositions[iStartPos]);	
		pEntity->SetPhysicsModel(pPM);

		//((CControllableEntity*) pEntity)->SetSpeed(5.f);


		m_LevelEntities[pEntity->GetID()] = pEntity;
	//////////////////////////////////////////////////////////////////////////


	
	//////////////////////////////////////////////////////////////////////////
	// player 1
	iStartPos = 1;
	pEntity = new CControllableEntity();


	pRM = new CCharacter();
	
	((CControllableEntity*) pEntity)->SetSpeed(1.5f);
	
		pRM->SetShared(st_MD5Manager->Load("MD5/vagary/vagary.chr"));
		((CControllableEntity*) pEntity)->SetRenderRenderModel(false);
		pRM->SetScale(CVec3f(1.f,1.f,1.f));
		pRM->SetPosition(m_pStartPositions[iStartPos]);	
		pRM->SetAngles(CVec3f(0.f,m_fStartAngles[iStartPos],0.f));
		pRM->Update(0.f);

	pEntity->SetRenderModel(pRM);

	// create a new physics model, and set it
		 pPM = new CControllablePhysics();
		///pPM->SetParent(pEntity);
		// TODO - compute real BBOx here!
		//pPM->m_BBox = pRM->GetBBox();
		pPM->m_BBox.min.Set(-25.f,-40.f,-25.f);
		pPM->m_BBox.max.Set(25.f,40.f,25.f);
		

		pPM->SetPosition(m_pStartPositions[iStartPos]);	
		
		pEntity->SetPhysicsModel(pPM);
		//((CControllableEntity*) pEntity)->SetSpeed(3.5f);
	

		m_LevelEntities[pEntity->GetID()] = pEntity;
	//////////////////////////////////////////////////////////////////////////
	*/





	

	

	for (int i=0;i<entitiesParser.m_iEntitiesNum;i++)
	{

		bool bLink = true;

		// a jump-pad!
		if (entitiesParser.m_pEntities[i].uiEntityType==kTriggerPush) 
		{
			// find and fill target position location
			for (int j=0;j<entitiesParser.m_iEntitiesNum;j++)
			{
				if (entitiesParser.m_pEntities[j].uiEntityType==kTargetPosition)
					if (strcmp(entitiesParser.m_pEntities[j].caTargetName,entitiesParser.m_pEntities[i].caTargetName)==0)
					{
						CVec3f vTarget = entitiesParser.m_pEntities[j].vOrigin;


						// create a new entity
						pEntity = new CJumpPadEntity();

						pEntity->SetRenderModel(NULL); // no render model

						// create a new physics model, and set it
						IPhysicsModel* pPM = new CBasicPhysics_Static();
						// TODO: find out the real bbox here

						BBox bbox;
						bbox.min = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].min;
						bbox.max = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].max;

						CVec3f vPos = (bbox.min+bbox.max)*0.5f;
						bbox.min-=vPos;
						bbox.max-=vPos;

						
						pPM->m_BBox = bbox;								
						pPM->SetPosition(vPos);
						pPM->SetActive(false);
						
						pEntity->SetPhysicsModel(pPM);
						((CJumpPadEntity*)pEntity)->SetDestination(vTarget);


						// here i need to create and link the JumpPad entity
						break;
					}												
			}
			
		}
		else if (entitiesParser.m_pEntities[i].uiEntityType==kTriggerTeleport) // a teleporter!
		{
			// find and fill target position location
			for (int j=0;j<entitiesParser.m_iEntitiesNum;j++)
			{
				if (entitiesParser.m_pEntities[j].uiEntityType==kTargetPosition)
					if (strcmp(entitiesParser.m_pEntities[j].caTargetName,entitiesParser.m_pEntities[i].caTargetName)==0)
					{
						CVec3f vTarget = entitiesParser.m_pEntities[j].vOrigin;


						// create a new entity
						pEntity = new CTeleporterEntity();

						pEntity->SetRenderModel(NULL); // no render model

						// create a new physics model, and set it
						IPhysicsModel* pPM = new CBasicPhysics_Static();
						// TODO: find out the real bbox here

						BBox bbox;
						bbox.min = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].min;
						bbox.max = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].max;

						CVec3f vPos = (bbox.min+bbox.max)*0.5f;
						bbox.min-=vPos;
						bbox.max-=vPos;

						
						pPM->m_BBox = bbox;								
						pPM->SetPosition(vPos);
						pPM->SetActive(false);
						
						pEntity->SetPhysicsModel(pPM);
						((CTeleporterEntity*)pEntity)->SetDestination(vTarget);
						((CTeleporterEntity*)pEntity)->SetRotYAngle(entitiesParser.m_pEntities[j].iAngle);



						///////////////////////////////////////////////////////////////////////////////
						//// create a particle system that will look like heat haze

						// ps2 - the "regular" non heat hazing one
						/*CParticleSystem* pPS2 = new CParticleSystem();			
						pPS2->SetOrigin(vPos+CVec3f(0.f,-45.f,0.f));											
						pPS2->SetShared(st_ParticleSystemManager->Load("ParticleSystems/HeatHaze2.ps"));																	
						// link it to the map
						SpawnParticleSystem(pPS2,NULL);*/
						//////////////////////////////////////////////////////////////////////////////////////////////

						// ps1 - the heat hazing one
						CParticleSystem* pPS = new CParticleSystem();			
						pPS->SetOrigin(vPos+CVec3f(0.f,-45.f,0.f));											
						pPS->SetShared(st_ParticleSystemManager->Load("ParticleSystems/TeleporterHeatHaze.ps"));																	
						// link it to the map
						SpawnParticleSystem(pPS,NULL);

						

						
						break;
					}												
			}
		}
		else if (entitiesParser.m_pEntities[i].uiEntityType==kTriggerHurt) // hirt
		{
			// create a new entity
						pEntity = new CHurtEntity();

						pEntity->SetRenderModel(NULL); // no render model

						// create a new physics model, and set it
						IPhysicsModel* pPM = new CBasicPhysics_Static();
						// TODO: find out the real bbox here

						BBox bbox;
						bbox.min = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].min;
						bbox.max = m_pMapModels[entitiesParser.m_pEntities[i].iModelIndex].max;

						CVec3f vPos = (bbox.min+bbox.max)*0.5f;
						bbox.min-=vPos;
						bbox.max-=vPos;

						
						pPM->m_BBox = bbox;								
						pPM->SetPosition(vPos);
						pPM->SetActive(false);
						
						pEntity->SetPhysicsModel(pPM);
						((CHurtEntity*)pEntity)->SetDamage(entitiesParser.m_pEntities[i].iDamage);

						
		}

		//////////// AMMO

		else if (entitiesParser.m_pEntities[i].uiEntityType==kAmmoRockets) // rockets ammo
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/shipping_crates.3ds","Materials/bricks-PM.utm",entitiesParser.m_pEntities[i].vOrigin);								
			// heat haze experiment
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/knot.3ds","Materials/PP/HeatHaze.mat",entitiesParser.m_pEntities[i].vOrigin);								
			//pEntity->SetCastShadow(false);
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/shipping_crates.3ds","Materials/Characters/vagary.utm",entitiesParser.m_pEntities[i].vOrigin);								
		}
		else if (entitiesParser.m_pEntities[i].uiEntityType==kAmmoBullets) // bullets ammo
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_2,"Models/shipping_crates.3ds","Materials/Characters/vagary.utm",entitiesParser.m_pEntities[i].vOrigin);								
			
		}
		else if (entitiesParser.m_pEntities[i].uiEntityType==kAmmoShells) // shells ammo
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_3,"Models/shipping_crates.3ds","Materials/Characters/marine_body.utm",entitiesParser.m_pEntities[i].vOrigin);		
			
		} else if (entitiesParser.m_pEntities[i].uiEntityType==kHealth) // health
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_HEALTH,"Models/shipping_crates.3ds","Materials/Characters/vagary_sac.utm",entitiesParser.m_pEntities[i].vOrigin);	
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_HEALTH,"Models/shipping_crates.3ds","Materials/bricks-PM.utm",entitiesParser.m_pEntities[i].vOrigin);	
			
		}  else if (entitiesParser.m_pEntities[i].uiEntityType==kArmorShard) // armor shard
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_ARMOR_SHARD,"Models/shipping_crates.3ds","Materials/bricks-PM.utm",entitiesParser.m_pEntities[i].vOrigin);				
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/shipping_crates.3ds","Materials/Characters/vagary.utm",entitiesParser.m_pEntities[i].vOrigin);								
		}  else if (entitiesParser.m_pEntities[i].uiEntityType==kArmorBody) // body armor
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_ARMOR_BODY,"Models/shipping_crates.3ds","Materials/bricks-PM.utm",entitiesParser.m_pEntities[i].vOrigin);				
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/shipping_crates.3ds","Materials/Characters/vagary.utm",entitiesParser.m_pEntities[i].vOrigin);								
		} else if (entitiesParser.m_pEntities[i].uiEntityType==kCombatArmor) // combat armor
		{
			// create a new entity
			pEntity = new CCollectableEntity();
			//((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_ARMOR_COMBAT,"Models/shipping_crates.3ds","Materials/bricks-PM.utm",entitiesParser.m_pEntities[i].vOrigin);				
			((CCollectableEntity*) pEntity)->Init(COLLECTABLE_ITEM_TYPE_AMMO_1,"Models/shipping_crates.3ds","Materials/Characters/vagary.utm",entitiesParser.m_pEntities[i].vOrigin);								
		}
		else 
		{
			// ignore other types of entities
			bLink = false;
		}
		
		// now, link the entity to the appropriate leafs
		if (bLink)
		{

			// first of all, add it to the big map of entities i keep
			m_LevelEntities[pEntity->GetID()] = pEntity;

			/*CVec3f vLeafMin,vLeafMax;

			BREAKPOINT(!pEntity);
			CVec3f vSphereCenter;
			float  fSphereRadius;

			GetSphereFromBBox(pEntity->GetPhysicsModel()->m_BBox,vSphereCenter,fSphereRadius);

			// instead of brute force running through all of the leafs, just find immidiatly the colliding ones
			LEAFS_INDICES_VECTOR leafsIndicesVec;
			FindLeafs(leafsIndicesVec,vSphereCenter+pEntity->GetPhysicsModel()->m_vPos,fSphereRadius);

			int iTemp = 0;

			for (int l=0;l<leafsIndicesVec.size();l++)
			{
				m_pLeafs[leafsIndicesVec[l]].m_Entities[pEntity->GetID()] = pEntity;
				iTemp++;
			}
			*/

		}
		
	}


	// force a relink of all entities (including static entities, since this is first time we link!)
	ReLinkMapEntitiesToLeafs(true);
}

/*void CQuake3BSP::LinkEntitiesToLeafs(int iEntitiesNum, CEntity** ppEntities)
{	
	////assert(_CrtCheckMemory());

	for (int i=0;i<iEntitiesNum;i++)
	{
		for (int j=0;j<m_iLeafsNum;j++)
		{		


			//////if (( m_ppAllStaticEntities[i])->iModelNum>=0)
			//////{
			//////	if (TestModelAgainstLeaf(( m_ppAllStaticEntities[i])->iModelNum,j))
			//////	{
			//////		m_pLeafs[j].StaticEntities.push_back(m_ppAllStaticEntities[i]);
			//////	}
			//////}
			//////else // no model num, so must be a MD3
			//////{


			//////	if (!  (m_ppAllStaticEntities[i])->m_pModel)
			//////		continue; // no md3 model exists here

			//////	CVector3f vModelMin,vModelMax;				
			//////	vModelMin.Set((m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[0],(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[2],-(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[1]);
			//////	vModelMax.Set((m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[0],(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[2],-(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[1]);
			//////	
			//////	vModelMin+=(m_ppAllStaticEntities[i])->m_vPos;
			//////	vModelMax+=(m_ppAllStaticEntities[i])->m_vPos;

			//////	// Here i should put TestBBoxAgainstLeaf()
			//////	if (TestBBoxAgainstLeaf(vModelMin,vModelMax,j))
			//////		m_pLeafs[j].StaticEntities.push_back(m_ppAllStaticEntities[i]);
			//////}

		}
	}

}*/

//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push all the vertexbuffers to the renderer (used mainly for debugging)
//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushAllToRenderer(void)
{

	for (int i=0;i<m_iFacesNum;i++)
	{
		if (m_pFaces[i].numOfVerts && m_pFaces[i].numMeshVerts)// ignore faces with no vertices
			g_pRendererFront->Push(&m_pRenderables[i]);
	}


	/*for (int i=0;i<m_iTexturesNum;i++)
	{
		if (m_pSharedMaterialRenderables[i].GetIB() == NULL)
			continue;

		g_pRendererFront->Push(&m_pSharedMaterialRenderables[i]);
	}*/
}

//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push ONLY the bezier patches
//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushAllBezierPatchesToRenderer(void)
{
	int i = m_iFacesNum;

	while(i--)
	{
		if (m_pFaces[i].type == Q3FACE_BEZIER_PATCH)
			g_pRendererFront->Push(&m_pRenderables[i]);
	}
}

//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push a single face to the renderer
//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushFaceToRenderer(UINT uiNum)
{
	if (uiNum>=m_iFacesNum) return;

	/*if (m_pDontRenderFaces[uiNum])
		return;*/

	//char temp[100];

	if (m_pFaces[uiNum].numOfVerts)// ignore faces with no vertices
	{
		//sprintf(temp,"PushFaceToRenderer - Face %d\n",uiNum);
		//OutputDebugStringA(temp);
		// Original standard call
		//g_pRendererFront->Push(&m_pRenderables[uiNum]);

		m_VisibleFaces.push_back(uiNum);
	}

	//if (g_OpenglCameraFrustum.PolygonInFrustum((CVec3f*)m_ppVBArray[uiNum].Get(VB_HAS_VERTS),m_ppVBArray[uiNum].GetSize()))
		//st_Renderer::Inst()->Push(&m_ppVBArray[uiNum]);
}

//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// returns the leaf index for a position in space
//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
int CQuake3BSP::FindLeaf(const CVec3f &vPos)
{
	int i = 0;
	float distance = 0.0f;

	while(i >= 0)
	{
		const tBSPNode&  node = m_pNodes[i];
		//const tBSPPlane& plane = m_pPlanesORIGINAL[node.plane];
		const CPlane& plane = m_pPlanes[node.plane];

        distance =	plane.n.v[0] * vPos.v[0] + 
					plane.n.v[1] * vPos.v[1] + 
					plane.n.v[2] * vPos.v[2] - plane.d;

        if(distance >= 0)	
		{
            i = node.front;
        }
        else		
		{
            i = node.back;
        }
    }

    return ~i; 
}


//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// fills the recieved leaf indices with the leafs that intersect with the given bounding sphere
//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::FindLeafs_1(LEAFS_INDICES_VECTOR& fillMe,const CVec3f &vPos,float fRadius,int iNode)
{
	float distance = 0.0f;


		if (iNode<0)
		{
			fillMe.push_back(~iNode);
			return;
		}


		const tBSPNode&  node = m_pNodes[iNode];
		//const tBSPPlane& plane = m_pPlanesORIGINAL[node.plane];
		const CPlane& plane = m_pPlanes[node.plane];

        distance =	plane.n.v[0] * vPos.v[0] + 
					plane.n.v[1] * vPos.v[1] + 
					plane.n.v[2] * vPos.v[2] - plane.d;
	
        if(distance >= fRadius)	// sphere infront of plane		
		{
            iNode = node.front;
			FindLeafs_1(fillMe,vPos,fRadius,iNode);
        }
		else
		if(distance <= -fRadius) // camera behind plane
		{
            iNode = node.back;
			FindLeafs_1(fillMe,vPos,fRadius,iNode);
        }
		else // need to travel both
		{
			iNode = node.front;
			FindLeafs_1(fillMe,vPos,fRadius,iNode);

			iNode = node.back;
			FindLeafs_1(fillMe,vPos,fRadius,iNode);
		}
	
}

//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// is "test" cluster is potentially visible from "current" cluster?
//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
int CQuake3BSP::IsClusterVisible(int current, int test)
{
	if(!m_clusters.pBitsets || current < 0) return 1;

	// Use binary math to get the 8 bit visibility set for the current cluster
	unsigned char visSet = m_clusters.pBitsets[(current*m_clusters.bytesPerCluster) + (test / 8)];
	
	// Now that we have our vector (bitset), do some bit shifting to find if
	// the "test" cluster is visible from the "current" cluster, according to the bitset.
	int result = visSet & (1 << ((test) & 7));

	// Return the result ( either 1 (visible) or 0 (not visible) )
	return ( result );
}


/*void CQuake3BSP::RenderEntity(unsigned int uiID,float fDeltaTime)
{
	ENTITIES_MAP_I i = m_LevelEntities.find(uiID);

	if (i==m_LevelEntities.end())
	{
		BREAKPOINT(1);
		// entity not found
	}

	//if ((*i).second->m_bAlreadyDrawnThisFrame)
	//		return;
	(*i).second->m_bAlreadyDrawnThisFrame = true;

	// directly adress the render model, and not via the entity
	// (which might block an attempt to draw)
	// notice this might cause problems, if there's some more logic
	// inside the entity equivilant functions.
	
	(*i).second->GetRenderModel()->Update(fDeltaTime);
	(*i).second->GetRenderModel()->Render(0);
}*/

/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render a leaf
/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::RenderLeaf( int iLeaf, bool CheckFrustum )
{

	
	
	// get the leaf we are in
	tBSPLeaf* pLeaf = &m_pLeafs[ iLeaf ];

	// perform PVS as in the previus tutorial
	if(!IsClusterVisible(CurrentCameraCluster, pLeaf->cluster))          // If the current leaf can't be seen from our cluster, go to the next leaf
		return;

	if (CheckFrustum)
		if(!m_pFrustum->BoxInFrustum( pLeaf->min.x, pLeaf->min.y, pLeaf->min.z,
			pLeaf->max.x, pLeaf->max.y, pLeaf->max.z)) return;

	int faceCount = pLeaf->numOfLeafFaces;


	float fDeltaTime = g_pTimer->GetDeltaTimeFAKE()*0.001f;

	for (ENTITIES_MAP_I i=pLeaf->m_Entities.begin(); i!=pLeaf->m_Entities.end();++i)
	{
		if (!i->second)
		{
			BREAKPOINT(1);
			continue;
		}

		if (i->second->GetCleanMe())
			continue;



		// prevent multiple rendering of same entity
		if ((*i).second->m_bAlreadyDrawnThisFrame)
			continue;
		(*i).second->m_bAlreadyDrawnThisFrame = true;
		


		// don't draw self
		/*if ((*i).second->GetID() == g_uiControlledHumanPlayer)
			continue;*/

		// debug
		CEntity* pRM = (*i).second;

		if (m_bUpdateEntitiesRenderModels)
			(*i).second->UpdateRenderModel(fDeltaTime,true);


		// if it's the human player, and there has been a request to not render the human player don't draw him.
		if (!(!m_bRenderHumanModel && (*i).second->GetID() == g_uiControlledHumanPlayer))
		{
			if (m_bRenderShadowCastersMode)
			{
				if ((*i).second->GetCastShadow())
					(*i).second->Render(0);
			}
			else
				(*i).second->Render(0);
		}
			
			

		// debugging
		g_idbgEntitiesDrawnNum++;
	}


	char temp[300];

	// Loop through and render all of the faces in this leaf
	while(faceCount--)
	{
		// Grab the current face index from our leaf faces array
		int faceIndex = m_pLeafFaces[pLeaf->leafface + faceCount];


	

		// THIS IS IMPORTANT
		if(m_pFaces[faceIndex].type == Q3FACE_BILLBOARD) continue;

		if(m_pFaces[faceIndex].type == Q3FACE_BEZIER_PATCH)
		{
			int i=0; //just for easy breakpoint...
			i++;
		}


		// if this renderable isn't supposed to cast shadow, continue
		/*if (m_bRenderShadowCastersMode &&  !m_pRenderables[faceIndex].IsShadowCaster())
			continue;*/
		
		m_pUsedTextures[m_pFaces[faceIndex].textureID] = true;

		// Since many faces are duplicated in other leafs, we need to
		// make sure this face wasn't already drawn
		if(!m_FacesDrawn.On(faceIndex)) 	
		{			
			// Set this face as drawn and render it
			m_FacesDrawn.Set(faceIndex);				
			
			// for prebatch<->batch go here
			if (!m_bPrebatching)
			{				
				PushFaceToRenderer(faceIndex);
			}
		}
	}			
}

/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render the BSP tree
/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
// This function wisely check against the frustum and sort leafs front to back
void CQuake3BSP::RenderTree( int Node, bool CheckFrustum )
{

	// Get a pointer to the node we are now in
	tBSPNode* pNode = &m_pNodes[Node];

	// if the node we came from is COMPLETE_IN then CheckFrustum will hold false and we 
	// won't check this node against the frustum because we know we are COMPLETE_IN the 
	// frustum
	if (CheckFrustum){
		// CheckFrustum = true so our node parent was intersecting the frustum and so
		// he have to continue checking this one
		switch (m_pFrustum->BoxInFrustum( pNode->min.x, pNode->min.y, pNode->min.z, pNode->max.x, pNode->max.y, pNode->max.z)){
			case COMPLETE_OUT: return;				// if this one is complete_out, stop analysing this branch
				// all leaf that come from this node are also complete_out

			case COMPLETE_IN: CheckFrustum = false; // if we are complete_in, Set CheckFrustum to false to not 
				// check frustum on any of my childs, CheckFrustum will be used
				// to call my children

				// if intersecting continue as usual, test all my children against the frustum
		}
	}

	// a node is an structure that hold a partition plane, all leaf are splited in two
	// groups: FRONT and BACK. plane is the partition plane for this node
	//const tBSPPlane* plane = &m_pPlanesORIGINAL[ pNode->plane ];
	const CPlane* plane = &m_pPlanes[ pNode->plane ];

	// We will render leafs in a from to back order 

	// If the camera position is in front the plane
	if ( (plane->n.v[0] * m_CameraPosition.v[0] + plane->n.v[1] * m_CameraPosition.v[1] + plane->n.v[2] * m_CameraPosition.v[2] -  plane->d  > 0) ) 
	{
		if (pNode->front>=0)  RenderTree( pNode->front, CheckFrustum );
			else RenderLeaf( ~pNode->front, CheckFrustum );
		// now that we render or leaf and nodes that are in front of the plane,
		// go with the  BACK ones
		if (pNode->back>=0) RenderTree( pNode->back , CheckFrustum);
			else RenderLeaf( ~pNode->back, CheckFrustum );
	}// else if is CameraPostion is back of the partition plane, draw fack to front
	else 
	{
		if (pNode->back>=0) RenderTree( pNode->back, CheckFrustum );
			else RenderLeaf( ~pNode->back, CheckFrustum );
		if (pNode->front>=0)  RenderTree( pNode->front, CheckFrustum );
			else RenderLeaf( ~pNode->front, CheckFrustum );
	}
}


void CQuake3BSP::UpdateCulledEntitiesRenderModels(void)
{
	float fDeltaTime = g_pTimer->GetDeltaTimeFAKE()*0.001f;

	for (ENTITIES_MAP_IR it=m_LevelEntities.rbegin();it!=m_LevelEntities.rend();++it)
	{
		if (!(*it).second->m_bAlreadyDrawnThisFrame)
		{
				(*it).second->UpdateRenderModel(fDeltaTime,false);
				g_idbgEntitiesOnlyNecessaryRMUpdate++;
		}
	}
}


void CQuake3BSP::RenderLevel_ShadowCasters(const CVec3f &vPos)
{
	m_bRenderShadowCastersMode = true;

	RenderLevel(vPos);

	m_bRenderShadowCastersMode = false;
}

////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render the Q3 BSP level
////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::RenderLevel(const CVec3f &vPos)
{
	g_idbgEntitiesDrawnNum = 0;
	g_idbgEntitiesOnlyNecessaryRMUpdate = 0;

	// tag all entities as not rendered
	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
	{
		(*it).second->m_bAlreadyDrawnThisFrame = false;		
	}



	static CProfileStep step("q3map");	
	step.Begin();

	for (int i=0;i<m_iTexturesNum;i++)
		m_pUsedTextures[i] = false;
	
	/*CVec3f tempCamPos(vPos);
	tempCamPos.v[2] = -tempCamPos.v[2];*/

	// Get the number of faces in our level
	int i = m_iFacesNum;
	
	// Reset our bitset so all the slots are zero. 
	
	m_FacesDrawn.ClearAll();
	

	m_CameraPosition = vPos; 

	int leafIndex = FindLeaf( m_CameraPosition );
	CurrentCameraCluster =   m_pLeafs[leafIndex].cluster;

	/*st_Engine::Inst()->m_iCameraClusterNum = CurrentCameraCluster;
	st_Engine::Inst()->m_iCameraLeafNum    = leafIndex;*/

	// Start rendering the tree from the Root (Node 0 ) and start checking the frustum (true)

	//PushAllBezierPatchesToRenderer();

	// RESTORE

	//char temp[100];

	//debugging
/*	for (int i=0;i<m_BezierFaceBBoxes.size();i++)
		g_pGraphicsAPI->DebugRenderBBox(m_BezierFaceBBoxes[i]);*/

	if(!st_Input->KeyPressed(DIK_R))
	{

		//OutputDebugStringA("--------------------\n");
		//OutputDebugStringA("--------Frame-------\n");
		//OutputDebugStringA("--------------------\n");

		RenderTree( 0, true ); 
		
		for (m_VisibleFaces_Vec_IT it=m_VisibleFaces.begin();it!=m_VisibleFaces.end();++it)
		{
			g_pRendererFront->Push(&m_pRenderables[*it]);
			//sprintf(temp,"m_VisibleFaces - Face %d\n",*it);
			//OutputDebugStringA(temp);
		}
		
		m_VisibleFaces.erase(m_VisibleFaces.begin(),m_VisibleFaces.end());

	}

	// now, loop on all entities that were culled down, and update necessary stuff for them
	// (this is important, because of the frame animation triggers can trigger projectile shooting / particles system / etc.)

	if (m_bUpdateEntitiesRenderModels)
		UpdateCulledEntitiesRenderModels();


	//BruteDrawDynamicEntities();


	
	// for prebatch<->batch go here

	if (m_bPrebatching)
	{
	
		// here i render all the pre-batched renderables	
		for (int i=0;i<m_iTexturesNum;i++)
		{
			// if texture wasn't used in this frame then continue
			if (!m_pUsedTextures[i])
				continue;

			if (m_pSharedMaterialRenderables[i].GetIB() == NULL)
				continue;


			g_pRendererFront->Push(&m_pSharedMaterialRenderables[i]);
		}
	}

	step.End();

	

	/*g_pGraphicsAPI->startPrinting();
	g_pGraphicsAPI->AddDebugText("Map Entities Num: %d. %d of them visible, %d only got critical update\n",m_LevelEntities.size(),
		g_idbgEntitiesDrawnNum,g_idbgEntitiesOnlyNecessaryRMUpdate);	
	g_pGraphicsAPI->stopPrinting();*/

}

void CQuake3BSP::RenderAllFacesBBox(void)
{
	for (int i=0;i<m_iFacesNum;i++)
		RenderFaceBBox(i);
}

void CQuake3BSP::DebugApplyForceOnAllNonStaticEntities(const CVec3f& vVec)
{
	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
		(*it).second->GetPhysicsModel()->ApplyForce(vVec);


}

void CQuake3BSP::RenderAllEntitiesBBoxes(void)
{
	BBox bbox;
	

	for (ENTITIES_MAP_I it=m_LevelEntities.begin();it!=m_LevelEntities.end();++it)
	{
		//bbox.min = (*it).second->GetPhysicsModel()->m_BBox.min + (*it).second->GetPhysicsModel()->GetPosition();
		//bbox.max = (*it).second->GetPhysicsModel()->m_BBox.max + (*it).second->GetPhysicsModel()->GetPosition();

		bbox.min = (*it).second->GetPhysicsModel()->m_BBox.min + (*it).second->GetPhysicsModel()->GetPosition();
		bbox.max = (*it).second->GetPhysicsModel()->m_BBox.max + (*it).second->GetPhysicsModel()->GetPosition();

		g_pGraphicsAPI->DebugRenderBBox(bbox);
	}
		
}


void CQuake3BSP::RenderFaceBBox(int iFaceNum)
{
	const BBox* bbox = m_pRenderables[iFaceNum].GetBBox();

	if (bbox)
	{
		CVec3f vMin = bbox->min;
		CVec3f vMax = bbox->max;

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

		g_pGraphicsAPI->DrawDebugLine(v1,v2,col);
		g_pGraphicsAPI->DrawDebugLine(v1,v4,col);
		g_pGraphicsAPI->DrawDebugLine(v1,v5,col);
		g_pGraphicsAPI->DrawDebugLine(v2,v3,col);
		g_pGraphicsAPI->DrawDebugLine(v3,v4,col);
		g_pGraphicsAPI->DrawDebugLine(v2,v6,col);
		g_pGraphicsAPI->DrawDebugLine(v5,v8,col);
		g_pGraphicsAPI->DrawDebugLine(v7,v8,col);
		g_pGraphicsAPI->DrawDebugLine(v6,v5,col);
		g_pGraphicsAPI->DrawDebugLine(v6,v7,col);
		g_pGraphicsAPI->DrawDebugLine(v4,v8,col);
		g_pGraphicsAPI->DrawDebugLine(v3,v7,col);
	}

}

/*
void CQuake3BSP::DrawMapModelsBBoxes(void)
{
//	for (int i=0;i<m_iMapModelsNum;i++)
//		st_Renderer::Inst()->DebugDrawBBox(m_pMapModels[i].min,m_pMapModels[i].max);

}*/

void CQuake3BSP::DrawMapLeafsBBoxes(void)
{
	BBox bbox;
	
	for (int i=0;i<m_iLeafsNum;i++)
	{
		bbox.min.Set(m_pLeafs[i].min.x,m_pLeafs[i].min.y,m_pLeafs[i].min.z);
		bbox.max.Set(m_pLeafs[i].max.x,m_pLeafs[i].max.y,m_pLeafs[i].max.z);

		g_pGraphicsAPI->DebugRenderBBox(bbox);		
	}
	

}


/*void CQuake3BSP::RenderMapModels(void)
{
	int faceIndex;

	//for (int i=0;i<m_iMapModelsNum;i++)
	for (int i=1;i<m_iMapModelsNum;i++)
	{
		//int faceIndex;          // The first face index in the model 
		//int numOfFaces;         // The number of faces in the model 
		for (int j=0;j<m_pMapModels[i].numOfFaces;j++)
		{
			faceIndex = m_pMapModels[i].faceIndex + j;
			// Since many faces are duplicated in other leafs, we need to
			// make sure this face already hasn't been drawn.
			if(!m_FacesDrawn.On(faceIndex)) 	
			{
				// Set this face as drawn and render it
				m_FacesDrawn.Set(faceIndex);	
				PushFaceToRenderer(faceIndex);
			}

		}
	}

}
*/

void CQuake3BSP::SetFrustum(CFrustum* pFrustum)
{
	m_pFrustum = pFrustum;
}

////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// set the map file name
////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::SetMapFileName(const char* pcSet)
{
	SafeDeleteArray(m_pcMapFileName);
	m_pcMapFileName = new char[strlen(pcSet)+1];
	strcpy(m_pcMapFileName,pcSet);
}


/////////////
// Debug
/////////////

/*void CQuake3BSP::OutPutShadersToFile(void)
{
	std::ofstream t_File;
	t_File.open("ThisMapShaders.txt");
	
	for (int i=0;i<m_iTexturesNum;i++)
	{
		t_File << i << ": " << m_pTextures[i].strName << ".\n";
	}

	t_File.close();	
}*/

/*
/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// recuresly slides along the planes
/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CQuake3BSP::TraceAndResponse(CTraceRequest* input,CTraceResult* output, int iDepth, CEntity* pCausedByEntity)
{
	if (iDepth<20)
		iDepth++;
	else
		return false;



	// DEBUG
	CVec3f vDirOriginal = (input->vRayEnd-input->vRayStart).Normalize();


	
    
	// almost nothing to move
	if (DISTANCE(input->vRayStart,input->vRayEnd)<EPSILON*2)
	{
		output->EndPoint = input->vRayEnd;
		
		if (iDepth==1)
			return true; // we completed the move in the FIRST try
		else
			return false; // we completed the move in a move > first move
	}


	Trace(input,output,true,true,pCausedByEntity); // trying to get there
    
	// didn't collide with anything
	if (output->Fraction>1.f-EPSILON)
	{
		if (iDepth==1)
			return true; // we completed the move in the FIRST try
		else
			return false; // we completed the move in a move > first move
	}


	

	//////// DEBUG DEBUG DEBUG
	

	// we intersected with something, so we need to find the new slide vector

	CVec3f vWhatsLeft = (input->vRayEnd - output->EndPoint); // this is what we still need to travel
	
	vWhatsLeft-= output->CollisionPlane.n * ((vWhatsLeft*output->CollisionPlane.n));


	CVec3f vNormalizedSlideVector = vWhatsLeft;
	vNormalizedSlideVector.Normalize();

	
	// we can't climb such a big slope
	if (vNormalizedSlideVector.v[1] > BIGGEST_CLIMB_SLOPE)
		return false;


	input->vRayStart = output->EndPoint;
	input->vRayEnd   = output->EndPoint + vWhatsLeft;



	TraceAndResponse(input,output,iDepth,pCausedByEntity);
	


	return true;
}
*/

/*
void CQuake3BSP::TraceAgainstModel(CTraceRequest* input,CTraceResult* output,int iModelNum)
{
    m_CurrentTraceInput  = input;
	m_CurrentTraceOutput = output;

	if (input->dwTraceType == 	TRACE_TYPE_LINE)
		m_CurrentTraceInput->fRadius = 0.f;


	{
		m_CurrentTraceOutput->StartOut = true;
		m_CurrentTraceOutput->AllSolid = false;
		m_CurrentTraceOutput->Fraction = 1.f;


		tBSPModel* model = &m_pMapModels[iModelNum];

		for (int i=0; i < model->numOfBrushes ; i++)
		{
		
			//tBSPBrushORIGINAL* brush = &m_pBrushesORIGINAL[m_pLeafBrushes[model->brushIndex + i]];
			tBSPBrushORIGINAL* brush = &m_pBrushesORIGINAL[model->brushIndex + i];

			//if (brush->numOfBrushSides>0 && m_pTextures[brush->textureID].contents & 1)
			if (brush->numOfBrushSides>0)
				CheckBrush(brush);

		}

				
		if (m_CurrentTraceOutput->Fraction==1)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayEnd;
		else
		if (m_CurrentTraceOutput->Fraction<EPSILON)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart;
		else
		{
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart + 
											(m_CurrentTraceInput->vRayEnd-m_CurrentTraceInput->vRayStart)*m_CurrentTraceOutput->Fraction;
		}
		
	}



}*/

bool CQuake3BSP::TestModelAgainstLeaf(int iModelNum,int iLeafNum)
{
	if (iLeafNum<0 || iLeafNum>=m_iLeafsNum)
		return false;

	/*CVec3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVec3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);*/

	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG

	CVec3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVec3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);

	return BBoxesIntersect(m_pMapModels[iModelNum].min,m_pMapModels[iModelNum].max,vLeafMin,vLeafMax);
		
}



bool CQuake3BSP::TestBBoxAgainstLeaf(CVec3f& vMin,CVec3f& vMax,int iLeafNum)
{
	/*CVec3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVec3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);*/

	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG

	CVec3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVec3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);

	return BBoxesIntersect(vMin,vMax,vLeafMin,vLeafMax);
		
}

ILight_Export** CQuake3BSP::GetLights()
{
	return m_ppLights;
}

/*
/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// send a trace down the bsp tree and check against the brushes
/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//void CQuake3BSP::Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities, int iAbsolutEntity)
void CQuake3BSP::Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities,bool bCheckDynamicEntities, CEntity* pCausedByEntity)
{	
	//
	//m_iTempCurrentTracedEntity = iAbsolutEntity;
	m_pTempCurrentTracedEntity = pCausedByEntity;
	//

    m_CurrentTraceInput  = input;
	m_CurrentTraceOutput = output;

	m_CurrentTraceOutput->HitContents = 0;
	m_CurrentTraceOutput->HitFlags = 0;

	
	// DEBUG DEBUG DEBUG

	//bool bIsntBigEnough=false;

	CVec3f vIsBigEnough = input->vRayEnd - input->vRayStart;


	{
		output->EndPoint+= vIsBigEnough.Normalize()*0.3f;
	}
	

	if (input->dwTraceType == 	TRACE_TYPE_LINE)
		m_CurrentTraceInput->fRadius = 0.f;


	{
		m_CurrentTraceOutput->StartOut = true;
		m_CurrentTraceOutput->AllSolid = false;
		m_CurrentTraceOutput->Fraction = 1.f;

		m_pBrushThatWasLastHit = NULL;
		m_CurrentTraceOutput->HitContents = 0;

		//
		m_CurrentTraceOutput->HitFlags = 0;




		
	
		
		//if (m_iTempCurrentTracedEntity!=-2) // -2 means don't check against dynamic entities (-1 means that this wasn't caused by a dynamic entity)
		// i have to add some boolean that means "don't check against dynamic entities or sth like that,
		// to replace the last line
		if (bCheckDynamicEntities)
		{
			for (int e=0;e<m_iDynamicEntitiesNum;e++)
			{

				//if (m_iTempCurrentTracedEntity>=0)
				if (m_pTempCurrentTracedEntity==m_ppAllDynamicEntities[e]) // we don't wanna check against the same entity ;)
					continue;

				//m_ppAllDynamicEntities[e]->m_uiCDFrame=st_Renderer::Inst()->m_uiFrameCounter;

				if (m_ppAllDynamicEntities[e]->m_bActive)
				if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType!=DYNAMIC_ENTITY_TYPE_PROJECTILE) // we don't want to aim at projectiles
				if (CheckBrush(m_ppAllDynamicEntities[e]->m_pBrush))
				{
					//if (m_iTempCurrentTracedEntity>=0)
					if (m_pTempCurrentTracedEntity)
					{
						// caused by player
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
						{
							// to a player
							if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{
								CVec3f vPlayerPushPlayer= (input->vRayEnd - input->vRayStart).Normalize();
								if (!m_ppAllDynamicEntities[e]->m_pPlayerClient->m_bDying && !m_ppAllDynamicEntities[e]->m_pPlayerClient->m_bStartDie)
									m_ppAllDynamicEntities[e]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vPlayerPushPlayer * 10.f;
							}

						}
						else 
						// caused by a projectile
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PROJECTILE)
						{
							// to a player
							if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{

								//direct projectile hit damage
								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_ROCKET)
								{
									m_ppAllDynamicEntities[e]->m_pPlayerClient->m_iLife-=50*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor;
									m_ppAllDynamicEntities[e]->m_pPlayerClient->PlayPainSound(50*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor);
								}
								else
								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_PLASMA)																							
								{
									m_ppAllDynamicEntities[e]->m_pPlayerClient->m_iLife-=8*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor;
									m_ppAllDynamicEntities[e]->m_pPlayerClient->PlayPainSound(8*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor);
								}


								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_ROCKET)																							
								{
									CVec3f vExplosionPushPlayer = (m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_vPos - m_pTempCurrentTracedEntity->m_vPos).Normalize();
									m_ppAllDynamicEntities[e]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vExplosionPushPlayer * 200.f;
								}

								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = true;
								m_pTempCurrentTracedEntity->OnTouch();
							}


						}
					}	
									
				}
			}
		}
		


		



		// DEBUG DEBUG DEBUG
		//if (!m_pBrushThatWasLastHit)
		CheckMoveByNode(0,0.f,1.f,m_CurrentTraceInput->vRayStart,m_CurrentTraceInput->vRayEnd,bActivateEntities,0);


		


		//BREAKPOINT(m_CurrentTraceOutput->HitContents & CONTENTS_SLIME);

		if (m_pBrushThatWasLastHit)
			{

				if (m_pBrushThatWasLastHit->textureID>=0) // shows we've hit a "regular" map brush
				{
					m_CurrentTraceOutput->HitContents |= m_pTextures[m_pBrushThatWasLastHit->textureID].contents;
					m_CurrentTraceOutput->HitFlags    |= m_pTextures[m_pBrushThatWasLastHit->textureID].flags;


					// if a projectile hit a regular map brush

					if (m_pTempCurrentTracedEntity)
					{
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PROJECTILE)
						{
							((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_vNormalAtCollision = m_CurrentTraceOutput->CollisionPlane.n;

							// only leave a mark if the surface hit "allows" it
							if (m_CurrentTraceOutput->HitFlags &SURF_NOMARKS)
								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = true;
							else
								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = false;
							m_pTempCurrentTracedEntity->OnTouch();
						}
					}
					else // if wasn't caused by an entity, but by a TRACE_CAUSED_BY_SHOOT
					{
						if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT)					
						if (m_CurrentTraceInput->iAbsoluteEntityIndexCause>=0)
						if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_bStartAttack)					
						if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
						{
							// add the effect of bullet hit something

							CEntity* pEntity;

							GetFreeProjectileEntity(pEntity,PROJECTILE_TYPE_BULLET); // recycle/allocate a projectile
	
							pEntity->m_bActive=false; // cause it to go to explosion immidiatly
							pEntity->m_pPhysicsModel->SetPosition(m_CurrentTraceOutput->EndPoint);
	
							pEntity->m_pPhysicsModel->ChangePositionNow(true);

							pEntity->m_vPos = m_CurrentTraceOutput->EndPoint;
                                		
							((CEntityProjectile*)pEntity)->m_vTryMoveDirection.Set(0.f,0.f,0.f);

							((CEntityProjectile*)pEntity)->m_vNormalAtCollision = m_CurrentTraceOutput->CollisionPlane.n;

							pEntity->OnTouch();

		
						}
								

					}

				}
				else
				{
					// here put the entity that we collided with
					// a cool "trick" might be to put in the "contents" the absolute ID to the dynamic entity we've hit
					// hmm, actually adding another field would be better

					m_CurrentTraceOutput->iAbsoluteEntityIndexHit = m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex;
					
					
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT)					
					if (m_CurrentTraceInput->iAbsoluteEntityIndexCause>=0)
					if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_bStartAttack)					
					if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
					{						
						if (m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex>=0)
						{
							if (m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{						
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_iLife-=4*m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_fReduceDamageFactor;
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->PlayPainSound(4*m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_fReduceDamageFactor);

								CVec3f vShotPushPlayer= (input->vRayEnd - input->vRayStart).Normalize();
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vShotPushPlayer * 50.f;
								
							}
						}

						
					}
				}

						

				if (m_pTempCurrentTracedEntity)
				{

					if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
					if (m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel())
					{
						m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround |= m_CurrentTraceOutput->HitContents;
						m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround |= m_CurrentTraceOutput->HitFlags;
					}
				}

			}




		if (m_CurrentTraceOutput->Fraction==1)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayEnd;
		else
		{
			//m_CurrentTraceOutput->Fraction-=EPSILON;
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart + 
											(m_CurrentTraceInput->vRayEnd-m_CurrentTraceInput->vRayStart)*m_CurrentTraceOutput->Fraction;
		}
		
	}


	

}
*/

/*

/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// recurse check the bsp tree by nodes
/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::CheckMoveByNode(int iNodeIndex, float fStartFraction, float fEndFraction, CVec3f vStart, CVec3f vEnd,bool bActivateEntities,int iRecurseDepth)
{	
	iRecurseDepth++;
	if (iRecurseDepth>20)
		return;

	// this doesn't work for some reason
	//if (fStartFraction > m_CurrentTraceOutput->Fraction)
	//{
		//return;
	//}

	if (iNodeIndex < 0) // a leaf
	{

		int iLeafNum = -(iNodeIndex+1);

		tBSPLeaf* leaf = &m_pLeafs[iLeafNum];




		tBSPBrush* brush;

		int i;




		if (bActivateEntities)
		for (i=0;i<leaf->StaticEntities.size();i++)
		{
									

			int iModelNum = ((CEntityJumpPad*) leaf->StaticEntities[i])->iModelNum; // hmm

			for (int j=0;j<m_pMapModels[iModelNum].numOfBrushes;j++)
			{

				if (iModelNum>=0)
				{

					brush = &m_pBrushes[m_pMapModels[iModelNum].brushIndex + j ];
					if (CheckBrush(brush))
					{
	


						g_bCollidedWithStaticEntity = true;
						leaf->StaticEntities[i]->OnTouched();

						break;				
					}
				}
				else // there is no brush related to this model (an MD3 model for example)
				{
					leaf->StaticEntities[i]->OnTouched();
					break;				
				}

				// hmm - i need to consider a case that one entity is blocking the other,
				// and to NOT activate her in that case...
				// (i just need to simply store a brush, and only perform the entity activation in the END of this function)

			}

		}

		//if (!g_bCollidedWithStaticEntity)
		{			

			// check collision against "regular" brushes

			for (i=0; i < leaf->numOfLeafBrushes ; i++)
			{
					brush = &m_pBrushes[m_pLeafBrushes[leaf->leafBrush + i]];
	
	
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_PLAYER) // it's a player
					{
						
						if (brush->numOfBrushSides>0 &&
							m_pTextures[brush->textureID].contents & (CONTENTS_SOLID | CONTENTS_PLAYERCLIP))
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_PROJECTILE) // it's a projectile
					{
						
						if (brush->numOfBrushSides>0 &&
							m_pTextures[brush->textureID].contents & (CONTENTS_SOLID))
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_BOT) // it's a bot
					{
						if (brush->numOfBrushSides>0 && 
						(m_pTextures[brush->textureID].contents & CONTENTS_SOLID) || (m_pTextures[brush->textureID].contents & CONTENTS_PLAYERCLIP) )
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT) // it's a gun shot
					{
						if (brush->numOfBrushSides>0 && m_pTextures[brush->textureID].contents & CONTENTS_SOLID)
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_TEST) // it's a test
					{
						if (brush->numOfBrushSides>0)
						{
							CheckBrush(brush);
						}
					}

					
			}



		}

		return; // nothing left to do
	}

	// a node

	tBSPNode  *node  = &m_pNodes[iNodeIndex];
	//tBSPPlane *plane = &m_pPlanesORIGINAL[node->plane];
	CPlane *plane = &m_pPlanes[node->plane];

	// distance from the start and end point to the plane

	float fStartDistance = (plane->n * vStart) - plane->d;
	float fEndDistance   = (plane->n * vEnd)   - plane->d;
    
	
	if (fStartDistance >= m_CurrentTraceInput->fRadius && fEndDistance >= m_CurrentTraceInput->fRadius) // both are infront
		CheckMoveByNode(node->front, fStartFraction, fEndFraction, vStart, vEnd,bActivateEntities,0);
	else
	if (fStartDistance < -m_CurrentTraceInput->fRadius && fEndDistance < -m_CurrentTraceInput->fRadius)  // both are behind
		CheckMoveByNode(node->back,fStartFraction,fEndFraction,vStart,vEnd,bActivateEntities,0);
	else // moving from one side to another
	{
		int side;
		float fraction1,fraction2,middleFraction;
		CVec3f vMiddle;

		if (fStartDistance < fEndDistance)
		{
			side = 1; // back
			float inverseDistance = 1.f / (fStartDistance - fEndDistance);
			fraction1 = (fStartDistance - m_CurrentTraceInput->fRadius - EPSILON) * inverseDistance;
			fraction2 = (fStartDistance + m_CurrentTraceInput->fRadius + EPSILON) * inverseDistance;
		}
		else
		if (fEndDistance < fStartDistance)
		{
			side = 0; // front
			float inverseDistance = 1.f/(fStartDistance - fEndDistance);
			fraction1 = (fStartDistance + m_CurrentTraceInput->fRadius + EPSILON) * inverseDistance;
			fraction2 = (fStartDistance - m_CurrentTraceInput->fRadius - EPSILON) * inverseDistance;
		}
		else
		{
			side = 0; //front
			fraction1 = 1.f;
			fraction2 = 0.f;
		}


		// step 2
		if (fraction1 < 0.f) 
			fraction1 = 0.f;
		else if (fraction1 > 1.f) 
			fraction1 = 1.f;

		if (fraction2 < 0.f) 
			fraction2 = 0.f;
		else if (fraction2 > 1.f) 
			fraction2 = 1.f;


		// step 3
		middleFraction = fStartDistance + (fEndDistance - fStartFraction) * fraction1;
		vMiddle = vStart + ((vEnd-vStart)*fraction1);

		// step 4
		if (side==1) // back
			CheckMoveByNode(node->back,fStartFraction,middleFraction,vStart,vMiddle,bActivateEntities,0);
		else  // front
			CheckMoveByNode(node->front,fStartFraction,middleFraction,vStart,vMiddle,bActivateEntities,0);

		// step 5

		middleFraction = fStartFraction + (fEndFraction - fStartFraction)*fraction2;
		vMiddle = vStart + ((vEnd-vStart)*fraction2);

		// step 6

		if (side==0) // back-->front
			CheckMoveByNode(node->back,middleFraction,fEndFraction,vMiddle,vEnd,bActivateEntities,0);
		else  // front-->back
			CheckMoveByNode(node->front,middleFraction,fEndFraction,vMiddle,vEnd,bActivateEntities,0);
	}

}

*/

///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
////// check against the brush
///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
/*bool CQuake3BSP::CheckBrush(tBSPBrush *brush)
{
	bool bDidCollide=false;

	float fStartFraction = -1.f;
	float fEndFraction   = 1.f;
	bool bStartsOut = false;
	bool bEndsOut = false;



	int iSavedBrushSideNum = 0;

	CVec3f vCandidateToHitNormal;

	for (int i=0;i<brush->numOfBrushSides;i++)
	{

		CPlane     *plane;

		if (brush->m_pPlanes) // a dynamic entity brush
		{
			plane     = &(brush->m_pPlanes[i]);
		}
		else // a map brush
		{
			tBSPBrushORIGINALSide *brushSide = &m_pBrushSides[brush->brushSide + i];						
			plane     = &m_pPlanes[brushSide->plane];
		}
		
		float fStartDistance,fEndDistance;

		if (m_CurrentTraceInput->dwTraceType==TRACE_TYPE_LINE)
		{
			fStartDistance = (plane->n*m_CurrentTraceInput->vRayStart) - plane->d;
			fEndDistance   = (plane->n*m_CurrentTraceInput->vRayEnd)   - plane->d;
		}
		else
		if (m_CurrentTraceInput->dwTraceType==TRACE_TYPE_SPHERE)
		{
			fStartDistance = (plane->n*m_CurrentTraceInput->vRayStart) - (plane->d + m_CurrentTraceInput->fRadius);
			fEndDistance   = (plane->n*m_CurrentTraceInput->vRayEnd)   - (plane->d + m_CurrentTraceInput->fRadius);
		}
        

		if (fStartDistance>0.f)
			bStartsOut=true;
		if (fEndDistance>0.f)
			bEndsOut = true;


		if (fStartDistance > 0 && fEndDistance > 0)
			return false;
		if (fStartDistance<=0 && fEndDistance<=0)
			continue;

		if (fStartDistance>fEndDistance) // line is entering the brush
		{
			float fraction = (fStartDistance - EPSILON) / (fStartDistance - fEndDistance);

			if (fraction > fStartFraction)
			{
				fStartFraction = fraction;
				iSavedBrushSideNum = i;
			}

	
		}
		else // line is leaving the brush
		{
			float fraction = (fStartDistance + EPSILON) / (fStartDistance - fEndDistance);
			if (fraction < fEndFraction)
			{
				fEndFraction = fraction;
			}

		}

	}

	if (bStartsOut == false)
	{
		m_CurrentTraceOutput->StartOut = false;
		if (bEndsOut == false)
		m_CurrentTraceOutput->AllSolid = true;
		
		
		// debug debug debug
		//m_CurrentTraceOutput->Fraction = 1.f;
		
		return false; //fishy
	}



	if (fStartFraction < fEndFraction)
	{
		if (fStartFraction > -1 && fStartFraction < m_CurrentTraceOutput->Fraction)
		{
			if (fStartFraction < 0)
				fStartFraction = 0;

			m_CurrentTraceOutput->Fraction = fStartFraction;

			CPlane *POTplane;

			if (brush->m_pPlanes) // a dynamic entity brush
			{
				POTplane     = &(brush->m_pPlanes[iSavedBrushSideNum]);
			}
			else // a map brush
			{
				tBSPBrushORIGINALSide *POTbrushSide = &m_pBrushSides[brush->brushSide + iSavedBrushSideNum];
				POTplane = &m_pPlanes[POTbrushSide->plane];
			}

			

			m_CurrentTraceOutput->CollisionPlane.n = POTplane->n;
			m_CurrentTraceOutput->CollisionPlane.d = POTplane->d;

			m_pBrushThatWasLastHit = brush;
			

			bDidCollide = true;
		}
	}

	return bDidCollide;
}*/



