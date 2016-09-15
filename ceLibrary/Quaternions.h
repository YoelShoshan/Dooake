#ifndef CRIMSON_QUATERNIONS_H
#define CRIMSON_QUATERNIONS_H

#pragma once

#include "Math3d.h"

#define QUAT_SLERP_DELTA 0.0001f

class CQuat
{
public:
	CQuat();
	CQuat(const float* fpMat);
	CQuat(float roll, float pitch, float yaw);
	CQuat(float setW, float setX, float setY, float setZ);
	CQuat(const CQuat& quat);
	~CQuat();

	void operator = (const CQuat& quat);

	void Set(const float* fpMat);
	void Set(float roll, float pitch, float yaw);
	void Set(float setW, float setX, float setY, float setZ);

	CQuat operator * (CQuat& quat) const;

	CQuat operator * (float fT) const;

	void  operator += (CQuat& quat);

	// assumes allocated!
	void ReturnMatrix(float* pfMatrix) const;

	void GetMatrix(CMatrix& mat) const;

	CQuat Inverse(void) const;


	// this functions returns a quaternion that is the slerp interpolation between
	// self and other
	// fT is in the range [0,1]
	CQuat Slerp(const CQuat& other,float fT) const;

	void ComputeW(void);


	// should i have normalize function ??



	float w,x,y,z;
};




#endif