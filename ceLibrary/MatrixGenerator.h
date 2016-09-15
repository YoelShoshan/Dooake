#ifndef CRIMSON_MATRIX_GENERATOR_H
#define CRIMSON_MATRIX_GENERATOR_H

#pragma once

class CMatrixGenerator
{
public:
	CMatrixGenerator() {};
	~CMatrixGenerator() {};

	// all functions return column based layout matrices!
	static void LookAtRH(float*& pFillMe,const float* pfPos, const float* pfLookAt, const float* pfUp);
	static void LookAtLH(float*& pFillMe,const float* pfPos, const float* pfLookAt, const float* pfUp);

	static void PespectiveRH(float*& pFillMe,float fFovy,float fAspect, float fZNear, float fZFar);
	static void PespectiveLH(float*& pFillMe,float fFovy,float fAspect, float fZNear, float fZFar);
};



#endif