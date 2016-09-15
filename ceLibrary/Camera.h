#ifndef YOEL_CAMERA_H
#define YOEL_CAMERA_H

#pragma once

#include "Math3d.h"
#include <Windows.h>
///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////////////////////////////////////////////////
// TODO:
// ///////////////* Add m_pmWorldMove, m_pmWorldRotate////////
// * 
// * all mouse input handling TRANSLATION into rotation 
//   maybe should be done here
///////////////////////////////////////////////////////////////////////////


#define CAMERA_ACTION_ROT_X_UP         (1<<1)
#define CAMERA_ACTION_ROT_X_DOWN       (1<<2)
#define CAMERA_ACTION_ROT_Y_LEFT       (1<<3)
#define CAMERA_ACTION_ROT_Y_RIGHT      (1<<4)
#define CAMERA_ACTION_ROT_Z_LEFT       (1<<5)
#define CAMERA_ACTION_ROT_Z_RIGHT      (1<<6)

#define CAMERA_ACTION_MOVE_X_LEFT      (1<<7)
#define CAMERA_ACTION_MOVE_X_RIGHT     (1<<8)
#define CAMERA_ACTION_MOVE_Y_UP        (1<<9)
#define CAMERA_ACTION_MOVE_Y_DOWN      (1<<10)
#define CAMERA_ACTION_MOVE_Z_FORWARD   (1<<11)
#define CAMERA_ACTION_MOVE_Z_BACKWARDS (1<<12)



class CCamera
{
public:

	CCamera();
	~CCamera();



	 void SetPosition(float fX,float fY,float fZ);
	 void SetPosition(const CVec3f& pVec);
	 void SetAngles(float fXAng,float fYAng,float fZAng);

	 void AddAngles(float fX,float fY,float fZ);

	 void AddMoveRotateFlags(DWORD dwFlags);
	 
	 CMatrix* ReturnCameraMatrix(void);

	 const float*   ReturnCameraMatrixfa(void);

	 const float*    ReturnMIRROREDCameraMatrixfa(void);
	 

	 // TODO:
	 // ModelToWorldARRAY(CVec3f* pVectors,int iArraySize;
	 // ModelToWorldVECTOR(CVec3f* pVec);
	 // etc.

  
	 void Init(void);
	 
	 const CVec3f* GetPosition(void);
	 float GetXAng(void) { return xang;};
	 float GetYAng(void) { return yang;};
	 float GetZAng(void) { return zang;};
	 
	 void InitMoveRotFlags(void);
	    
	 float AngleBetweenVectorAndCamera(CVec3f v);
	 
	 bool IsAngleBetweenVectorAndCameraBiggerThan90(CVec3f v1,CVec3f v2);
	 
	 bool CanISeeCreature(CVec3f v);
	 
	 void RotateAround(CVec3f point,float ZDistance,float Xang,float Yang,float Zang);
	 
	 CVec3f ReturnLookDirectionNormalized(void);
	 CVec3f ReturnLeftDirectionNormalized(void);
	 CVec3f ReturnUpDirectionNormalized(void);
	 
	 CVec3f RotateWithCameraRotationMatrix(CVec3f RotateMe);

	 //////////////////////
	 // GetLookAtAngles()
	 // (FROM (0,0,1)!)
	 //////////////////////
	 void GetLookAtAngles(CVec3f& from,CVec3f& to,float& xAng, float& yAng);

	 CVec3f GetLookAt(void);
	 
	 void LookAt(CVec3f p1);
	 
	 //void Write(CVec3f p1);
	 
	 void UpdatePositionAndMatrices(float fDeltaTime);

	 //CVec3f GetDesiredPosition(void);

	 // all data should be private...

	 private:
	 
	 CVec3f m_LookDir;
	 ///CVec3f CameraSpeed;

	 float m_fCameraSpeed;

	 
	 float RotateYSpeed,RotateXSpeed,RotateZSpeed;

	 // world to camera matrix

	 CMatrix m_mCameraMove;
	 CMatrix m_mCameraRotate;
	 CMatrix m_mCamera; 

	 float* m_pfCameraMatrix;
	 float* m_pfMIRROREDCameraMatrix;

	 

	 // model to world matrix

	 /*CMatrix* m_pmWorldMove;
	 CMatrix* m_pmWorldRotate;
	 CMatrix* m_pmWorld; // m_pmWorldRotate*m_pmWorldMove*/

	 //TODO: CVec3f m_CameraLookDirection
	 
	 
	 // DEBUG or NEED TO CHANGE
	 
	 float AngleBetweenVectorsTemp;
	 float RotateAroundXAng,RotateAroundYAng,RotateAroundZAng;
	 //float TimePassed;


	 DWORD m_dwMoveRotFlags;
	 
	 // Booleans

	 bool Fly;

	CVec3f m_Pos;

	CVec3f m_PosTemp;

	float xang,yang,zang;
  
};


#endif // YOEL_CAMERA_H