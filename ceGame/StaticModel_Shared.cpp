#include "stdafx.h"
#include "StaticModel_Shared.h"


#include "StaticModelManager.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;



#include "ce.h"

CStaticModel_Shared::CStaticModel_Shared()
{
	m_iSubMeshesNum=0;
	m_ppSubMeshes=NULL;
	m_pcName = NULL;
	m_ppVertexBuffers = NULL;
	m_ppIndexBuffers = NULL;
}

CStaticModel_Shared::~CStaticModel_Shared()
{
	char message[200];
	sprintf(message,"deleting static_model_shared: ""%s""...",m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);

	sprintf(message,"deleting static_model_sharedmodel: ""%s"" DONE!",m_pcName);

	st_StaticModelManager->Unregister(m_pcName); // unregister this static model from static-model-manager
	
	for (int i=0;i<m_iSubMeshesNum;i++)
		SafeDelete(m_ppSubMeshes[i]);

	SafeDeleteArray(m_ppSubMeshes);


	if (m_ppVertexBuffers)
	{
		for (int i=0;i<m_iSubMeshesNum;i++)
			SafeDelete(m_ppVertexBuffers[i]);

		SafeDeleteArray(m_ppVertexBuffers);
	}

	if (m_ppIndexBuffers)
	{
		for (int i=0;i<m_iSubMeshesNum;i++)
			SafeDelete(m_ppIndexBuffers[i]);

		SafeDeleteArray(m_ppIndexBuffers);
	}
	
	SafeDeleteArray(m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

bool CStaticModel_Shared::GenerateBuffers(DWORD dwGenerate)
{
	m_ppVertexBuffers = new IVertexBuffer_Export*[m_iSubMeshesNum];
	m_ppIndexBuffers  = new IIndexBuffer*[m_iSubMeshesNum];



	CVec3f* pVertices;
	CVec3f* pTangents;
	CVec3f* pBinormals;
	CVec3f* pNormals;
	CVec2f*  pTexCoords;


	//IIndexBuffer* pIB;
	UINT* pIndices;

for (int iCurrentSubMeshNum=0;iCurrentSubMeshNum<m_iSubMeshesNum;iCurrentSubMeshNum++)
{

	m_ppIndexBuffers[iCurrentSubMeshNum] = g_pRendererBack->GenerateIndexBuffer();
	//pIB->Allocate(3,IB_MEM_GPU);
	m_ppIndexBuffers[iCurrentSubMeshNum]->Allocate(m_ppSubMeshes[iCurrentSubMeshNum]->iIndicesNum,IB_MEM_GPU);
	//pIB->Lock(CR_LOCK_READ_WRITE);
	m_ppIndexBuffers[iCurrentSubMeshNum]->Lock(CR_LOCK_WRITE_ONLY,0,0);
	
	pIndices = (UINT*) m_ppIndexBuffers[iCurrentSubMeshNum]->GetData();

	memcpy(pIndices,m_ppSubMeshes[iCurrentSubMeshNum]->uiIndices,sizeof(unsigned int)*m_ppSubMeshes[iCurrentSubMeshNum]->iIndicesNum);
	
	m_ppIndexBuffers[iCurrentSubMeshNum]->Unlock();



	m_ppVertexBuffers[iCurrentSubMeshNum] = g_pRendererBack->GenerateVertexBuffer();
	m_ppVertexBuffers[iCurrentSubMeshNum]->AddStream(STREAM_POSITION,CR_FLOAT3);
	m_ppVertexBuffers[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD0,CR_FLOAT2);	
	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		m_ppVertexBuffers[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD1,CR_FLOAT3);	
		m_ppVertexBuffers[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD2,CR_FLOAT3);	
		m_ppVertexBuffers[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD3,CR_FLOAT3);	
	}

	
	m_ppVertexBuffers[iCurrentSubMeshNum]->AllocateStreams(m_ppSubMeshes[iCurrentSubMeshNum]->iVerticesNum/3,VB_MEM_GPU|VB_SINGLE,CR_STREAM_STATIC);

	
	m_ppVertexBuffers[iCurrentSubMeshNum]->Lock(CR_LOCK_READ_WRITE);	

	pVertices = (CVec3f*) m_ppVertexBuffers[iCurrentSubMeshNum]->GetStream(STREAM_POSITION);
	memcpy(pVertices,m_ppSubMeshes[iCurrentSubMeshNum]->pfVertices,sizeof(float)*m_ppSubMeshes[iCurrentSubMeshNum]->iVerticesNum);

	pTexCoords = (CVec2f*) m_ppVertexBuffers[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD0);
	memcpy(pTexCoords,m_ppSubMeshes[iCurrentSubMeshNum]->pfTexCoords,sizeof(float)*m_ppSubMeshes[iCurrentSubMeshNum]->iTexCoordsNum);

	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		pTangents = (CVec3f*) m_ppVertexBuffers[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD1);
		//memcpy(pTangents,m_spSharedModel->GetSubMesh(iCurrentSubMeshNum)->pfVertices,sizeof(float)*m_spSharedModel->GetSubMesh(iCurrentSubMeshNum)->iVerticesNum);

		pBinormals = (CVec3f*) m_ppVertexBuffers[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD2);
		//memcpy(pBinormals,m_spSharedModel->GetSubMesh(iCurrentSubMeshNum)->pfVertices,sizeof(float)*m_spSharedModel->GetSubMesh(iCurrentSubMeshNum)->iVerticesNum);

		pNormals = (CVec3f*) m_ppVertexBuffers[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD3);


		//CVec3f* pNormals = new CVec3f[m_spSharedModel->GetSubMesh(iCurrentSubMeshNum)->iVerticesNum];

		m_ppVertexBuffers[iCurrentSubMeshNum]->CalculateTBN(m_ppSubMeshes[iCurrentSubMeshNum]->iIndicesNum,pIndices,0,m_ppSubMeshes[iCurrentSubMeshNum]->iVerticesNum/3,
			pVertices,pTexCoords,pNormals,pTangents,pBinormals);
	}
		
	m_ppVertexBuffers[iCurrentSubMeshNum]->Unlock();
}

return true;
}

bool CStaticModel_Shared::Generate(DWORD dwGenerate, int iTessLevel,float fScaleX, float fScaleY, float fScaleZ,DWORD dwGeneratePPL)
{
	//TODO: support other types,
	//      and have some switch on the dwGenerate type

	m_iSubMeshesNum = 1;

	m_ppSubMeshes = new tSubMesh_3DS*[m_iSubMeshesNum];
    m_ppSubMeshes[0] = new tSubMesh_3DS;
	char pcTempName[500];
	sprintf(pcTempName,"StaticModel_TessQuad_Level%u",iTessLevel);

	m_ppSubMeshes[0]->pcName = new char[strlen(pcTempName)+1];
	strcpy(m_ppSubMeshes[0]->pcName,pcTempName);


	if (iTessLevel<2)
		iTessLevel = 2;

	/////////////////////////
	// generate vertices
	/////////////////////////

	int iVertsNum = iTessLevel*2;

	m_ppSubMeshes[0]->iVerticesNum = iVertsNum*3;
	m_ppSubMeshes[0]->pfVertices = new float[iVertsNum*3];

	m_ppSubMeshes[0]->iTexCoordsNum = iVertsNum*2;	
	m_ppSubMeshes[0]->pfTexCoords = new float[iVertsNum*2];

	float fSizeX = fScaleX;
	float fSizeY = fScaleY;

	float fTessLevel = (float) iTessLevel;
	float fI=0.f;

	for (int i=0;i<iTessLevel;i++)
	{
		fI = (float) i;
		m_ppSubMeshes[0]->pfVertices[i*3+0] = (fI/(fTessLevel-1.f))* fSizeX;
		m_ppSubMeshes[0]->pfVertices[i*3+1] = fSizeY+10.f;
		m_ppSubMeshes[0]->pfVertices[i*3+2] = 0.f;

		// tex coord
		m_ppSubMeshes[0]->pfTexCoords[i*2+0]=(fI/(fTessLevel-1.f));
		m_ppSubMeshes[0]->pfTexCoords[i*2+1]=0.f;
	}

	for (int i=0;i<iTessLevel;i++)
	{
		fI = (float) i;
		m_ppSubMeshes[0]->pfVertices[(i+iTessLevel)*3+0] = (fI/(fTessLevel-1.f))* fSizeX;
		m_ppSubMeshes[0]->pfVertices[(i+iTessLevel)*3+1] = -fSizeY+10.f;
		m_ppSubMeshes[0]->pfVertices[(i+iTessLevel)*3+2] = 0.f;

		// tex coord
		m_ppSubMeshes[0]->pfTexCoords[(i+iTessLevel)*2+0]=(fI/(fTessLevel-1.f));
		m_ppSubMeshes[0]->pfTexCoords[(i+iTessLevel)*2+1]=1.f;
		
	}

	m_BBox.min.Set(-fSizeX,-fSizeY,0.f);
	m_BBox.max.Set(fSizeX,fSizeY,0.f);


	
	///////////////////////
	// generate faces
	///////////////////////

	int iFacesNum = (iTessLevel-1)*2;

	m_ppSubMeshes[0]->iIndicesNum = iFacesNum*3;                
	m_ppSubMeshes[0]->uiIndices = new unsigned int[iFacesNum*3];

	for (int i=0;i<iTessLevel-1;i++)
	{
		m_ppSubMeshes[0]->uiIndices[i*3+0] = i+1+iTessLevel;
		m_ppSubMeshes[0]->uiIndices[i*3+1] = i+1;
		m_ppSubMeshes[0]->uiIndices[i*3+2] = i;

		/*m_ppSubMeshes[0]->uiIndices[i*3+0] = i;
		m_ppSubMeshes[0]->uiIndices[i*3+1] = i+1;
		m_ppSubMeshes[0]->uiIndices[i*3+2] = i+1+iTessLevel;*/
	}

	for (int i=0;i<iTessLevel-1;i++)
	{
		m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+0] = i;
		m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+1] = i+iTessLevel;
		m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+2] = i+iTessLevel+1;

		/*m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+0] = i+iTessLevel+1;
		m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+1] = i+iTessLevel;
		m_ppSubMeshes[0]->uiIndices[(i+iTessLevel-1)*3+2] = i;*/
	}
	


	
		/*	//  gl and d3d have flipped coordinate system when it comes to the y coordinate...
			if (g_pGraphicsAPI)
				if (g_pGraphicsAPI->GetHandness()==API_LEFT_HANDED)
				{
					float fTemp;
					for (int i=0;i<usNum;i++)
					{
						ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords[i*2+1] = 1.f - ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords[i*2+1];
					}
				}
*/

	


	//TODO: fix me
	GenerateBuffers(RENDERABLE_GENERATE_PPL);

	char message[300];
	sprintf(message,"////CStaticModel::Finished Generation of tesselated quad (tess level %i)",iTessLevel);
	g_pLogFile->OutPutPlainText(message,"4","448800",true,LOG_MESSAGE_INFORMATIVE);

	
	return true;
}

bool CStaticModel_Shared::Load3DS(const char* pcFileName,float fScaleX, float fScaleY, float fScaleZ)
{
	char message[300];
	sprintf(message,"CStaticModel::Load3DS loading %s model...",pcFileName);
	g_pLogFile->OutPutPlainText(message,"4","448800",true,LOG_MESSAGE_INFORMATIVE);

	m_iSubMeshesNum=0;
	tSubMesh_3DS** ppSubMeshes = new tSubMesh_3DS*[SUB_MESHES_MAX_3DS];



	m_BBox.min.Set(999999.f,999999.f,999999.f);
	m_BBox.max.Set(-999999.f,-999999.f,-999999.f);

	FILE* pFile;
    unsigned short usChunkId;
    unsigned int   uiChunkLength;
    unsigned char  cChar;
    unsigned short usNum;
    unsigned short usFaceFlags;


	float* pfVerticesBuffer=NULL;
	float* pfTextureCoords=NULL;
	

	unsigned short* pUSIndices = NULL;

	if ((pFile=fopen (pcFileName, "rb"))== NULL)
	{
		char temp[500];
		sprintf(temp,"CStaticModel::Load3DS loading of %s Failed!",pcFileName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_INFORMATIVE);

		return false;
	}


	m_pcName = new char[strlen(pcFileName)+1];
	strcpy(m_pcName,pcFileName);

    while (ftell (pFile) < filelength (fileno (pFile))) //Loop to scan the whole file 
    {

		fread (&usChunkId, 2, 1, pFile); //Read the chunk header
        fread (&uiChunkLength, 4, 1, pFile); //Read the length of the chunk

      switch (usChunkId)
      {
         case 0x4d4d: // Main chunk! do nothing.
         break;

		 case 0x3d3d: // 3D editor chunk! do nothing.
         break;

		 case 0x4000: // Object Block
			 {
				 m_iSubMeshesNum++;

				 ppSubMeshes[m_iSubMeshesNum-1] = new tSubMesh_3DS;

				char pcTempName[500];

				int i=0;
				do
				{					
					fread (&cChar, 1, 1, pFile);					
					pcTempName[i] = cChar;
				i++;
				}while(cChar != '\0' && i<500);

				ppSubMeshes[m_iSubMeshesNum-1]->pcName = new char[strlen(pcTempName)+1];
				strcpy(ppSubMeshes[m_iSubMeshesNum-1]->pcName,pcTempName);

			}
         break;

  	    case 0x4100: // OBJ_TRIMESH
			break;

		case 0x4110: // Position
			{
				fread (&usNum, sizeof (unsigned short), 1, pFile);
				
				ppSubMeshes[m_iSubMeshesNum-1]->iVerticesNum = usNum*3;
				ppSubMeshes[m_iSubMeshesNum-1]->pfVertices = new float[usNum*3];
				fread(ppSubMeshes[m_iSubMeshesNum-1]->pfVertices,sizeof(float)*usNum*3,1,pFile);

				
				float fTemp;

				for (int i=0;i<usNum;i++)
				{
					fTemp = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1];
					ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1] = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2];
					ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2] = fTemp;





					ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+0]*=fScaleX;
					ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1]*=fScaleY;
					ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2]*=fScaleZ;

					if (m_BBox.max.x < ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+0])
						m_BBox.max.x = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+0];

					if (m_BBox.max.y < ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1])
						m_BBox.max.y = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1];

					if (m_BBox.max.z < ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2])
						m_BBox.max.z = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2];


					if (m_BBox.min.x > ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+0])
						m_BBox.min.x = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+0];

					if (m_BBox.min.y > ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1])
						m_BBox.min.y = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+1];

					if (m_BBox.min.z > ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2])
						m_BBox.min.z = ppSubMeshes[m_iSubMeshesNum-1]->pfVertices[i*3+2];

				}

				////
	
			}
   	    break;

		
		case 0x4120: // Faces List (each face is a 3 unsigned short indices - tri)
			{
				fread (&usNum, sizeof (unsigned short), 1, pFile);
				ppSubMeshes[m_iSubMeshesNum-1]->iIndicesNum = usNum*3;
                
				ppSubMeshes[m_iSubMeshesNum-1]->uiIndices = new unsigned int[usNum*3];

				unsigned short usTempTri[3];

                for (int i=0; i<usNum; i++)
                {


					
					fread(usTempTri,sizeof(unsigned short)*3,1,pFile);

					ppSubMeshes[m_iSubMeshesNum-1]->uiIndices[i*3+0] = usTempTri[0];
					ppSubMeshes[m_iSubMeshesNum-1]->uiIndices[i*3+1] = usTempTri[1];
					ppSubMeshes[m_iSubMeshesNum-1]->uiIndices[i*3+2] = usTempTri[2];

					
					fread (&usFaceFlags, sizeof (unsigned short), 1, pFile);
				}
				
			}
         break;

		 case 0x4140: // mapping coordinates
            fread (&usNum, sizeof (unsigned short), 1, pFile);

			ppSubMeshes[m_iSubMeshesNum-1]->iTexCoordsNum = usNum*2;
			
			ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords = new float[usNum*2];

			fread(ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords,sizeof(float)*usNum*2,1,pFile);
	
			// for some reason i have flipped s/t coordinate space for gl and d3d...
/*			if (g_pGraphicsAPI)
				if (g_pGraphicsAPI->GetHandness()==API_LEFT_HANDED)
				{
					float fTemp;
					for (int i=0;i<usNum;i++)
					{
						ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords[i*2+1] = 1.f - ppSubMeshes[m_iSubMeshesNum-1]->pfTexCoords[i*2+1];
					}
				}*/
				


         break;
			
		 default:
				 fseek(pFile, uiChunkLength-6, SEEK_CUR);
	  }



	}


	m_ppSubMeshes = new tSubMesh_3DS*[m_iSubMeshesNum];

	for (int i=0;i<m_iSubMeshesNum;i++)
		m_ppSubMeshes[i] = ppSubMeshes[i];

	SafeDeleteArray(ppSubMeshes);

	///////////////////////////////////////////////////////////
	//Center Object

	// Find total vertices count

	unsigned int uiTotalVertices = 0;
	for (int i=0;i<m_iSubMeshesNum;i++)
	{
		for (int j=0;j<m_ppSubMeshes[i]->iVerticesNum;j++)
		{
			uiTotalVertices++;
		}
	}

	// Find Center
	CVec3f center(0.f,0.f,0.f);
	for (int i=0;i<m_iSubMeshesNum;i++)
	{
		for (int j=0;j<m_ppSubMeshes[i]->iVerticesNum;j++)
		{
			switch (j%3)
			{
			case 0:
				center.x+= m_ppSubMeshes[i]->pfVertices[j];
				break;
			case 1:
				center.y+= m_ppSubMeshes[i]->pfVertices[j];
				break;
			default: //2
				center.z+= m_ppSubMeshes[i]->pfVertices[j];
				break;
			}	
		}
	}

	center *= 1.f / float(uiTotalVertices/3);

	center *= -1.0f;

	// Move all vertices in a way that center of the object will be the center of the axis
	for (int i=0;i<m_iSubMeshesNum;i++)
	{
		for (int j=0;j<m_ppSubMeshes[i]->iVerticesNum;j++)
		{
			switch (j%3)
			{
			case 0:
				m_ppSubMeshes[i]->pfVertices[j] += center.x;
				break;
				
			case 1:
				m_ppSubMeshes[i]->pfVertices[j] += center.y;
				break;
				
			default: //2
				m_ppSubMeshes[i]->pfVertices[j] += center.z;
				break;
				
			}	
		
		}
	}

	GenerateBuffers(RENDERABLE_GENERATE_PPL);

	sprintf(message,"////CStaticModel::Load3DS loading of %s model Finished.",pcFileName);
	g_pLogFile->OutPutPlainText(message,"4","448800",true,LOG_MESSAGE_INFORMATIVE);

	return true;
}