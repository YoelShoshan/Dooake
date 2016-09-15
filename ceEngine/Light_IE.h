#ifndef CRIMSON_LIGHT_INTERFACE_H
#define CRIMSON_LIGHT_INTERFACE_H

#pragma once

#include "Math3D.h"
#include "Renderable_IE.h"

#include "RenderToTexture.h"

#include <vector>

#include "Frustum.h"

//TODO: add warning if some value wasn't set befure updating matrix

class IRenderable_Export;
class ILight_Export
{
public:
	ILight_Export() {};
	virtual ~ILight_Export() {};

	// dirty requires for example re-rendering of the shadow maps (due to moving entities in its sight)
	virtual bool GetDirty(void) = 0;
	virtual void SetDirty(bool bDirty) = 0;

	virtual void SetActive(bool bActive) = 0;
	virtual bool GetActive() = 0;

	virtual void BindRenderToDepthMap() = 0;
	virtual void UnbindRenderToDepthMap() = 0;

	//virtual void BindDepthMap(int iSamplerNum) = 0;
	virtual IRenderToTexture* GetDepthMapRTT() = 0;

	virtual void SetPos(const CVec3f& vPos) = 0;
	virtual CVec3f GetPos() const= 0;

	virtual void SetColor(const CVec3f& vColor)= 0;
	virtual CVec3f GetColor() const= 0;

	virtual void SetRadius(float fRadius) = 0;
	virtual float GetRadius() const = 0;
	
	virtual void ClearVisibleRenderables(void)= 0;
	virtual void AddVisibleRenderable(const IRenderable_Export* pRend)= 0;
	/*virtual const std::vector<const IRenderable_Export*>* GetVisibleRenderables() const = 0;*/

	virtual  CFrustum* GetFrustum(void) = 0;

	////////////////////////////////////////////////////////////////////////////////////
	// Shadows
	virtual void SetLookAt(const CVec3f& vLookAt)= 0;
	virtual CVec3f GetLookAt() const = 0;
	virtual void SetUp(const CVec3f& vUp)= 0;
	virtual void SetFOV(float fFov)= 0;
	virtual void SetAspectRatio(float fAR)= 0;
	virtual void SetFrustumNearPlane(float fVal)= 0;
	virtual void SetFrustumFarPlane(float fVal)= 0;
	virtual void Update() = 0;
	virtual const float* GetProjectionMatrix() = 0;
	virtual const float* GetViewMatrix() = 0;
	virtual const float* GetViewProjectionMatrix() = 0;
	////////////////////////////////////////////////////////////////////////////////////



};



#endif