#include "stdafx.h"
#include "MD5Instance.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;

//#include "crtdbg.h"

#include "assert.h"

#include "Profiler.h"
#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

//#include "Timer_IE.h"
//#include "PerformanceTimer.h"
//#include "MaterialManager.h"
#include "StaticModelManager.h"

//#include "crtdbg.h"
#include "assert.h"



/*#include "Logics.h"
extern CLogics g_Logics;*/

#include "MD5Mesh.h"
#include "MD5Anim.h"

#include "Parser.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "Material_IE.h"


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

//#include "crtdbg.h"
#include "assert.h"


extern float        g_fDEBUGRotJointX;
extern float        g_fDEBUGRotJointY;
extern float        g_fDEBUGRotJointZ;
extern int          g_iDEBUGJoint;

CMD5Inst::CMD5Inst()
{
	
	m_ppJointModel = NULL;
	m_pSELECTEDJointModel=NULL;
	
	m_AnimatedRends = NULL;

	m_ppVerts = NULL;

	m_pTempTangentsArray = NULL;
	m_pTempBinormalsArray = NULL;
	m_pTempNormalsArray = NULL;


	m_ppAnimatedVB = NULL;


	m_pCurrentBindJoints = NULL;

	m_pCurrentJointsPositions = NULL;

	m_pDebugJointsFKRot = NULL;

	//m_bFKDebug = true;

	m_bFKDebug = false;

	

	m_bRenderSkeleton=false;

	m_bSkeletonMeshesInitied=false;

	m_pAnimController = NULL;

	m_vScale.Set(1.f,1.f,1.f);

	m_fLookUpDownAngle = 0.f;

	

}



/*bool CMD5Inst::OnlyBase(void)
{ 
	if (m_pAnimController) 
		return (m_pAnimController->OnlyBase());
	BREAKPOINT(1);
	return false;
};*/

void CMD5Inst::SetShared(CSmartPointer<CMD5_Shared> spSet,CEntity* pParentEntity)
{
	////assert(_CrtCheckMemory());
	if (!spSet)
	{
		g_pLogFile->OutPutPlainText("CMD5Inst::SetShared trying to set a NULL shared!","4","440088",true,LOG_MESSAGE_ERROR);
		return;
	}
	
	
	////assert(_CrtCheckMemory());
	m_spShared = spSet;

	////assert(_CrtCheckMemory());

	m_pCurrentBindJoints = new tMD5BindJoint[ m_spShared->GetMesh()->GetJointsNum()];

	for (int i=0;i<m_spShared->GetMesh()->GetJointsNum();i++)
	{
		int iDebug = m_spShared->GetMesh()->GetJoints()[i].iParentIndex;
		m_pCurrentBindJoints[i].iParentIndex = m_spShared->GetMesh()->GetJoints()[i].iParentIndex;
	}

	//assert(_CrtCheckMemory());

	m_pCurrentJointsPositions = new CVec3f[m_spShared->GetMesh()->GetJointsNum()];
	m_pDebugJointsFKRot       = new CVec3f[m_spShared->GetMesh()->GetJointsNum()];


	//assert(_CrtCheckMemory());
	m_pAnimController = new CAnimController(m_spShared->GetMesh(),m_spShared->GetAnims(),m_pCurrentBindJoints,this,pParentEntity);

	//assert(_CrtCheckMemory());

	GenerateRenderables(RENDERABLE_GENERATE_PPL,0);
	//GenerateRenderables(RENDERABLE_GENERATE_NONE,0);

	//assert(_CrtCheckMemory());
	

	int iBiggestVertsNum = 0;

	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		if (m_spShared->GetMesh()->GetSubMeshes()[i].iVertsNum > iBiggestVertsNum)
			iBiggestVertsNum = m_spShared->GetMesh()->GetSubMeshes()[i].iVertsNum;

	//assert(iBiggestVertsNum);

	m_pTempTangentsArray = new CVec3f[iBiggestVertsNum];
	m_pTempBinormalsArray = new CVec3f[iBiggestVertsNum];
	m_pTempNormalsArray = new CVec3f[iBiggestVertsNum];


	//assert(_CrtCheckMemory());
}

void CMD5Inst::Die()
{
	if (m_pAnimController)
		m_pAnimController->InitWeightedAnims();
}

void CMD5Inst::SetSkeletonRender(bool bSet)
{

	//TODO: check if shared was already set and valid !!


	if (!bSet)
	{
		m_bRenderSkeleton=false;
		return;
	}

	m_bRenderSkeleton=true;


	if (m_bSkeletonMeshesInitied)
		return;

	m_ppJointModel = new CStaticModel* [m_spShared->GetMesh()->GetJointsNum()];
	for (int i=0;i<m_spShared->GetMesh()->GetJointsNum();i++)
	{
		m_ppJointModel[i] = new CStaticModel();
		m_ppJointModel[i]->SetShared(st_StaticModelManager->Load("Models/earth.3ds",0.01f,0.01f,0.01f),RENDERABLE_GENERATE_PPL);
		m_ppJointModel[i]->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/joint.utm"));
	}

	m_pSELECTEDJointModel = new CStaticModel();
	m_pSELECTEDJointModel->SetShared(st_StaticModelManager->Load("Models/earth.3ds",0.01f,0.01f,0.01f),RENDERABLE_GENERATE_PPL);

	/*m_pJointModel->SetShared(st_StaticModelManager->Load("Models/earth.3ds",0.01f),RENDERABLE_GENERATE_NONE);
	m_pSELECTEDJointModel->SetShared(st_StaticModelManager->Load("Models/earth.3ds",0.01f),RENDERABLE_GENERATE_NONE);*/
		

	/*CMaterial* pMat = new CMaterial();
	pMat->Load("Materials/green.utm");	
	m_pJointModel->SetMaterialForAll(pMat);*/

	


	/*pMat = new CMaterial();
	pM*at->Load("Materials/red.utm");	

	m_pSELECTEDJointModel->SetMaterialForAll(pMat);*/

	m_pSELECTEDJointModel->SetMaterialForAll(g_pRendererBack->LoadMaterial("Materials/red.utm"));

	m_bSkeletonMeshesInitied=true;
	
}

CMD5Inst::~CMD5Inst()
{
	g_pLogFile->OutPutPlainText("deleting md5 instance...",LOG_MESSAGE_INFORMATIVE);

	if (!(m_spShared==NULL) && m_ppJointModel)
	{
		for (int i=0;i<m_spShared->GetMesh()->GetJointsNum();i++)
			SafeDelete(m_ppJointModel[i]);
		SafeDeleteArray(m_ppJointModel);
	}

	SafeDeleteArray(m_pCurrentBindJoints);
	SafeDeleteArray(m_pCurrentJointsPositions);
	SafeDeleteArray(m_pDebugJointsFKRot);

	SafeDeleteArray(m_AnimatedRends);

	SafeDelete(m_pAnimController);

	//SafeDelete(m_pJointModel);
	

	SafeDeleteArray(m_pTempTangentsArray)
	SafeDeleteArray(m_pTempBinormalsArray);
	SafeDeleteArray(m_pTempNormalsArray);

	g_pLogFile->OutPutPlainText("deleting md5 instance DONE.",LOG_MESSAGE_INFORMATIVE);

}





void CMD5Inst::RequestMoveState(int iMoveState)
{
	m_pAnimController->RequestMoveState(iMoveState);
}

void CMD5Inst::PlayAction(int iAnimNum)
{
	if (iAnimNum>=m_spShared->GetAnimsNum())
	{
		g_pLogFile->OutPutPlainText("CMD5Inst::PlayAni trying to play animation>= m_spShared->GetAnimsNum()!","4","440088",true,LOG_MESSAGE_ERROR);
		return;
	}
	m_pAnimController->PlayAction(iAnimNum);
}

int CMD5Inst::GetAnimationsNum(void)
{
	return m_spShared->GetAnimsNum();
}

float CMD5Inst::GetAnimationFPS(int iAnimationNum)
{
	if (iAnimationNum>=m_spShared->GetAnimsNum())
	{
		g_pLogFile->OutPutPlainText("CMD5Inst::GetAnimationFPS animation doesn't exist!",LOG_MESSAGE_ERROR);
		return -1;
	}


	return m_spShared->GetAnims()[iAnimationNum]->GetAnimationFPS();

}

int CMD5Inst::GetAnimationFramesNum(int iAnimationNum)
{
	if (iAnimationNum>=m_spShared->GetAnimsNum())
	{
		g_pLogFile->OutPutPlainText("CMD5Inst::iGetAnimationFramesNum animation doesn't exist!",LOG_MESSAGE_ERROR);
		return -1;
	}


	return m_spShared->GetAnims()[iAnimationNum]->GetFramesNum();


}



void CMD5Inst::Render(int iTechNum)
{
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		//g_pRendererBack->Render(&m_AnimatedRends[i],iTechNum);
		g_pRendererFront->Push(&m_AnimatedRends[i]);


	if (m_bRenderSkeleton)
		RenderJoints();
}


void CMD5Inst::SetMaterialForAll(IMaterial_Export* pMat)
{
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		m_AnimatedRends[i].SetMaterial(pMat);

}

void CMD5Inst::SetTransformForAll(float* pfTransf)
{
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		m_AnimatedRends[i].SetTransform(pfTransf);

}

void CMD5Inst::ClearLightsLinkage(void)
{
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		m_AnimatedRends[i].ClearEffectingLights();

}

void CMD5Inst::AddLightToAllRends(ILight_Export* pLight)
{
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		m_AnimatedRends[i].AddEffectingLight(pLight);

}

void  CMD5Inst::UpdateAni(DWORD dwGenerate, float fDeltaTime,bool bVisible)
{
	

	static CProfileStep UpdatingJoints("Updating Joints");	
	UpdatingJoints.Begin();
	

	static CProfileStep UpdatingRenderables("Updating Renderables");	

	if (m_pAnimController)
	{
		//m_pAnimController->SetAnimInterpolatedFrame(m_pCurrentBindJoints,iAnim,iFrameFrom, iFrameTo,fT);
		m_pAnimController->ComputeFinalSkeleton(fDeltaTime);


		UpdatingJoints.End();


		UpdatingRenderables.Begin();

		if (bVisible)
			UpdateRenderables(dwGenerate,m_vScale);
		UpdatingRenderables.End();

		
		


		return;

	}
	else
	{
		BREAKPOINT(1);
		
		return;
	}

	
}


void CMD5Inst::UpdateRenderables(DWORD dwGenerate,const CVec3f &vScale)
{

	static CProfileStep InsideUpdateRenderables("Inside Generate Renderables");	
	InsideUpdateRenderables.Begin();

	int iWeightIndexStart;
	int iWeightCount;
	const tMD5Weight* pWeight;
	int iJointIndex;
	CVec3f vTempOffset;
	CMatrix rotMat;


	static CProfileStep CreatingVertices("CreatingVertices");	
	static CProfileStep ComputingTBN("ComputingTBN");	

	static CProfileStep UploadingToVB("UploadingToVB");	
	
for (int iSubMeshNum=0;iSubMeshNum<m_spShared->GetMesh()->GetSubMeshesNum();iSubMeshNum++)
{


	IVertexBuffer_Export* pVB = m_ppAnimatedVB[iSubMeshNum];

	
	//pVB->Lock(CR_LOCK_READ_WRITE);
		

	//CVec3f* pVertices = (CVec3f*) pVB->GetStream(STREAM_POSITION);
	
	CreatingVertices.Begin();

	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum;i++)
	{				
		m_ppVerts[iSubMeshNum][i].Set(0.f,0.f,0.f);
		

		iWeightIndexStart = m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pVerts[i].iWeightsIndex;
		iWeightCount      = m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pVerts[i].iWeightsCount;

		for (int j=iWeightIndexStart;j<iWeightIndexStart+iWeightCount;j++)
		{
			pWeight = &m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pWeights[j];
			iJointIndex = pWeight->iJointIndex;		
			
			//pJointFrom = &m_ppAnimations[iAnimFrom]->GetBaseJoints()[iJointIndex];
			//pJointTo = &m_ppAnimations[iAnimTo]->GetBaseJoints()[iJointIndex];
			
			
			//pJoint->bindRot.GetMatrix(rotMat);
			m_pCurrentBindJoints[iJointIndex].bindRot.GetMatrix(rotMat);
	
			vTempOffset =rotMat * pWeight->vPosOffest;		
		
			CVec3f finalVec = ( m_pCurrentBindJoints[iJointIndex].bindPos+vTempOffset ) * pWeight->fBias;

			

			// convertion from id axis to gl/dx

			m_ppVerts[iSubMeshNum][i].v[0] += finalVec.v[0]*vScale.x;
			m_ppVerts[iSubMeshNum][i].v[1] += finalVec.v[2]*vScale.y;
			m_ppVerts[iSubMeshNum][i].v[2] += finalVec.v[1]*vScale.z;


			/*m_ppVerts[iSubMeshNum][i].v[0] += finalVec.v[0];
			m_ppVerts[iSubMeshNum][i].v[1] -= finalVec.v[1];
			m_ppVerts[iSubMeshNum][i].v[2] += finalVec.v[2];*/


		}
	}

	CreatingVertices.End();
	
	UploadingToVB.Begin();

	float fTimePerTBN;
	//char message[100];

	
	
	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{				

		

		/*double fElpasedTime;
		CPerformanceTimer timer;
		timer.Start();	*/

		UploadingToVB.End();
		ComputingTBN.Begin();
			
		pVB->CalculateTBN(m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iIndicesNum,m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].puiIndices,0,m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum,
			m_ppVerts[iSubMeshNum],m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pTexCoords,
			m_pTempNormalsArray,m_pTempTangentsArray,m_pTempBinormalsArray);	

		ComputingTBN.End();
		UploadingToVB.Begin();



		pVB->Lock(CR_LOCK_WRITE_ONLY);
		CVec3f* pTangents = (CVec3f*) pVB->GetStream(STREAM_TEXCOORD1);
		CVec3f* pBinormals = (CVec3f*) pVB->GetStream(STREAM_TEXCOORD2);
		CVec3f* pNormals = (CVec3f*) pVB->GetStream(STREAM_TEXCOORD3);

		memcpy(pTangents,m_pTempTangentsArray,sizeof(CVec3f)*m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum);
		memcpy(pBinormals,m_pTempBinormalsArray,sizeof(CVec3f)*m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum);
		memcpy(pNormals,m_pTempNormalsArray,sizeof(CVec3f)*m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum);
		
		CVec3f* pPositions = (CVec3f*) pVB->GetStream(STREAM_POSITION);
		memcpy(pPositions,m_ppVerts[iSubMeshNum],sizeof(float)*3*m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum);

		pVB->Unlock();


		UploadingToVB.End();
		/*fElpasedTime = timer.GetElapsedTime();

		fElpasedTime*=1000.f;

		sprintf(message,"TBN: %f",fElpasedTime);*/
		//g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);

	}
		
	
}

	InsideUpdateRenderables.End();

}

void CMD5Inst::RenderJoints(void)
{
	float pfWorld[16];
	CMatrix world;
	CMatrix mCurTrans;

	for (int i=0;i<m_spShared->GetMesh()->GetJointsNum();i++)
	{
		/*world.MatMakeMove(m_pCurrentJointsPositions[i].v[0],
			m_pCurrentJointsPositions[i].v[1],m_pCurrentJointsPositions[i].v[2]);*/

		world.MatMakeMove(m_pAnimController->GetCurrentBindJoints()[i].bindPos.v[0],
			m_pAnimController->GetCurrentBindJoints()[i].bindPos.v[2],
			m_pAnimController->GetCurrentBindJoints()[i].bindPos.v[1]);

		//GetCurrentBindJoints

		
		mCurTrans.Set(m_AnimatedRends[0].GetTransform());

		//world = world*mCurTrans;
		world = mCurTrans * world;

		world.ReturnArray(pfWorld);

		/*if (i==g_iDEBUGJoint)
		{		
			m_pSELECTEDJointModel->SetTransformForAll(pfWorld);	
			m_pSELECTEDJointModel->Render(0);

		}
		else*/
		{
			m_ppJointModel[i]->SetTransformForAll(pfWorld);	
			//m_pJointModel->SetTransformForAll(NULL);	

			m_ppJointModel[i]->Render(0);
			//g_pRendererFront->Push(m_ppJointModel[i]);
		}
	}


	// DEBUG DEBUG DEBUG DEBUG
	/***/
/*
	g_pGraphicsAPI->SetWorldMatrix(NULL);
	
	for (int i=0;i<m_spShared->GetMesh()->GetJointsNum();i++)
	if (m_pCurrentBindJoints[i].iParentIndex>=0)
	{
		g_pGraphicsAPI->DrawDebugLine(m_pCurrentJointsPositions[i].v[0],
			m_pCurrentJointsPositions[i].v[1],
			m_pCurrentJointsPositions[i].v[2],
			m_pCurrentJointsPositions[m_pCurrentBindJoints[i].iParentIndex].v[0],
			m_pCurrentJointsPositions[m_pCurrentBindJoints[i].iParentIndex].v[1],
			m_pCurrentJointsPositions[m_pCurrentBindJoints[i].iParentIndex].v[2]);
	}*/
	/****/
}


CVec3f CMD5Inst::GetJointPosition(int iJointIndex)
{
	BREAKPOINT(iJointIndex>=m_spShared->GetMesh()->GetJointsNum());
	BREAKPOINT(iJointIndex<0);

	float pfWorld[16];
	CMatrix world;
	CMatrix mCurTrans;

	world.MatMakeMove(m_pAnimController->GetCurrentBindJoints()[iJointIndex].bindPos.v[0],
			m_pAnimController->GetCurrentBindJoints()[iJointIndex].bindPos.v[2],
			m_pAnimController->GetCurrentBindJoints()[iJointIndex].bindPos.v[1]);

		//GetCurrentBindJoints

//	BREAKPOINT(iJointIndex!=44);
		
		mCurTrans.Set(m_AnimatedRends[0].GetTransform());

		//world = world*mCurTrans;
		world = mCurTrans * world;

		//world.Transpose();

	CVec3f test(0.f,0.f,0.f);

	test = world*test;


	return test;
}

bool CMD5Inst::GenerateRenderables(DWORD dwGenerate, int iFrame)
{

	

	m_AnimatedRends = new CRenderable[m_spShared->GetMesh()->GetSubMeshesNum()];

	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshesNum();i++)
		m_AnimatedRends[i].SetMaterial( m_spShared->GetMesh()->GetSubMeshes()[i].spMaterial);


	/////////////////////////////////////////////////////////////
	// first of all create current frame
	/////////////////////////////////////////////////////////////

	//static CVec3f** ppVerts = NULL;
	
	if (m_ppVerts==NULL)
	{
		m_ppVerts = new CVec3f*[m_spShared->GetMesh()->GetSubMeshesNum()];
		int iSubMeshesNum = m_spShared->GetMesh()->GetSubMeshesNum();
		for (int i=0;i<iSubMeshesNum;i++)
		{
			int iSubMeshVertsNum = m_spShared->GetMesh()->GetSubMeshes()[i].iVertsNum;
			m_ppVerts[i] = new CVec3f[iSubMeshVertsNum];
		}
	}



	int iJointIndex;


	
	CVec3f vTempOffset;

	int iElementCount=0;

	tMD5Joint* pJoint;	
	const tMD5Weight* pWeight;

	const tMD5AnimFrame* pFrame = &m_spShared->GetAnims()[0]->GetAnimFrames()[iFrame];


	CMatrix rotMat;

	for (int i=0;i<m_spShared->GetAnims()[0]->GetJointsNum();i++)
	{
			pJoint = &m_spShared->GetAnims()[0]->GetBaseJoints()[i];
			
			CVec3f newPos=pJoint->pos;
			CQuat  newRot=pJoint->rot;

			iElementCount=pJoint->iStartIndex;

			if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_X)
				newPos.v[0] = pFrame->pData[iElementCount++];

			if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Y)
				newPos.v[1] = pFrame->pData[iElementCount++];

			if (pJoint->dwAnimFlags & ANIMATED_NODE_TRANS_Z)
				newPos.v[2] = pFrame->pData[ iElementCount++];

			if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_X)
				newRot.x = pFrame->pData[iElementCount++];

			if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Y)
				newRot.y = pFrame->pData[ iElementCount++];

			if (pJoint->dwAnimFlags & ANIMATED_NODE_ROT_Z)
				newRot.z = pFrame->pData[ iElementCount++];

			newRot.ComputeW();


			if (!pJoint->pParent)
			{			
				m_pCurrentBindJoints[i].bindPos = newPos;
				m_pCurrentBindJoints[i].bindRot = newRot;
			}
			else
			{
				/*if (strcmp(strlwr(pJoint->pchName),"waist")==0)
				{
					CQuat add(100.f,100.f,100.f);
					newRot = add * newRot;

				}*/

				m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindRot.GetMatrix(rotMat);							
				m_pCurrentBindJoints[i].bindPos = (rotMat*newPos) + m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindPos;
				m_pCurrentBindJoints[i].bindRot = newRot * m_pCurrentBindJoints[m_pCurrentBindJoints[i].iParentIndex].bindRot;

				
					

			}



	}


for (int iSubMeshNum=0;iSubMeshNum<m_spShared->GetMesh()->GetSubMeshesNum();iSubMeshNum++)
{
	//assert(_CrtCheckMemory());
	
	for (int i=0;i<m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].iVertsNum;i++)
	{			
		//assert(_CrtCheckMemory());
		m_ppVerts[iSubMeshNum][i].Set(0.f,0.f,0.f);

		for (int j=m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pVerts[i].iWeightsIndex;
			j<m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pVerts[i].iWeightsIndex+m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pVerts[i].iWeightsCount;
			j++)
		{
			pWeight = &m_spShared->GetMesh()->GetSubMeshes()[iSubMeshNum].pWeights[j];
			iJointIndex = pWeight->iJointIndex;		
			
			//pJointFrom = m_spShared->GetAnims()[0]->GetBaseJoints()[iJointIndex];
			
			
			//pJoint->bindRot.GetMatrix(rotMat);
			m_pCurrentBindJoints[iJointIndex].bindRot.GetMatrix(rotMat);

			vTempOffset =rotMat * pWeight->vPosOffest;		
			
			//CVec3f finalVec = ( pJoint->bindPos+vTempOffset ) * pWeight->fBias;
			CVec3f finalVec = ( m_pCurrentBindJoints[iJointIndex].bindPos+vTempOffset ) * pWeight->fBias;
			

			m_ppVerts[iSubMeshNum][i].v[0] += finalVec.v[0];
			m_ppVerts[iSubMeshNum][i].v[1] += finalVec.v[2];
			m_ppVerts[iSubMeshNum][i].v[2] += finalVec.v[1];
	

		}
		//assert(_CrtCheckMemory());
	}
}

	////////////////////////////////////////////////////////////////////////
	
	//tStreamMap sMap;
	//IVertexBuffer* pVB;

	m_ppAnimatedVB = new IVertexBuffer_Export*[m_spShared->GetMesh()->GetSubMeshesNum()];



	CVec3f* pVertices;
	CVec3f* pTangents;
	CVec3f* pBinormals;
	CVec3f* pNormals;
	CVec2f*  pTexCoords;


	//assert(_CrtCheckMemory());

	
	//assert(_CrtCheckMemory());

for (int iCurrentSubMeshNum=0;iCurrentSubMeshNum<m_spShared->GetMesh()->GetSubMeshesNum();iCurrentSubMeshNum++)
{
	//assert(_CrtCheckMemory());

	m_ppAnimatedVB[iCurrentSubMeshNum] = g_pRendererBack->GenerateVertexBuffer();
	m_ppAnimatedVB[iCurrentSubMeshNum]->AddStream(STREAM_POSITION,CR_FLOAT3);
	m_ppAnimatedVB[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD0,CR_FLOAT2);	
	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		m_ppAnimatedVB[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD1,CR_FLOAT3);	
		m_ppAnimatedVB[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD2,CR_FLOAT3);	
		m_ppAnimatedVB[iCurrentSubMeshNum]->AddStream(STREAM_TEXCOORD3,CR_FLOAT3);	
	}

	//assert(_CrtCheckMemory());

	
	m_ppAnimatedVB[iCurrentSubMeshNum]->AllocateStreams(m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].iVertsNum,VB_MEM_GPU|VB_SINGLE,CR_STREAM_DYNAMIC);


	m_ppAnimatedVB[iCurrentSubMeshNum]->Lock(CR_LOCK_WRITE_ONLY);


	pVertices = (CVec3f*) m_ppAnimatedVB[iCurrentSubMeshNum]->GetStream(STREAM_POSITION);
	memcpy(pVertices,m_ppVerts[iCurrentSubMeshNum],sizeof(float)*m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].iVertsNum*3);

	pTexCoords = (CVec2f*) m_ppAnimatedVB[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD0);


	memcpy(pTexCoords,m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].pTexCoords,sizeof(float)*2*m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].iVertsNum);




	if (dwGenerate==RENDERABLE_GENERATE_PPL)
	{
		pTangents = (CVec3f*) m_ppAnimatedVB[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD1);

		pBinormals = (CVec3f*) m_ppAnimatedVB[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD2);

		pNormals = (CVec3f*) m_ppAnimatedVB[iCurrentSubMeshNum]->GetStream(STREAM_TEXCOORD3);

		m_ppAnimatedVB[iCurrentSubMeshNum]->CalculateTBN(m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].iIndicesNum,m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].puiIndices,0,m_spShared->GetMesh()->GetSubMeshes()[iCurrentSubMeshNum].iVertsNum,
			pVertices,pTexCoords,pNormals,pTangents,pBinormals);	
	}
		
	m_ppAnimatedVB[iCurrentSubMeshNum]->Unlock();

	//assert(_CrtCheckMemory());

	m_AnimatedRends[iCurrentSubMeshNum].SetVertexBuffer(m_ppAnimatedVB[iCurrentSubMeshNum],true);
	m_AnimatedRends[iCurrentSubMeshNum].SetIndexBuffer((IIndexBuffer*)m_spShared->GetIndexBuffer(iCurrentSubMeshNum),false);

	m_AnimatedRends[iCurrentSubMeshNum].SetDrawCallParameters(
		0,
		((IIndexBuffer*)m_spShared->GetIndexBuffer(iCurrentSubMeshNum))->GetLength(),
		0,0,
		m_ppAnimatedVB[iCurrentSubMeshNum]->GetLength()
		);

	//assert(_CrtCheckMemory());
}
	

	return true;
	
}


