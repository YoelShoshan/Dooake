#include "stdafx.h"
#include "StaticModel.h"


#include "ce.h"

//#include "crtdbg.h"

#include "assert.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "StaticModelManager.h"


//#include "MaterialManager.h"

/*#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;*/

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;

#include "Renderable.h"

#include "Material_IE.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;


CStaticModel::CStaticModel()
{	
	m_vAnglesRotateSpeed.Set(0.f,0.f,0.f);
	m_spSharedModel=NULL;
	m_pRenderables=NULL;	
	m_pcName = NULL;
}
CStaticModel::~CStaticModel()
{
	SafeDeleteArray(m_pRenderables);		
}

void CStaticModel::SetShared(CSmartPointer<CStaticModel_Shared> spSet,DWORD dwGenerate)
{
	if (!spSet)
	{
		g_pLogFile->OutPutPlainText("CStaticModel::SetShared Trying to set NULL shared model!",LOG_MESSAGE_ERROR);
		return;
	}


	m_pcName = new char[strlen(spSet->GetName())+1];
	strcpy(m_pcName,spSet->GetName());

	m_spSharedModel = spSet;

	GenerateRenderable(dwGenerate);
	



}


// assumes a model was loaded!
bool CStaticModel::GenerateRenderable(DWORD dwGenerate)
{	
	UINT* pIndices;

	//assert(_CrtCheckMemory());

	m_pRenderables = new CRenderable[m_spSharedModel->GetSubMeshesNum()];

	//assert(_CrtCheckMemory());

for (int iCurrentSubMeshNum=0;iCurrentSubMeshNum<m_spSharedModel->GetSubMeshesNum();iCurrentSubMeshNum++)
{
	//assert(_CrtCheckMemory());

	//m_pRenderables[iCurrentSubMeshNum].SetBuffers(pVB,pIB);
	m_pRenderables[iCurrentSubMeshNum].SetVertexBuffer((IVertexBuffer_Export*)m_spSharedModel->GetVertexBuffer(iCurrentSubMeshNum),false);
	m_pRenderables[iCurrentSubMeshNum].SetIndexBuffer((IIndexBuffer*)m_spSharedModel->GetIndexBuffer(iCurrentSubMeshNum),false);


	m_pRenderables[iCurrentSubMeshNum].SetDrawCallParameters(
		0,
	    ((IIndexBuffer*)m_spSharedModel->GetIndexBuffer(iCurrentSubMeshNum))->GetLength(),
		0,0,
		((IVertexBuffer_Export*)m_spSharedModel->GetVertexBuffer(iCurrentSubMeshNum))->GetLength()
		);


	//assert(_CrtCheckMemory());

}

	return true;
}
	
void CStaticModel::Render(int iTech)
{
	//g_pRendererBack->Render(&m_pRenderables[0]);

	for (int i=0;i<m_spSharedModel->GetSubMeshesNum();i++)
		//g_pRendererBack->Render(&m_pRenderables[i],iTechniqueNum);
		g_pRendererFront->Push(&m_pRenderables[i]);

}


void CStaticModel::SetMaterialForAll(CSmartPointer<IMaterial_Export> spMat)
{
	for (int i=0;i<m_spSharedModel->GetSubMeshesNum();i++)
		m_pRenderables[i].SetMaterial(spMat);

}

void CStaticModel::SetTransformForAll(float* pfTransf)
{
	for (int i=0;i<m_spSharedModel->GetSubMeshesNum();i++)
		m_pRenderables[i].SetTransform(pfTransf);
}

void CStaticModel::ClearEffectingLights(void)
{
	for (int i=0;i<m_spSharedModel->GetSubMeshesNum();i++)
		m_pRenderables[i].ClearEffectingLights();
}

void CStaticModel::AddEffectingLight(ILight_Export* pLight)
{
	for (int i=0;i<m_spSharedModel->GetSubMeshesNum();i++)
		m_pRenderables[i].AddEffectingLight(pLight);
}

void CStaticModel::SetPosition(const CVec3f& vPos)
{
	m_vPosition = vPos;
}

void CStaticModel::SetAngles(const CVec3f& vAngles)
{
	m_vPreviousAngles = m_vCurrentAngles;

	m_vCurrentAngles = vAngles;

	m_vAngles = vAngles;
}

void CStaticModel::Update(float fDeltaTime,bool bVisible)
{

	if (!bVisible)
		return;

	float pfWorld[16];
	CMatrix trans,rot;

	CVec3f vAdd(0.f,0.f,0.f);

	// if it's rotating then assume that also a nice up/down sin movement is ok =)
	if (m_vAnglesRotateSpeed.x!=0.f || m_vAnglesRotateSpeed.y!=0.f || m_vAnglesRotateSpeed.z!=0.f)
	{
		float fTime =  g_pTimer->GetTimeFAKE();
		vAdd.y = 25.f+(sinf(fTime*0.006f)+0.5f)*7.5f;

	}

	CVec3f vFinalPos = m_vPosition+vAdd;

	// RESTORE RESTORE RESTORE
	trans.MatMakeMove(vFinalPos.x,
					  vFinalPos.y,
					  vFinalPos.z );

	m_vAngles+= m_vAnglesRotateSpeed*fDeltaTime;

	


	rot.MatMakeRot(DEG2RAD(m_vAngles.x),DEG2RAD(m_vAngles.y),DEG2RAD(m_vAngles.z));

	rot = trans*rot;

	rot.ReturnArray(pfWorld);
	SetTransformForAll(pfWorld);
}