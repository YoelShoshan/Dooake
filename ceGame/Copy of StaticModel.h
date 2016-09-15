#ifndef CRIMSON_STATIC_MODEL_H
#define CRIMSON_STATIC_MODEL_H

#pragma once

#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "Renderable.h"
#include "Material_IE.h"

#include "StaticModel_Shared.h"

#include "SmartPointer.h"
#include "Light_IE.h"

#include "RenderModel.h"

class CStaticModel  : public IRenderModel
{
public:
	CStaticModel();
	~CStaticModel();

	//////////////////////////////////////////////
	// implementations of the IEntity demands
	void Render(int iTech);
	void Update(float fDeltaTime,bool bVisible);
	
	void SetPosition(const CVec3f& vPos);
	void SetAngles(const CVec3f& vAngles);
	CVec3f GetAngles(void) const { return m_vAngles; };
	CVec3f GetPreviousAngles(void) const { return m_vPreviousAngles; };
	CVec3f GetCurrentAngles(void) const { return m_vCurrentAngles; };

	void BlendAnglesState(float fAlpha)  {};

	void ClearEffectingLights(void);
	void AddEffectingLight(ILight_Export* pLight);
	//////////////////////////////////////////////

	void SetShared(CSmartPointer<CStaticModel_Shared> spSet,DWORD dwGenerate);

	// assumes a model was loaded!
	bool GenerateRenderable(DWORD dwGenerate);

	BBox GetBBox(void) { return m_spSharedModel->m_BBox;};
	

	void SetMaterialForAll(CSmartPointer<IMaterial_Export> spMat);

	void SetTransformForAll(float* pfTransf);

	void SetAnglesRotateSpeed(const CVec3f& vAnglesSpeed) { m_vAnglesRotateSpeed = vAnglesSpeed;};
	
	const char* GetName(void) const { return m_pcName; };

private:
	CRenderable* m_pRenderables;

	CSmartPointer<CStaticModel_Shared> m_spSharedModel;

	CVec3f m_vPosition;
	CVec3f m_vAngles;
	CVec3f m_vPreviousAngles;
	CVec3f m_vCurrentAngles;

	CVec3f m_vAnglesRotateSpeed;

	char* m_pcName;
};

#endif