#ifndef CRIMSON_LIGHT_H
#define CRIMSON_LIGHT_H

#pragma once

#include "Math3D.h"
#include "Renderable_IE.h"

#include <vector>

#include "Light_IE.h"

#include "Frustum.h"

#include "RenderToTexture.h"
//TODO: add warning if some value wasn't set befure updating matrix

class IRenderable_Export;
class CLight : public ILight_Export
{
public:
	CLight();
	~CLight();
	
	bool GetDirty(void) { return m_bDirty;};
	void SetDirty(bool bDirty) { m_bDirty = bDirty;};

	void SetPos(const CVec3f& vPos);
	CVec3f GetPos() const;

	void SetColor(const CVec3f& vColor);
	CVec3f GetColor() const;

	void SetRadius(float fRadius) { m_fRadius = fRadius;};
	float GetRadius() const {return m_fRadius;};


	void ClearVisibleRenderables(void);
	void AddVisibleRenderable(const IRenderable_Export* pRend);

	/*const std::vector<const IRenderable_Export*>* GetVisibleRenderables() const;*/

	////////////////////////////////////////////////////////////////////////////////////
	// Shadows
	void SetActive(bool bActive);
	bool GetActive();

	void BindRenderToDepthMap();
	void UnbindRenderToDepthMap();

	//void BindDepthMap(int iSamplerNum);
	IRenderToTexture* GetDepthMapRTT();

	void SetLookAt(const CVec3f& vLookAt);
	CVec3f GetLookAt() const;
	void SetUp(const CVec3f& vUp);
	void SetFOV(float fFov);
	void SetAspectRatio(float fAR);
	void SetFrustumNearPlane(float fVal);
	void SetFrustumFarPlane(float fVal);
	void Update();
	const float* GetProjectionMatrix();
	const float* GetViewMatrix();
	const float* GetViewProjectionMatrix();
	////////////////////////////////////////////////////////////////////////////////////

	 CFrustum* GetFrustum(void);

private:

	bool m_bDirty;
	CVec3f m_vPos;		
	CVec3f m_vColor;
	float m_fRadius;
	
	bool  m_bActive;
	IRenderToTexture*   m_pRTTDepthMap;

	std::vector<const IRenderable_Export*> m_VisibleRenderables;

	CFrustum m_Frustum;

	//////////////////////////////////////////////////////////////////////////////////////
	// Shadows
	CVec3f m_vLookAt;
	CVec3f m_vUp;

	float* m_pfView; 
	float* m_pfProjection; 
	float* m_pfViewProjection; 

	float m_fNearPlane;
	float m_fFarPlane;
	float m_fAspectRatio;
	float m_fAR;
	float m_fFov;
	//////////////////////////////////////////////////////////////////////////////////////

};



#endif