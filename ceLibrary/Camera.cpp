#include "stdafx.h"


#include "Camera.h"
#include "Math3d.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Singleton.h"

//#include "Timer.h"


#include "GMath.h"

#include "MatrixGenerator.h"


#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Quaternions.h"

CCamera::CCamera()
{
	Init();
}

CCamera::~CCamera()
{

}

void CCamera::Init(void)
 {
	 m_LookDir.Set(0.f,0.f,1.f);
	 m_pfCameraMatrix = new float[16];
	 m_pfMIRROREDCameraMatrix = new float[16];
    Fly=true;
	 //Fly=false;
	RotateYSpeed=1.f;
	RotateXSpeed=0.5f;
	RotateZSpeed=1.f;
	m_Pos.v[0]=m_Pos.v[1]=m_Pos.v[2]=xang=yang=zang=0.f;
	RotateAroundXAng=RotateAroundYAng=RotateAroundZAng=0.f;
	
	m_fCameraSpeed = 0.3f;
	
	m_mCameraMove.MatMakeUnit();
	m_mCameraRotate.MatMakeUnit();

	m_dwMoveRotFlags = 0;
 }

void CCamera::AddMoveRotateFlags(DWORD dwFlags)
{
	m_dwMoveRotFlags|=dwFlags;
}
 
void CCamera::AddAngles(float fX,float fY,float fZ)
{
	xang+=fX;
	yang+=fY;
	zang+=fZ;

}
 
 const CVec3f* CCamera::GetPosition(void) 
 {
	 return &m_Pos;
 }
 void CCamera::InitMoveRotFlags(void)
 {
	 m_dwMoveRotFlags = 0;
 }
 
 CMatrix* CCamera::ReturnCameraMatrix(void)
 {
	 CMatrix* returnM = new CMatrix;

	 (*returnM) = m_mCameraMove*m_mCameraRotate;

	 return returnM;
 }
 
 void CCamera::SetPosition(float fX,float fY,float fZ)
 {
	 m_Pos.Set(fX,fY,fZ);
 }

 void CCamera::SetPosition(const CVec3f& pVec)
 {
	 m_Pos.Set(pVec.v[0],pVec.v[1],pVec.v[2]);
 }

 void CCamera::SetAngles(float fXAng,float fYAng,float fZAng)
 {
	 xang = fXAng;
	 yang = fYAng;
	 zang = fZAng;
 }
 
 
 float CCamera::AngleBetweenVectorAndCamera(CVec3f v)
 {
	 // Changes during cleaning:
     // changed the all m_mCameraRotate into t_mRotate;
	 CVec3f CameraDirectionVector;
	 CMatrix t_mRotate;
	 CameraDirectionVector.v[0]=CameraDirectionVector.v[1]=CameraDirectionVector.v[2]=0.f;
	 t_mRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0));
	 CameraDirectionVector.v[2]=m_fCameraSpeed;
	 CameraDirectionVector=t_mRotate*CameraDirectionVector;
	
	 return AngleBetweenVectorsANG(CameraDirectionVector,v);
 }

 // it seems like it should be IsAngleBetweenVECTORSBiggerThan90
 bool CCamera::IsAngleBetweenVectorAndCameraBiggerThan90(CVec3f v1,CVec3f v2)
 {
	 float angle;
	 angle = AngleBetweenVectorsANG(v1,v2);

	 if ((angle<-90) || angle>90)
		 return false;
	 else
		 return true;
 }

 bool CCamera::CanISeeCreature(CVec3f v)
 {
	 // Changes during cleaning:
	 // changed the all m_mCameraRotate into t_mRotate;

	CVec3f CameraDirectionVector;
	CMatrix t_mRotate;


	CameraDirectionVector.v[0]=CameraDirectionVector.v[1]=CameraDirectionVector.v[2]=0.f;
	t_mRotate.MatMakeRot(DEG2RAD(xang),DEG2RAD(yang),DEG2RAD(zang));
	CameraDirectionVector.v[2]=m_fCameraSpeed;
	CameraDirectionVector=t_mRotate*CameraDirectionVector;

	v.v[1]=0;

	AngleBetweenVectorsTemp=AngleBetweenVectorsANG(CameraDirectionVector,v);
	return IsAngleBetweenVectorAndCameraBiggerThan90(CameraDirectionVector,v);

 }

 void CCamera::RotateAround(CVec3f point,float ZDistance,float Xang,float Yang,float Zang)
 {
	CVec3f v;
	CMatrix Move,Rotate;
	Move.MatMakeMove(0.f,m_Pos.v[1]-point.v[1],-ZDistance);
	Rotate.MatMakeRot(DEG2RAD(Xang),DEG2RAD(Yang),DEG2RAD(Zang));
	//Rotate.MatMakeRot(Xang,Yang,Zang);
	v.v[0]=v.v[1]=v.v[2]=0.f;
	v = (Rotate * Move) * v;
	//v = (Move) * v;
	v.v[0]+=point.v[0];
	v.v[1]+=point.v[1];
	v.v[2]+=point.v[2];
	m_Pos.v[0]=v.v[0];
	m_Pos.v[1]=v.v[1];
	m_Pos.v[2]=v.v[2];
	/*x=0.f;
	y=0.f;
	z=0.f;*/
 }

 CVec3f CCamera::ReturnLookDirectionNormalized(void)
   {
	CMatrix t_RotateMatrix;
	CVec3f LookDirection;
	LookDirection.v[0]=LookDirection.v[1]=0.f;
	LookDirection.v[2]=-1.f;

	
	t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	LookDirection=t_RotateMatrix*LookDirection;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	LookDirection=t_RotateMatrix*LookDirection;
	
	return LookDirection;
   }

   CVec3f CCamera::ReturnLeftDirectionNormalized(void)
   {
	 CMatrix t_RotateMatrix;
	 CVec3f LeftDirection;
	 LeftDirection.v[1]=LeftDirection.v[2]=0.f;
	 LeftDirection.v[0]=-1.f;
	
	 t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	 LeftDirection=t_RotateMatrix*LeftDirection;
	
	 t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	 LeftDirection=t_RotateMatrix*LeftDirection;	
	 return LeftDirection;
   }
 
   CVec3f CCamera::ReturnUpDirectionNormalized(void)
   {
	 CMatrix t_RotateMatrix;
	 CVec3f UpDirection;
	 UpDirection.v[1]=1.f;
	 UpDirection.v[2]=UpDirection.v[0]=0.f;
	
	 t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	 UpDirection=t_RotateMatrix*UpDirection;
	
	 t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	 UpDirection=t_RotateMatrix*UpDirection;	
	 return UpDirection;

   }

 CVec3f CCamera::RotateWithCameraRotationMatrix(CVec3f RotateMe)
 {
    CMatrix t_RotateMatrix;

	t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	RotateMe=t_RotateMatrix*RotateMe;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	RotateMe=t_RotateMatrix*RotateMe;
	return RotateMe;
 }

 //////////////////////
 // GetLookAtAngles()
 // (FROM (0,0,1)!)
 //////////////////////
 void CCamera::GetLookAtAngles(CVec3f& from,CVec3f& to,float& xAng, float& yAng)
 {
	 CVec3f vec1,vec2,vec3;

	 //CVec3f pos(0.f,0.f,0.f);

	 vec1.v[0]=to.v[0]-m_Pos.v[0];;
	 vec1.v[1]=to.v[1]-m_Pos.v[1];;
	 vec1.v[2]=to.v[2]-m_Pos.v[2];;

	 vec2.v[0]=to.v[0]-m_Pos.v[0];;
	 vec2.v[1]=0.f;
	 vec2.v[2]=to.v[2]-m_Pos.v[2];;

	 vec3.v[0]=0.f;
	 vec3.v[1]=0.f;
	 vec3.v[2]=1.f;

	 /*xang=-RAD2DEG(acos(Dot(v1,v2) / (LENGTH(v1) * LENGTH(v2))));
	 yang=RAD2DEG(acos(Dot(v2,v3) / (LENGTH(v2) * LENGTH(v3))));*/

	 xAng = -(AngleBetweenVectorsANG(vec1,vec2));
	 yAng = (AngleBetweenVectorsANG(vec2,vec3));


	 if (to.v[0]<from.v[0])
		 yAng=180.f-yAng;
	 else
		 yAng=-(180.f - yAng);

	 if (to.v[1]>from.v[1])
		 xAng=-xAng;

	 //zAng=0.f;
 }
 
 
 void CCamera::LookAt(CVec3f p1)
 {
	GetLookAtAngles(m_Pos,p1,xang,yang);
	zang=0.f;
 }
 /*void CCamera::Write(CVec3f p1)
 {
    FILE *fp;
	
	if ((fp=fopen("Camera.txt","w" ))==NULL)					
	{	printf("Error opening file: Can't find file\n");
	}

	CVec3f v1,v2,v3;
	CVec3f temp;

	v1.v[0]=p1.v[0]-m_Pos.v[0];
	v1.v[1]=p1.v[1]-m_Pos.v[1];
	v1.v[2]=p1.v[2]-m_Pos.v[2];

	v2.v[0]=p1.v[0]-m_Pos.v[0];
	v2.v[1]=0.f;
	v2.v[2]=p1.v[2]-m_Pos.v[2];

	v3.v[0]=0.f;
	v3.v[1]=0.f;
	v3.v[2]=0.f;


	temp.v[0]=-RAD2DEG(acosf(Dot(v1,v2) / v1.Length() * v2.Length() ));
	temp.v[1]=RAD2DEG(acosf(Dot(v2,v3) / v2.Length() * v3.Length() ));
	temp.v[2]=0.f;
	
	{
		fprintf(fp,"XAng:%f,YAng:%f,ZAng:%f.\n",temp.v[0],temp.v[1],temp.v[2]);
	}

 }*/

 /*CVec3f CCamera::GetDesiredPosition(void)
 {
	 CVec3f vDesiredPosition = m_Pos;

	     //CVec3f TempCameraSpeed;
	 float fTempCameraSpeed;


	m_mCameraRotate.MatMakeUnit();
	m_mCameraMove.MatMakeUnit();


	//if (RotateYLeft)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_LEFT)
	{
		yang+=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	//if (RotateYRight)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_RIGHT)
	{
		yang-=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}

	if (yang<0.f) yang +=360.f;
	if (yang>360.f) yang -=360.f;


	//if (RotateXDown)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_DOWN)
	{
		if (xang>-89.f) xang-=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateXSpeed /15.f;		
	}

	//if (RotateXUp)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_UP)
	{
		if (xang<89.f) xang+=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateYSpeed /15.f;
	}



	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_LEFT)
	{
		zang+=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_RIGHT)
	{
		zang-=CLib::m_pTimer->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}
	
	
	TempPoint.v[0]=TempPoint.v[1]=TempPoint.v[2]=0.f;

	
	fTempCameraSpeed = fCameraSpeed;
	


	fCameraSpeed*=CLib::m_pTimer->GetDeltaTimeFAKE();

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_FORWARD)
	{
		TempPoint.v[2]-=fCameraSpeed;
	}

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_BACKWARDS)
	{
		TempPoint.v[2]+=fCameraSpeed;
	}

	//if (y>0.3f) 
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_DOWN)
	{
		TempPoint.v[1]-=fCameraSpeed;
	}
	//if (y<3.9f)
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_UP)
	{
		TempPoint.v[1]+=fCameraSpeed;
	}


	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_LEFT)
	{
		TempPoint.v[0]-=fCameraSpeed;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_RIGHT)
	{
		TempPoint.v[0]+=fCameraSpeed;
	}
	

	// make m_mCameraMove matrix contain all translation needed
	m_mCameraMove.MatMakeMove(TempPoint.v[0],TempPoint.v[1],TempPoint.v[2]);

	
	// make m_mCameraRotate matrix will contain all rotation needed
	if (Fly)
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(xang),DEG2RAD(yang),DEG2RAD(0));
	}
	else // a fake "walk"
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0));
	}

	
	// TODO:
	// an optimization would be to create a method inside CMatrix that does:
	// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
	//                               small (mX*mY*mZ)
	// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,xAng,yAng,zAng);

	m_mCamera = m_mCameraRotate*m_mCameraMove; // m_mCamera has entire WORLD -> CAMERA matrix now

	TempPoint=m_mCamera*TempPoint;


	// actually change the position of the camera

	///m_Pos+=TempPoint;
	vDesiredPosition+=TempPoint;


	// Basic Collision Detection and Restrictions

	if (xang < -50.f)
		xang = -50.f;
	if (xang > 50.f)
		xang = 50.f;

	fCameraSpeed = fTempCameraSpeed;

	return vDesiredPosition;
 }
*/
void CCamera::UpdatePositionAndMatrices(float fDeltaTime)
 {

 float fXAngFittingCS,fYAngFittingCS,fZAngFittingCS; // angles fitting the CoordinateSystem

 fXAngFittingCS=xang;
 fYAngFittingCS=yang;
 fZAngFittingCS=zang;

   float fTempCameraSpeed;

   CMatrix move,rotate,camera;


	move.MatMakeUnit();
	rotate.MatMakeUnit();
	camera.MatMakeUnit();


	//if (RotateYLeft)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_LEFT)
	{
		fYAngFittingCS+=fDeltaTime * RotateYSpeed / 15.f;
	}

	//if (RotateYRight)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_RIGHT)
	{
		fYAngFittingCS-=fDeltaTime * RotateYSpeed  / 15.f;
	}

	if (fYAngFittingCS<0.f) fYAngFittingCS +=360.f;
	if (fYAngFittingCS>360.f) fYAngFittingCS -=360.f;


	//if (RotateXDown)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_DOWN)
	{
		if (fXAngFittingCS>-89.f) fXAngFittingCS-=fDeltaTime * RotateXSpeed /15.f;		
	}

	//if (RotateXUp)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_UP)
	{
		if (fXAngFittingCS<89.f) fXAngFittingCS+=fDeltaTime * RotateYSpeed /15.f;
	}



	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_LEFT)
	{
		zang+=fDeltaTime * RotateYSpeed / 15.f;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_RIGHT)
	{
		zang-=fDeltaTime * RotateYSpeed  / 15.f;
	}
	
	CVec3f MoveVec(0.f,0.f,0.f);
	
	fTempCameraSpeed = m_fCameraSpeed;

	m_fCameraSpeed*=fDeltaTime;
	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_FORWARD)
	{
		MoveVec.v[2]-=m_fCameraSpeed;
	}
	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_BACKWARDS)
	{
		MoveVec.v[2]+=m_fCameraSpeed;
	}

	//if (y>0.3f) 
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_DOWN)
	{
		MoveVec.v[1]-=m_fCameraSpeed;
	}
	//if (y<3.9f)
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_UP)
	{
		MoveVec.v[1]+=m_fCameraSpeed;
	}


	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_LEFT)
	{
		MoveVec.v[0]-=m_fCameraSpeed;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_RIGHT)
	{
		MoveVec.v[0]+=m_fCameraSpeed;
	}
	
	// make move matrix contain all translation needed

	move.MatMakeMove(MoveVec.v[0],MoveVec.v[1],MoveVec.v[2]);

	CMatrix rotX,rotY,rotZ;
	
	rotX.MatMakeXRot(DEG2RAD(fXAngFittingCS));
	rotY.MatMakeYRot(DEG2RAD(fYAngFittingCS));
	rotZ.MatMakeZRot(DEG2RAD(fZAngFittingCS));

	camera = rotate*move; 

	MoveVec=(rotY*rotX*rotZ)*MoveVec;

	// actually change the position of the camera

	m_Pos+=MoveVec;

	// Basic Collision Detection and Restrictions
/*
    if (g_pGraphicsAPI->GetHandness()==API_LEFT_HANDED)
	{
		if (fXAngFittingCS < 180.f+85.f)
			fXAngFittingCS = 180.f+85.f;
	}
	else //opengl
	{
		if (fXAngFittingCS < -85.f)
			fXAngFittingCS = -85.f;
	}*/
	

	m_fCameraSpeed = fTempCameraSpeed;



	float pfPos[3],pfLookAt[3],pfUp[3];
	
	CVec3f forward;

	forward.Set(0.f,0.f,-1.f);
	CVec3f up(0.f,1.f,0.f);

	forward=(rotY*rotX*rotZ)*forward;
	up=(rotY*rotX*rotZ)*up;

	forward.Normalize();

	pfUp[0] = up.v[0];
	pfUp[1] = up.v[1];
	pfUp[2] = up.v[2];

	pfPos[0] = m_Pos.v[0];
	pfPos[1] = m_Pos.v[1];
	pfPos[2] = m_Pos.v[2];

	pfLookAt[0] = m_Pos.v[0]+forward.v[0];
	pfLookAt[1] = m_Pos.v[1]+forward.v[1];
	pfLookAt[2] = m_Pos.v[2]+forward.v[2];

	
	CMatrixGenerator::LookAtRH(m_pfCameraMatrix,pfPos,pfLookAt,pfUp);	


	// creating mirror matrix
	CMatrix m1,m2;
	m1.SetTransposed(m_pfCameraMatrix);
	m2.MatMakeScale(1.f,1.f,-1.f);
	m2.Transpose();

	(m2*m1).ReturnTransposedArray(m_pfMIRROREDCameraMatrix);



	

 }

 CVec3f CCamera::GetLookAt(void)
 {

	 CMatrix rotX,rotY,rotZ;
	


	rotX.MatMakeXRot(DEG2RAD(xang));
	rotY.MatMakeYRot(DEG2RAD(yang));
	rotZ.MatMakeZRot(DEG2RAD(zang));


	 float pfPos[3],pfLookAt[3],pfUp[3];
	
	CVec3f forward;

	forward.Set(0.f,0.f,-1.f);
	CVec3f up(0.f,1.f,0.f);

	forward=(rotY*rotX*rotZ)*forward;
	up=(rotY*rotX*rotZ)*up;

	forward.Normalize();

	pfUp[0] = up.v[0];
	pfUp[1] = up.v[1];
	pfUp[2] = up.v[2];

	pfPos[0] = m_Pos.v[0];
	pfPos[1] = m_Pos.v[1];
	pfPos[2] = m_Pos.v[2];

	pfLookAt[0] = /*m_Pos.v[0]+*/forward.v[0];
	pfLookAt[1] = /*m_Pos.v[1]+*/forward.v[1];
	pfLookAt[2] = /*m_Pos.v[2]+*/forward.v[2];

	return CVec3f(pfLookAt[0],pfLookAt[1],pfLookAt[2]);


 }

 const float*  CCamera::ReturnCameraMatrixfa(void)
 {
	 return m_pfCameraMatrix;
 }

 const float*  CCamera::ReturnMIRROREDCameraMatrixfa(void)
 {
	 return m_pfMIRROREDCameraMatrix;
 }
