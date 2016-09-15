#ifndef YOEL_BEZIER_H
#define YOEL_BEZIER_H

#pragma once

#include <windows.h>
#include "Math3D.h"

//#include "ZedTracker.h"

#define DEFAULT_BEZIER_PATCH_TESSELATION_LEVEL 7

void CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert);


CVec3f EvaluateQuadricBezierCurve(float fT, const CVec3f &p0, const CVec3f &p1, const CVec3f &p2);
CVec2f EvaluateQuadricBezierCurve(float fT,CVec2f &p0,CVec2f &p1,CVec2f &p2);

CVec3f evalQuadricBezierDerivative(float t, CVec3f &p0,CVec3f &p1,CVec3f &p2);

// pOriginalVerts and pOriginalTC MUST point to valid 9 CVec3f/CVec2f
// pTargetVerts and pTargetTC MUST point to NULL
// returns number of sub3x3patches created
int EvaluateQuadricBezierPatch(CVec3f* pOriginalVerts, CVec3f* pOriginalNormals, CVec2f* pOriginalTC,CVec2f* pOriginalLMC,
							   int iStepsX, int iStepsY,
							   CVec3f* pTargetVerts, CVec3f* pTargetNormals, CVec2f* pTargetTC,CVec2f* pTargetLMC,
							   int iOriginalPatchDimX, int iOriginalPatchDimY,UINT* puiNewIndices,
							   int iStartVertexNum);




class CBezierRoute
{
public:
	CBezierRoute();
	~CBezierRoute();

	void AllocRoute(int iNum);	
    
	// fT should be in the range [0,(m_iVertsNum-1) /2] 
	// (if it's bigger then the last point will be returned)
	CVec3f FindPosInRoute(float fT);


CVec3f* m_pVertsArray;
int        m_iVertsNum;


};










#endif // YOEL_BEZIER_H