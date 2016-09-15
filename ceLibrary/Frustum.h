#ifndef CRIMSON_FRUSTUM_H
#define CRIMSON_FRUSTUM_H

#pragma once

#include "Math3D.h"

#define FRUSTUM_CHECK_AT_LEAST_PARTLY_IN 0
#define FRUSTUM_CHECK_OUT                1


#define FRUSTUM_BACK_DOWN_LEFT_CORNER   0
#define FRUSTUM_BACK_DOWN_RIGHT_CORNER  1
#define FRUSTUM_BACK_UP_LEFT_CORNER     2
#define FRUSTUM_BACK_UP_RIGHT_CORNER    3
#define FRUSTUM_FRONT_DOWN_LEFT_CORNER  4
#define FRUSTUM_FRONT_DOWN_RIGHT_CORNER 5
#define FRUSTUM_FRONT_UP_LEFT_CORNER    6
#define FRUSTUM_FRONT_UP_RIGHT_CORNER   7
#define FRUSTUM_CORNERS_NUM             8

// This will allow us to create an object to keep track of our frustum
class CFrustum {

public:


	CFrustum();
	~CFrustum();


	void AllocPlanes(int iNum);

	// Call this every time the camera moves to update the frustum
	void CalculateFrustum(const float* pfView, const float* pfProjection);

	void CalculateCornerPoints(const float* pfView, const float* pfProjection);

	void ProjectCornerPoints(const float* pfView, const float* pfProjection,CFrustum* pCamFrustum);

//	bool PointInFrustum(float x, float y, float z);

//	bool SphereInFrustum(float x, float y, float z, float radius);

//	bool CubeInFrustum(float x, float y, float z, float size);*/

	#define COMPLETE_OUT 0
	#define INTERSECT    1
	#define COMPLETE_IN  2

	int BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ) const;

	int BoxInFrustum(CVec3f& vMins, CVec3f& vMaxs) const;

	int PolygonInFrustum(CVec3f* pVerts, int iNum);

	CVector4f* GetCorners(void) { return m_pCorners ; };

	CVector4f* GetProjectedCorners(void) { return m_ProjectedCorners ; };

	void GetProjectedCornersBoundingSquare(float& fXmin,float& fXmax, float& fYmin, float& fYmax) const
	{
		fXmin = m_fXmin;
		fXmax = m_fXmax;
		fYmin = m_fYmin;
		fYmax = m_fYmax;
	}

	CPlane* GetPlanes(void) { return m_pPlanes;};

private:

	void ProjectPoints(CVector4f* points,int iNum, CMatrix& mat);

	float m_fXmin,m_fXmax,m_fYmin,m_fYmax;

	CPlane* m_pPlanes;
	int     m_iPlanesNum;

	CVector4f m_pCorners[FRUSTUM_CORNERS_NUM];
	
	CVector4f m_ProjectedCorners[FRUSTUM_CORNERS_NUM];

	CVector4f m_ActiveProjectedCorners[20];
};

#endif // YOEL_FRUSTUM_H