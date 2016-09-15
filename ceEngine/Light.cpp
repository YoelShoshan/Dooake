#include "stdafx.h"
#include "Light.h"

#include "ce.h"

#include "MatrixGenerator.h"

#include "RendererBackend.h"
extern CRendererBackend* g_pRendererBack;

CLight::CLight()
{
	SetPos(CVec3f(0.f,0.f,0.f));
	SetColor(CVec3f(1.f,1.f,1.f));
	SetRadius(1000.f);
	
	
	SetLookAt(CVec3f(0.f,0.f,-1.f));

	m_bActive = true;


	m_pRTTDepthMap = g_pGraphicsAPI->GenerateRenderToTexture();

	static int iID = 0;
	char name[100];
	printf(name,"light%i",iID);


	/*int iResX = 1024;
	int iResY = 1024;*/

	/*int iResX = 2048;
	int iResY = 2048;*/

	int iResX = 1024;
	int iResY = 768;

	m_pRTTDepthMap->Init(name,CR_RTT_SHADOW_MAP,iResX,iResY);

	//m_pRTTDepthMap->Init(name,CR_RTT_COLOR_AND_DEPTH,g_pGraphicsAPI->GetResX(),g_pGraphicsAPI->GetResY());

	iID++;

	SetUp(CVec3f(0.f,1.f,0.f));
	//SetFOV(45.f);
	SetFOV(90.f);
	SetAspectRatio((float) iResX/ (float) iResY);

	//SetFrustumNearPlane(CR_NEAR_Z);
	SetFrustumNearPlane(2.f);

	// restore
	//SetFrustumFarPlane(CR_FAR_Z);
	
	// debug
	SetFrustumFarPlane(700.f);

	//SetFrustumFarPlane(3000);
	
	//m_pfModelViewProj = new float[16];
	m_pfView = new float[16];
	m_pfProjection = new float[16];
	m_pfViewProjection = new float[16];

	CMatrix ident;
	ident.MatMakeUnit();
	ident.ReturnArray(m_pfView);
	ident.ReturnArray(m_pfProjection);

	m_bDirty = true;
	
}

CLight::~CLight()
{	
	SafeDeleteArray(m_pfView);
	SafeDeleteArray(m_pfProjection);	
	SafeDeleteArray(m_pfViewProjection);
}

void CLight::SetActive(bool bActive)
{
	m_bActive = bActive;
}

bool CLight::GetActive()
{
	return m_bActive;
}

void CLight::BindRenderToDepthMap()
{
	m_pRTTDepthMap->BindRenderTo();
}

void CLight::UnbindRenderToDepthMap()
{
	m_pRTTDepthMap->UnbindRenderTo();
}

/*void CLight::BindDepthMap(int iSamplerNum)
{
	m_pRTTDepthMap->Bind(iSamplerNum);
}*/

IRenderToTexture* CLight::GetDepthMapRTT()
{
	return m_pRTTDepthMap;
}

CVec3f CLight::GetPos() const
{
	return m_vPos;
}

CVec3f CLight::GetColor() const
{
	return m_vColor;
}

void CLight::SetPos(const CVec3f& vPos)
{
	m_vPos = vPos;	
}

void CLight::ClearVisibleRenderables()
{
	m_VisibleRenderables.clear();

}

void CLight::AddVisibleRenderable(const IRenderable_Export* pRend)
{
	m_VisibleRenderables.push_back(pRend);
}

/*const std::vector<const IRenderable_Export*>* CLight::GetVisibleRenderables() const
{
	return &m_VisibleRenderables;
}*/




void CLight::SetColor(const CVec3f& vColor)
{
	m_vColor = vColor;
}


void CLight::SetLookAt(const CVec3f& vLookAt)
{
	m_vLookAt = vLookAt;	
	m_vLookAt.Normalize();
}

CVec3f CLight::GetLookAt() const
{
	return m_vLookAt;
}


void CLight::SetUp(const CVec3f& vUp)
{
	m_vUp = vUp;
}

void CLight::SetFOV(float fFov)
{
	m_fFov = fFov;

}

void CLight::SetAspectRatio(float fAR)
{
	m_fAR = fAR;

}

void CLight::SetFrustumNearPlane(float fVal)
{
	m_fNearPlane = fVal;
}

void CLight::SetFrustumFarPlane(float fVal)
{
	m_fFarPlane = fVal;
}

void CLight::Update()
{		
	SetRadius(m_fNearPlane + m_fFarPlane);

	CMatrixGenerator::LookAtRH(m_pfView,m_vPos.v,(m_vPos+m_vLookAt).v,m_vUp.v);
	CMatrixGenerator::PespectiveRH(m_pfProjection,m_fFov,m_fAR,m_fNearPlane, m_fFarPlane);
	
	CMatrix mView;
	mView.Set(m_pfView);
	
	CMatrix mProjection;
	mProjection.Set(m_pfProjection);

	//(mView*mProjection).ReturnArray(m_pfViewProjection);
	(mProjection*mView).ReturnArray(m_pfViewProjection);



	// calculate frustum
	float pfTempLightView[16];
	float pfTempLightProjection[16];


	CMatrix mLightView,mLightProjection;
	mLightView.SetTransposed(m_pfView);
	mLightView.ReturnArray(pfTempLightView);
	mLightProjection.SetTransposed(m_pfProjection);
	mLightProjection.ReturnArray(pfTempLightProjection);


	// calculate lights frustum
	m_Frustum.CalculateFrustum(pfTempLightView,pfTempLightProjection);

	// calculate lights frustum corners - used in scissor recting
	m_Frustum.CalculateCornerPoints(pfTempLightView,pfTempLightProjection);


	SetDirty(true);
}

const float* CLight::GetProjectionMatrix()
{
	return m_pfProjection;
}

const float* CLight::GetViewMatrix()
{
	return m_pfView;
}

const float* CLight::GetViewProjectionMatrix()
{
	return m_pfViewProjection;
}

CFrustum* CLight::GetFrustum(void)
{
	return &m_Frustum;
}