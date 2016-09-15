#include "stdafx.h"
#include "MatrixGenerator.h"
#include "GMath.h"

#include "Math3D.h"

// all functions return column based layout matrices!
void CMatrixGenerator::LookAtRH(float*& pFillMe, const float* pfPos, const float* pfLookAt, const float* pfUp) 
{
	CVec3f vLookAt(pfLookAt[0],pfLookAt[1],pfLookAt[2]);
	CVec3f vPos(pfPos[0],pfPos[1],pfPos[2]);
	CVec3f vUp(pfUp[0],pfUp[1],pfUp[2]);

	CVec3f zaxis=(vPos-vLookAt).Normalize();
	CVec3f xaxis=(Cross(vUp,zaxis)).Normalize();
	CVec3f yaxis=(Cross(zaxis,xaxis)).Normalize();


	pFillMe[0] = xaxis.v[0];	      pFillMe[1] = yaxis.v[0];	        pFillMe[2] = zaxis.v[0];	      pFillMe[3] = 0.f;
	pFillMe[4] = xaxis.v[1];	      pFillMe[5] = yaxis.v[1];	        pFillMe[6] = zaxis.v[1];	      pFillMe[7] = 0.f;
	pFillMe[8] = xaxis.v[2];	      pFillMe[9] = yaxis.v[2];	        pFillMe[10] = zaxis.v[2];	      pFillMe[11] = 0.f;
	pFillMe[12] = (vPos*-1.f)*xaxis;  pFillMe[13] = (vPos*-1.f)*yaxis;	pFillMe[14] = (vPos*-1.f)*zaxis;  pFillMe[15] = 1.f;	

/*	pFillMe[0] = xaxis.v[0];	      pFillMe[4] = yaxis.v[0];	        pFillMe[8] = zaxis.v[0];	      pFillMe[12] = 0.f;
	pFillMe[1] = xaxis.v[1];	      pFillMe[5] = yaxis.v[1];	        pFillMe[9] = zaxis.v[1];	      pFillMe[13] = 0.f;
	pFillMe[2] = xaxis.v[2];	      pFillMe[6] = yaxis.v[2];	        pFillMe[10] = zaxis.v[2];	      pFillMe[14] = 0.f;
	pFillMe[3] = (vPos*-1.f)*xaxis;  pFillMe[7] = (vPos*-1.f)*yaxis;	pFillMe[11] = (vPos*-1.f)*zaxis;  pFillMe[15] = 1.f;	*/

	
}
void CMatrixGenerator::LookAtLH(float*& pFillMe, const float* pfPos, const float* pfLookAt, const float* pfUp)
{
	CVec3f vLookAt(pfLookAt[0],pfLookAt[1],pfLookAt[2]);
	CVec3f vPos(pfPos[0],pfPos[1],pfPos[2]);
	CVec3f vUp(pfUp[0],pfUp[1],pfUp[2]);
	

	CVec3f zaxis =(vLookAt - vPos).Normalize();
	CVec3f xaxis = (Cross(vUp, zaxis).Normalize());
	CVec3f yaxis = (Cross(zaxis, xaxis).Normalize());

	
	pFillMe[0] = xaxis.v[0];	pFillMe[1] = yaxis.v[0];	pFillMe[2] = zaxis.v[0];	pFillMe[3] = 0.f;
	pFillMe[4] = xaxis.v[1];	pFillMe[5] = yaxis.v[1];	pFillMe[6] = zaxis.v[1];	pFillMe[7] = 0.f;
	pFillMe[8] = xaxis.v[2];	pFillMe[9] = yaxis.v[2];	pFillMe[10] = zaxis.v[2];	pFillMe[11] = 0.f;
	pFillMe[12] = (vPos*-1.f)*xaxis;	pFillMe[13] = (vPos*-1.f)*yaxis;	pFillMe[14] = (vPos*-1.f)*zaxis;	pFillMe[15] = 1.f;	

}

// gl
void CMatrixGenerator::PespectiveRH(float*& pFillMe,float fFovy,float fAspect, float fZNear, float fZFar) 
{
	float alpha=DEG2RAD(fFovy/2.0);
	//float yScale=cosf(alpha)/sinf(alpha);
	float yScale=1.0/tanf(alpha);
	float xScale=yScale/fAspect;

	float nf=1.0f/(fZNear-fZFar);

	
	pFillMe[0] = xScale;  pFillMe[1] = 0.f;	    pFillMe[2] = 0.f;	            pFillMe[3] = 0.f;
	pFillMe[4] = 0.f;	  pFillMe[5] = yScale;	pFillMe[6] = 0.f;	            pFillMe[7] = 0.f;
	pFillMe[8] = 0.f;     pFillMe[9] = 0.f;	    pFillMe[10] = fZFar*nf;     	pFillMe[11] = -1.f;
	pFillMe[12] = 0.f;	  pFillMe[13] = 0.f;	pFillMe[14] = fZNear*fZFar*nf;	pFillMe[15] = 0.f;	

	/*pFillMe[0] = xScale;  pFillMe[4] = 0.f;	    pFillMe[8] = 0.f;	            pFillMe[12] = 0.f;
	pFillMe[1] = 0.f;	  pFillMe[5] = yScale;	pFillMe[9] = 0.f;	            pFillMe[13] = 0.f;
	pFillMe[2] = 0.f;     pFillMe[6] = 0.f;	    pFillMe[10] = fZFar*nf;     	pFillMe[14] = -1.f;
	pFillMe[3] = 0.f;	  pFillMe[7] = 0.f;	    pFillMe[11] = fZNear*fZFar*nf;	pFillMe[15] = 0.f;	*/

	
}

//d3d

void CMatrixGenerator::PespectiveLH(float*& pFillMe,float fFovy,float fAspect, float fZNear, float fZFar) 
{
	
/*xScale     0          0               0
0        yScale       0               0
0          0       zf/(zf-zn)         1
0          0       -zn*zf/(zf-zn)     0
where:
yScale = cot(fovY/2)

xScale = aspect ratio / yScale*/

	float alpha=DEG2RAD(fFovy/2.0);
	float yScale=cosf(alpha)/sinf(alpha);
	float xScale=yScale/fAspect;

	pFillMe[0] = xScale;
	pFillMe[1] = 0.f;
	pFillMe[2] = 0.f;
	pFillMe[3] = 0.f;

	pFillMe[4] = 0.f;
	pFillMe[5] = yScale;
	pFillMe[6] = 0.f;
	pFillMe[7] = 0.f;

	pFillMe[8] = 0.f;
	pFillMe[9] = 0.f;
	pFillMe[10] = fZFar/(fZFar-fZNear);
	pFillMe[11] = 1.f;

	pFillMe[12] = 0.f;
	pFillMe[13] = 0.f;
	pFillMe[14] = -fZNear*fZFar/(fZFar-fZNear);
	pFillMe[15] = 0.f;
}
