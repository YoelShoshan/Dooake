#include "stdafx.h"
#include "Renderable.h"

#include "Defines.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

//#include "Engine.h"
#include "ce.h"


CRenderable::CRenderable()
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

	m_pBBox = NULL;
	m_bDontRender = false;

	m_bShadowCaster = true;

	m_pIndicesData = NULL;
}

CRenderable::~CRenderable()
{
	if (m_bFreeVB)
		SafeDelete(m_pVertexBuffer);

	if (m_bFreeIB)
		SafeDelete(m_pIndexBuffer);

}

// expects column based layout !
void CRenderable::SetTransform(const float* pMat)
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

const float* CRenderable::GetTransform(void) const
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

bool CRenderable::SetMaterial(CSmartPointer<IMaterial_Export> spMat)
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


bool CRenderable::SetVertexBuffer(const IVertexBuffer_Export* pVB,bool bFree)
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

bool CRenderable::SetIndexBuffer(const IIndexBuffer* pIB,bool bFree)
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

int CRenderable::GetIndicesStart(void)  const
{
	return m_iIndicesStart;	
}

int CRenderable::GetIndicesCount(void) const
{
	return m_iIndicesCount;
}

int CRenderable::GetVerticesNum(void) const
{
	return m_iVertsNum;
}

int CRenderable::GetBaseVertexIndex(void) const
{
	return m_iBaseVertexIndex;
}

int CRenderable::GetMinIndex(void) const
{
	return m_iMinIndex;
}


bool CRenderable::SetDrawCallParameters(int iStart, int iCount,int iBaseVertexIndex, int iMinIndex, int iVertsNum)
{
	m_iIndicesStart = iStart;
	m_iIndicesCount = iCount;
	m_iVertsNum		= iVertsNum;

	return true;
}

void	CRenderable::SetIndices_Data(const UINT* pIndices)
{
	m_pIndicesData = pIndices;
}

const UINT*	CRenderable::GetIndices_Data(void) const
{
//	assert(m_pIndicesData);
	return m_pIndicesData;
}

bool CRenderable::BindMaterial(void) const
{
	if (!m_spMaterial)
	{
		g_pLogFile->OutPutPlainText("CRenderable::BindMaterial::Trying to bind NULL Material!",LOG_MESSAGE_ERROR);
		return false;
	}

	m_spMaterial->Bind();

	return true;
}


void CRenderable::ClearEffectingLights()
{
	m_EffectingLights.clear();
}

void CRenderable::AddEffectingLight(ILight_Export* pLight)
{
	m_EffectingLights.insert(pLight);
}

const std::set<ILight_Export*>* CRenderable::GetEffectingLights() const
{
	return &m_EffectingLights;
}

const BBox* CRenderable::GetBBox(void) const
{
	return m_pBBox;

}

void CRenderable::SetBBox(BBox& bbox)
{
	if (!m_pBBox)
		m_pBBox = new BBox();

	(*m_pBBox) = bbox;
}

void CRenderable::SetDontRender(bool bDontRender)
{
	m_bDontRender = bDontRender;
}

bool CRenderable::GetDontRender(void) const
{
	return m_bDontRender;

}

unsigned int CRenderable::GetSort() const
{
	if (m_spMaterial)
		return m_spMaterial->GetSort();

	return MATERIAL_SORT_OPAQUE;

}