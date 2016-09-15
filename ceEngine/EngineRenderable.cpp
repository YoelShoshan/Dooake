#include "stdafx.h"
#include "EngineRenderable.h"

#include "Defines.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "RendererBackend.h"
extern CRendererBackend* g_pRendererBack;

//#include "Engine.h"
#include "ce.h"


CEngineRenderable::CEngineRenderable()
{
	m_pVertexBuffer      = NULL;
	m_pIndexBuffer       = NULL;

	m_Transform.MatMakeUnit();

	m_pfTransform = new float[16];

	m_Transform.ReturnArray(m_pfTransform);

	m_bTransformIsIdentity = true;

	m_iIndicesStart		= -1;
	m_iIndicesCount		= -1;
	m_iVertsNum			= -1;
	m_iBaseVertexIndex	= 0;
	m_iMinIndex			= 0;

	m_pIndicesData = NULL;

}

CEngineRenderable::~CEngineRenderable()
{
	if (m_bFreeVB)
		SafeDelete(m_pVertexBuffer);

	if (m_bFreeIB)
		SafeDelete(m_pIndexBuffer);

}

// expects column based layout !
void CEngineRenderable::SetTransform(const float* pMat)
{
	if (!pMat)
	{
		if (!m_bTransformIsIdentity)
		{
			m_Transform.MatMakeUnit();
			m_bTransformIsIdentity = true;
			m_Transform.ReturnArray(m_pfTransform);
		}
	}
	else
	{
		m_Transform.Set(pMat);
		m_bTransformIsIdentity = false;
		m_Transform.ReturnArray(m_pfTransform);
	}
		


}

const float* CEngineRenderable::GetTransform(void) const
{
	if (m_bTransformIsIdentity)
		return NULL;

	// DEBUG DEBUG DEBUG

	//return NULL;


	
	//m_Transform.ReturnArray(m_pfTransform);
	return m_pfTransform;
}
/*bool CRenderable::Validate(CSmartPointer<IMaterial_Export> spMat,const IVertexBuffer_Export* pVB)
{
	const tMaxVD* pMaterialMaxVD = ((const CMaterial* ) ((const IMaterial_Export*)spMat)) ->GetMaxVD();


	for (int i=0;i<STREAM_MAX;i++)
		{
			if (pMaterialMaxVD->elements[i].bActive)
			{	
				if (!pVB->IsStreamActive(i)) // the vertex decleration doesn't contain this stream!
				{
					char error[400];
					sprintf_s(error,400,"CRenderable::SetMaterial vertex buffer doesn't contain stream %u requested in material %s!",
						i,spMat->GetName());

					g_pLogFile->OutPutPlainText(error,LOG_MESSAGE_ERROR);
					return false;
				}

				if (pMaterialMaxVD->elements[i].chStreamDataType != pVB->GetStreamCRtype(i)) // the vertex decleration contains this stream with a different type!
				{
					char error[400];
					sprintf_s(error,400,"CRenderable::SetMaterial vertex buffer contains different data type for stream %u then material %s!",
						i,spMat->GetName());

					g_pLogFile->OutPutPlainText(error,LOG_MESSAGE_ERROR);
					return false;

				}

			}
	}

}*/

bool CEngineRenderable::SetMaterial(CSmartPointer<IMaterial_Export> spMat)
{
	if (!spMat)
		return false;
			
	/*if (m_pVertexBuffer)
	{		
		if (!Validate(spMat,m_pVertexBuffer))
			return false;
	}*/

	m_spMaterial = spMat;
	return true;
}


bool CEngineRenderable::SetVertexBuffer(const IVertexBuffer_Export* pVB,bool bFree)
{
	if (  (!pVB)  )
		return false;
	
	/*if (m_spMaterial)
	{
		
		if (!Validate(m_spMaterial,pVB))
			return false;
	}*/


	if (m_bFreeVB)
		SafeDelete(m_pVertexBuffer);
	
	m_pVertexBuffer      = (IVertexBuffer_Export*) pVB;
	m_bFreeVB = bFree;
	
	return true;
}

bool CEngineRenderable::SetIndexBuffer(const IIndexBuffer* pIB,bool bFree)
{	
	if (  (!pIB) )
		return false;

	if (m_bFreeIB)
		SafeDelete(m_pIndexBuffer);

	m_pIndexBuffer       = pIB;
	m_bFreeIB = bFree;


	// only do this if the user didn't use "SetDrawCallParameters"
	if (m_iIndicesCount<0 || m_iIndicesCount<0)
	{
		m_iIndicesStart = 0;
		m_iIndicesCount = m_pIndexBuffer->GetLength();
	}

	return true;

}

int CEngineRenderable::GetIndicesStart(void)  const
{
	return m_iIndicesStart;	
}

int CEngineRenderable::GetIndicesCount(void) const
{
	return m_iIndicesCount;
}

int CEngineRenderable::GetVerticesNum(void) const
{
	return m_iVertsNum;
}

int CEngineRenderable::GetBaseVertexIndex(void) const
{
	return m_iBaseVertexIndex;
}

int CEngineRenderable::GetMinIndex(void) const
{
	return m_iMinIndex;
}


bool CEngineRenderable::SetDrawCallParameters(int iStart, int iCount,int iBaseVertexIndex, int iMinIndex,int iVertsNum)
{
	m_iIndicesStart = iStart;
	m_iIndicesCount = iCount;
	m_iVertsNum		= iVertsNum;
	return true;
}

void	CEngineRenderable::SetIndices_Data(const UINT* pIndices)
{
	m_pIndicesData = pIndices;
}

const UINT*	CEngineRenderable::GetIndices_Data(void) const
{
	assert(m_pIndicesData);
	return m_pIndicesData;
}

unsigned int CEngineRenderable::GetSort(void) const
{
	if (m_spMaterial)
		return m_spMaterial->GetSort();

	else return MATERIAL_SORT_OPAQUE;
}

bool CEngineRenderable::BindMaterial(void) const
{
	if (!m_spMaterial)
	{
		g_pLogFile->OutPutPlainText("CRenderable::BindMaterial::Trying to bind NULL Material!",LOG_MESSAGE_ERROR);
		return false;
	}

	m_spMaterial->Bind();

	return true;
}

bool CEngineRenderable::Generate(DWORD dwType/*,int iTessLevel*/)
{
	switch (dwType)
	{
		case ENGINE_RENDERABLE_FULL_SCREEN_QUAD:
			GenerateFullScreenQuad();
		break;

/*		case ENGINE_RENDERABLE_TESSELATED_QUAD:
			GenerateTesselatedQuad(iTessLevel);
		break;*/

		case ENGINE_RENDERABLE_SPHERE:
			GenerateSphere();
		break;

		default:
			g_pLogFile->OutPutPlainText("CEngineRenderable::Generate::Unrecognized Generate Type!",LOG_MESSAGE_ERROR);
			return false;

		break;
	}

	return true;

}

bool CEngineRenderable::GenerateFullScreenQuad()
{
		/*glBegin (GL_QUADS); 

		glTexCoord2f(0.0f, 1.f);  glVertex3i (-1, 1, 1); 
		glTexCoord2f(1.0f, 1.f);  glVertex3i (1, 1, 1); 
		glTexCoord2f(1.0f, 0.f);  glVertex3i (1, -1, 1); 
		glTexCoord2f(0.0f, 0.f);  glVertex3i (-1, -1, 1);    

	         
		glEnd ();*/

	////////////////////////////////////////////////////////////////////////
	// create the vertex buffer

	IVertexBuffer_Export* pVertexBuffer = g_pRendererBack->GenerateVertexBuffer();

	pVertexBuffer->AddStream(STREAM_POSITION,CR_FLOAT3);
	pVertexBuffer->AddStream(STREAM_TEXCOORD0,CR_FLOAT2);		
		
	pVertexBuffer->AllocateStreams(4,VB_MEM_GPU|VB_SINGLE,CR_STREAM_STATIC);

	pVertexBuffer->Lock(CR_LOCK_WRITE_ONLY);	
	
	CVec3f* pVertices = (CVec3f*) pVertexBuffer->GetStream(STREAM_POSITION);	
	CVec2f* pTexCoords = (CVec2f*) pVertexBuffer->GetStream(STREAM_TEXCOORD0);



	

	/*float fShiftX = -3.0f / ((float) g_pGraphicsAPI->GetResX());
	float fShiftY = +2.5f / ((float) g_pGraphicsAPI->GetResY());*/

	//float fShiftX = -0.75f / ((float) g_pGraphicsAPI->GetResX());
	
	/*float fShiftX = -1.f / ((float) g_pGraphicsAPI->GetResX());
	float fShiftY = 1.f / ((float) g_pGraphicsAPI->GetResY());*/

	float fShiftX = 0.f;
	float fShiftY = 0.f;




	// RESTORE RESTORE RESTORE

	/*pVertices[0].Set(-1.f,1.f,1.f);
	pVertices[1].Set(1.f,1.f,1.f);
	pVertices[2].Set(1.f,-1.f,1.f);
	pVertices[3].Set(-1.f,-1.f,1.f);

	for (int i=0;i<4;i++)
		pVertices[i] = end * pVertices[i];*/


	pVertices[0].Set(-1.f+fShiftX,1.f +fShiftY,1.f);
	pVertices[1].Set(1.f +fShiftX,1.f +fShiftY,1.f);
	pVertices[2].Set(1.f +fShiftX,-1.f+fShiftY,1.f);
	pVertices[3].Set(-1.f+fShiftX,-1.f+fShiftY,1.f);


	{
		/*pTexCoords[0].Set(0.f+fShiftX,0.f+fShiftY);
		pTexCoords[1].Set(1.f+fShiftX,0.f+fShiftY);
		pTexCoords[2].Set(1.f+fShiftX,1.f+fShiftY);
		pTexCoords[3].Set(0.f+fShiftX,1.f+fShiftY);*/

		/*float fTCShiftX = 0.5f /  ((float) g_pGraphicsAPI->GetResX());
		float fTCShiftY = 0.5f /  ((float) g_pGraphicsAPI->GetResY());*/

		float fTCShiftX = 0.f;
		float fTCShiftY = 0.f;

		pTexCoords[0].Set(0.f+fTCShiftX,0.f+fTCShiftY);
		pTexCoords[1].Set(1.f+fTCShiftX,0.f+fTCShiftY);
		pTexCoords[2].Set(1.f+fTCShiftX,1.f+fTCShiftY);
		pTexCoords[3].Set(0.f+fTCShiftX,1.f+fTCShiftY);

		/*pTexCoords[0].Set(0.f+fShiftX,0.f+fShiftY);
		pTexCoords[1].Set(1.f+fShiftX,0.f+fShiftY);
		pTexCoords[2].Set(1.f+fShiftX,1.f+fShiftY);
		pTexCoords[3].Set(0.f+fShiftX,1.f+fShiftY);*/

		/*pTexCoords[0].Set(0.f,0.f);
		pTexCoords[1].Set(1.f,0.f);
		pTexCoords[2].Set(1.f,1.f);
		pTexCoords[3].Set(0.f,1.f);*/
	}

	// write the actual vertices data

	pVertexBuffer->Unlock();
	
	SetVertexBuffer(pVertexBuffer,false);

	////////////////////////////////////////////////////////////////////////
	// create ethe index buffer


	
	IIndexBuffer* pIndexBuffer = g_pRendererBack->GenerateIndexBuffer();
	//m_pIndexBuffer->Allocate(m_numOfIndices,IB_MEM_GPU | IB_STATIC);
	pIndexBuffer->Allocate(6,IB_MEM_GPU | IB_STATIC);
	pIndexBuffer->Lock(CR_LOCK_WRITE_ONLY,0,0);

	// write the actual indices data
	
	UINT* pIndices = (UINT*) pIndexBuffer->GetData();

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	pIndexBuffer->Unlock();

	SetIndexBuffer(pIndexBuffer,false);

	return true;

}

/*bool CEngineRenderable::GenerateTesselatedQuad(int iTessLevel)
{

	return true;
}*/

bool CEngineRenderable::GenerateSphere()
{


	return true;
}

