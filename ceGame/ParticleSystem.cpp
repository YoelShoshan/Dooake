#include "stdafx.h"
#include "ParticleSystem.h"

#include "GMath.h"
#include <math.h>

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;


CVec3f* CParticleSystem::s_pVertices_Temp = NULL;
DWORD* CParticleSystem::s_pColors_Temp = NULL;
CVec2f* CParticleSystem::s_pTexcoords_Temp = NULL;
unsigned int CParticleSystem::s_uiVertices_Temp_Size = 0;

CParticleSystem::CParticleSystem()
{
	m_bParentEntityDied = false;
	m_bCleanMe = false;
	// start velocity range
	/*m_vStartVelocityRange_Min.Set(-10,-10,-10);
	m_vStartVelocityRange_Max.Set(10,10,10);*/

	

	/*m_vStartVelocityRange_Min.Set(0,0,0);
	m_vStartVelocityRange_Max.Set(0,0,0);*/

	// start time delay
	m_fStartTimeDelay = 0;


	m_iVertsNum = -1;
	

	// time till full first apperance
	//m_dTimeTillFullFirstAppearance = 500.f;
	//m_dTimeTillFullFirstAppearance = 1500.f;

	

	

	m_vOrigin.Set(0.f,0.f,0.f);

	//m_dTotalLifeTime = 20000;



	m_uiBindIndex = 0;

	m_bDying = false;
	m_bFinishedDying = false;

	m_pParticles = NULL;

	m_pRend = NULL;
	

	if (!s_pVertices_Temp)
	{
		s_uiVertices_Temp_Size = 15000;
		s_pVertices_Temp = new CVec3f[s_uiVertices_Temp_Size];
		s_pColors_Temp   = new DWORD[s_uiVertices_Temp_Size];
		s_pTexcoords_Temp = new CVec2f[s_uiVertices_Temp_Size];
		for (int i=0;i<s_uiVertices_Temp_Size;)
		{
			s_pTexcoords_Temp[i].Set(0.f,0.f);
			s_pTexcoords_Temp[i+1].Set(0.f,1.f);
			s_pTexcoords_Temp[i+2].Set(1.f,1.f);
			s_pTexcoords_Temp[i+3].Set(1.f,0.f);
			i+=4;
		}						
	}
	//s_pVertices_Temp = NULL;

	

	m_uiParticlesAppeared = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// create renderable

	m_pRend = new CRenderable();
	m_pRend->SetMaterial(g_pRendererBack->LoadMaterial("Materials/ParticleSystems/hellholefire.utm"));

	m_pVB = NULL;

	
	m_pcName = NULL;
	
};


CParticleSystem::~CParticleSystem()
{

	//SafeDeleteArray(m_pVertices_Temp);

	//SafeDeleteArray(m_pColors_Temp);

	//SafeDelete(m_pVB);

	SafeDelete(m_pRend);

	//SafeDeleteArray(m_pVertices_Temp);

	



};

float CParticleSystem::GetRandFloatInRange(float fMin,float fMax)
{
	if (fMin>fMax)
	{
		float fTemp = fMin;
		fMin = fMax;
		fMax = fTemp;
	}

	float fRange = fabs(fMin-fMax);
	
	fRange *= RANDOM_NUM;

	fRange += fMin;

	return fRange;
}



/*void CParticleSystem::SetTimeTillFullFirstAppear(double dTime)
{
	m_dTimeTillFullFirstAppearance = dTime*1000.f;
}*/

void CParticleSystem::SetShared_Helper(const CParticleSystem_Shared* pShared)
{
	m_pShared = (CParticleSystem_Shared*) pShared;

	m_pRend->SetIndexBuffer(pShared->GetIB(),false);
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create the VERTEX BUFFER

	m_pVB = g_pRendererBack->GenerateVertexBuffer();
	m_pVB->AddStream(STREAM_POSITION,CR_FLOAT3);
	m_pVB->AddStream(STREAM_TEXCOORD0,CR_FLOAT2);	
	m_pVB->AddStream(STREAM_COLOR,CR_BYTE4);	
	//m_pVB->AddStream(STREAM_TEXCOORD1,CR_FLOAT1);	
	
	m_iVertsNum = pShared->GetParticlesMax()*4;

	//m_pVB->AllocateStreams(iVertsNum,VB_MEM_GPU|VB_SINGLE,CR_STREAM_DYNAMIC);
	m_pVB->AllocateStreams(m_iVertsNum,VB_MEM_GPU|VB_SINGLE|CR_STREAM_DISCARD,CR_STREAM_DYNAMIC);


	m_pVB->Lock(/*CR_LOCK_WRITE_ONLY*/CR_LOCK_WRITE_ONLY_DISCARD);

	bool bBloom = m_pShared->GetBloom();
	
	DWORD* pColors_temp = new DWORD[m_iVertsNum];
	for (int i=0;i<m_iVertsNum;i++)
	{	
		//pColors_temp[i] = 0x00FF00FF;
		//pColors_temp[i] = 0x0000FF00; //green
		//pColors_temp[i] = 0x000000FF; //red
		//pColors_temp[i] = 0x00FF0000; // blue

		//pColors_temp[i] = 0xFF00FFFF; // blue

		if (bBloom)
			pColors_temp[i] = 0xFFFFFFFF; 
		else
			pColors_temp[i] = 0x00FFFFFF; 

		//pColors_temp[i] = 0;
	}

	DWORD* pColors = (DWORD*) m_pVB->GetStream(STREAM_COLOR);
	memcpy(pColors,pColors_temp,sizeof(DWORD)*m_iVertsNum);

	/*float* pColors_temp = new float[iVertsNum];
	for (int i=0;i<iVertsNum;i++)
	{	
		pColors_temp[i] = 1.f;
		//pColors_temp[i] = 0.f;
	}

	float* pColors = (float*) m_pVB->GetStream(STREAM_TEXCOORD1);
	memcpy(pColors,pColors_temp,sizeof(unsigned char)*iVertsNum);*/


	/*CVec2f* pTexCoord_temp = new CVec2f[iVertsNum];
	for (int i=0;i<pShared->GetParticlesMax();i++)
	{
		pTexCoord_temp[4*i].Set(0.f,0.f);
		pTexCoord_temp[4*i+1].Set(0.f,1.f);
		pTexCoord_temp[4*i+2].Set(1.f,1.f);
		pTexCoord_temp[4*i+3].Set(1.f,0.f);
	}*/

	CVec2f* pTexCoords = (CVec2f*) m_pVB->GetStream(STREAM_TEXCOORD0);

	//memcpy(pTexCoords,pTexCoord_temp,sizeof(CVec2f)*iVertsNum);

	memcpy(pTexCoords,s_pTexcoords_Temp,sizeof(CVec2f)*m_iVertsNum);

	m_pVB->Unlock();

	m_pRend->SetVertexBuffer(m_pVB,false);

	m_pRend->SetMaterial(g_pRendererBack->LoadMaterial(m_pShared->GetMaterialName()));

	Init();
	

	

	// traverse all children

	const std::vector<CParticleSystem_Shared*>* pChildren = pShared->GetChildren();

	for (std::vector<CParticleSystem_Shared*>::const_iterator it=pChildren->begin();it!=pChildren->end();++it)
	{
		CParticleSystem* pPS = new CParticleSystem();
		m_Children.push_back(pPS);

		pPS->SetShared_Helper((*it));

	}



}

void CParticleSystem::SetShared(CSmartPointer<CParticleSystem_Shared> pShared)
{
	SetShared_Helper((const CParticleSystem_Shared*) pShared);
	
	
	m_pcName = new char[strlen(pShared->GetName())+1];
	strcpy(m_pcName,pShared->GetName());
}

void CParticleSystem::SetOrigin(const CVec3f& vOrigin)
{
	m_vOrigin = vOrigin;

}





void CParticleSystem::Init(void)
{
	m_dCreationTime = g_pTimer->GetTime();

	// allocate and init the particles

	m_pParticles = new tParticle[m_pShared->GetParticlesMax()];
		
	if (s_uiVertices_Temp_Size < m_pShared->GetParticlesMax()*4)
	{
		s_uiVertices_Temp_Size = m_pShared->GetParticlesMax()*4;
		SafeDeleteArray(s_pVertices_Temp);
		s_pVertices_Temp = new CVec3f[s_uiVertices_Temp_Size];

		SafeDeleteArray(s_pColors_Temp);
		s_pColors_Temp = new DWORD[s_uiVertices_Temp_Size];

		SafeDeleteArray(s_pTexcoords_Temp);
		s_pTexcoords_Temp = new CVec2f[s_uiVertices_Temp_Size];

		for (int i=0;i<s_uiVertices_Temp_Size;)
		{
			s_pTexcoords_Temp[i].Set(0.f,0.f);
			s_pTexcoords_Temp[i+1].Set(0.f,1.f);
			s_pTexcoords_Temp[i+2].Set(1.f,1.f);
			s_pTexcoords_Temp[i+3].Set(1.f,0.f);
			i+=4;
		}		
	}


	

	for (int i=0;i<m_pShared->GetParticlesMax();i++)
	{
		//m_pParticles[i].fLife = GetRandFloatInRange(m_fLifeTimeRange_Min,m_fLifeTimeRange_Max);
		m_pParticles[i].fLife = -1.f;
		m_pParticles[i].vVelocity.Set(GetRandFloatInRange(m_pShared->GetStartVelRangeMin().x,m_pShared->GetStartVelRangeMax().x),
			GetRandFloatInRange(m_pShared->GetStartVelRangeMin().y,m_pShared->GetStartVelRangeMax().y),
			GetRandFloatInRange(m_pShared->GetStartVelRangeMin().z,m_pShared->GetStartVelRangeMax().z));
		m_pParticles[i].vPos = m_vOrigin;
	}




	

	
}

void CParticleSystem::Update(float fDeltaTime,bool bVisible)
{
	// debug
	//return;

	if (!m_pShared)
		return;


	if (m_pShared->GetRotateAroundOrigin())
	{
		CVec3f vStartPos = m_pShared->GetStartRotationDistance();

		CMatrix mRotX,mRotY,mRotZ;
		

		double dTimeSinceCreation = g_pTimer->GetTime() - m_dCreationTime;

		float fSpeed = m_pShared->GetRotationSpeed()*dTimeSinceCreation;

		if (m_pShared->GetRotateAroundX())
			mRotX.MatMakeXRot(DEG2RAD(fSpeed));
		else
			mRotX.MatMakeUnit();

		if (m_pShared->GetRotateAroundY())
			mRotY.MatMakeYRot(DEG2RAD(fSpeed));
		else
			mRotY.MatMakeUnit();

		if (m_pShared->GetRotateAroundZ())
			mRotZ.MatMakeZRot(DEG2RAD(fSpeed));
		else
			mRotZ.MatMakeUnit();

		vStartPos = (mRotX * mRotY * mRotZ) * vStartPos;

		m_vOrigin += vStartPos;

	}







	unsigned int uiParticlesMax = m_pShared->GetParticlesMax();

	unsigned int uiMaxAllowed = uiParticlesMax;
	// make particles appear gradually
	double dTimeSinceCreation = g_pTimer->GetTime() - m_dCreationTime;
	if (m_uiParticlesAppeared<uiParticlesMax)
	{
		double dParticlesAppearNow = (dTimeSinceCreation / m_pShared->GetTimeTillFullAppear())*((float)uiParticlesMax);
		uiMaxAllowed = (unsigned int)dParticlesAppearNow;
	}
	

	if (uiMaxAllowed>uiParticlesMax)
		uiMaxAllowed = uiParticlesMax;


	double dTotalLifeTime = m_pShared->GetTotalLifeTime();

	// check if should start dying

	if (dTotalLifeTime!=-1.0)
	if (!m_bDying && dTimeSinceCreation>=dTotalLifeTime)
	{
		//g_TextMessages.AddMessage("Particle System Started Dying",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		m_bDying = true;
	}



	// rotate a template particle quad to face the camera

	CMatrix mRot;

	mRot.Set(g_pGraphicsAPI->GetViewMatrix());
	
	mRot.m[0][3] = 0.f;
	mRot.m[1][3] = 0.f;
	mRot.m[2][3] = 0.f;
	mRot.m[3][3] = 1.f;

	mRot.Transpose();

	//float fSize = 50.f;
	float fSize = m_pShared->GetParticleSize();

	CVec3f quad[4];
	quad[0].Set(-fSize,fSize,0.f);
	quad[1].Set(fSize,fSize,0.f);
	quad[2].Set(fSize,-fSize,0.f);
	quad[3].Set(-fSize,-fSize,0.f);

	for (int i=0;i<4;i++)
	{
		quad[i] = mRot*quad[i];		
	}

	m_uiParticlesAppeared = 0;

	float fAlpha = 0.f;
	unsigned char cAlpha = 0;

	
	// update all of the particles physics
	for (int i=0;i<uiParticlesMax;i++)
	{
		if (m_uiParticlesAppeared>uiMaxAllowed)
			break;


		m_pParticles[i].fLife-=fDeltaTime;

		if (!m_bDying && m_pParticles[i].fLife<=0)
		{
			m_pParticles[i].fLife = GetRandFloatInRange(m_pShared->GetLifeTimeRangeMin(),m_pShared->GetLifeTimeRangeMax());

			m_pParticles[i].vVelocity.Set(GetRandFloatInRange(m_pShared->GetStartVelRangeMin().x,m_pShared->GetStartVelRangeMax().x),
				GetRandFloatInRange(m_pShared->GetStartVelRangeMin().y,m_pShared->GetStartVelRangeMax().y),
				GetRandFloatInRange(m_pShared->GetStartVelRangeMin().z,m_pShared->GetStartVelRangeMax().z));

			// TODO: make this tweakable
			
			//m_pParticles[i].vPos = m_vOrigin+CVec3f(GetRandFloatInRange(-17.f,17.f),GetRandFloatInRange(-6.f,6.f),GetRandFloatInRange(-17.f,17.f));
			m_pParticles[i].vPos = m_vOrigin+CVec3f(
				GetRandFloatInRange(m_pShared->GetStartPosOffsetRangeMin().x,m_pShared->GetStartPosOffsetRangeMax().x),
				GetRandFloatInRange(m_pShared->GetStartPosOffsetRangeMin().y,m_pShared->GetStartPosOffsetRangeMax().y),
				GetRandFloatInRange(m_pShared->GetStartPosOffsetRangeMin().z,m_pShared->GetStartPosOffsetRangeMax().z));
			
		}

		if (m_bDying && m_pParticles[i].fLife<=0)
			continue;

		m_pParticles[i].vVelocity += m_pShared->GetAcceleration() * fDeltaTime;
		m_pParticles[i].vPos += m_pParticles[i].vVelocity * fDeltaTime;		

		s_pVertices_Temp[4*m_uiParticlesAppeared]   = m_pParticles[i].vPos+quad[0];
		s_pVertices_Temp[4*m_uiParticlesAppeared+1] = m_pParticles[i].vPos+quad[1];
		s_pVertices_Temp[4*m_uiParticlesAppeared+2] = m_pParticles[i].vPos+quad[2];
		s_pVertices_Temp[4*m_uiParticlesAppeared+3] = m_pParticles[i].vPos+quad[3];

	

		fAlpha = m_pParticles[i].fLife / m_pShared->GetLifeTimeRangeMax();
		cAlpha = (unsigned char) (fAlpha * 255.f);

		/*cAlpha = 255;

		if (m_pParticles[i].fLife < 0.3)
		{
			fAlpha = (m_pParticles[i].fLife-0.1) / 0.2;
			if (fAlpha<0.f)
				fAlpha=0.f;
			cAlpha = (unsigned char) (fAlpha * 255.f);
		}*/

		bool bBloom = m_pShared->GetBloom();
		DWORD dwMask = 0x00FFFFFF;

		for (int v=0;v<4;v++)		
		{
			memset(&s_pColors_Temp[4*m_uiParticlesAppeared+v],cAlpha,4);
			/*if (bBloom)
				s_pColors_Temp[4*m_uiParticlesAppeared+v]|=0xFF000000;
			else*/
			if (!bBloom)
				s_pColors_Temp[4*m_uiParticlesAppeared+v]&=dwMask;
		}



		m_uiParticlesAppeared++;		
	}


	if (m_bDying && !m_uiParticlesAppeared)
	{
		m_bFinishedDying = true;
		//test
		m_bCleanMe=true;
	}

	if (!bVisible)
		return;



	if (m_uiParticlesAppeared>0)
	{
	
		// update vertexbuffer
		m_pVB->Lock(/*CR_LOCK_WRITE_ONLY*/CR_LOCK_WRITE_ONLY_DISCARD);

		// update vertices
		CVec3f* pVerts = (CVec3f*) m_pVB->GetStream(STREAM_POSITION);
		memcpy(pVerts,s_pVertices_Temp,sizeof(CVec3f)*4*m_uiParticlesAppeared);

		// hack for now, since discard discards them... (they should be in a difference static VB)

		// update texcoords
		CVec2f* pTexCoords = (CVec2f*) m_pVB->GetStream(STREAM_TEXCOORD0);
		memcpy(pTexCoords,s_pTexcoords_Temp,sizeof(CVec2f)*4*m_uiParticlesAppeared);

		// update "colors"
		DWORD* pColors = (DWORD*) m_pVB->GetStream(STREAM_COLOR);
		memcpy(pColors,s_pColors_Temp,sizeof(DWORD)*4*m_uiParticlesAppeared);

		m_pVB->Unlock();
	}


	// traverse the children

	for (std::vector<CParticleSystem*>::iterator it=m_Children.begin();it!=m_Children.end();++it)
	{
		/*if (m_pShared->GetRotateAroundOrigin())
			(*it)->SetParentOrigin(m_vParentOrigin);
		else
		(*it)->SetParentOrigin(m_vOrigin);*/

		(*it)->SetOrigin(m_vOrigin);

		(*it)->Update(fDeltaTime,bVisible);
	}
	
}

/*void CParticleSystem::SetParentOrigin(const CVec3f& vParentOrigin)
{
	m_vParentOrigin = vParentOrigin;
}*/


void CParticleSystem::Render(int iTech)
{
	// debug
	//return;
	/*char temp[100];
	sprintf(temp,"particles num:%u",m_uiParticlesAppeared);
	g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

	if (m_uiParticlesAppeared>0)
	{
		m_pRend->SetDrawCallParameters(0,m_uiParticlesAppeared*6,0,0,m_iVertsNum);
		g_pRendererFront->Push(m_pRend);
	}


	// debug
	/*char msg[150];
	sprintf(msg,"Renderable material %s sort value:%d",m_pRend->GetMaterial()->GetName(),m_pRend->GetMaterial()->GetSort());
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/


	// traverse the children

	for (std::vector<CParticleSystem*>::iterator it=m_Children.begin();it!=m_Children.end();++it)
	{
		(*it)->Render(iTech);
	}

}