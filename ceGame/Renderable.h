#ifndef CRIMSON_RENDERABLE_H
#define CRIMSON_RENDERABLE_H

#pragma once

#include "Material_IE.h"
//#include "VertexDecleration.h"
#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"

#include "Math3D.h"

#include "SmartPointer.h"

#include "Renderable_IE.h"

#include "Light_IE.h"


#define RENDERABLE_BBOX_MIN 0
#define RENDERABLE_BBOX_MAX 0


class CRenderable : public IRenderable_Export
{
public:
	CRenderable();
	~CRenderable();

	// if buffers aren't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	bool SetMaterial(CSmartPointer<IMaterial_Export> pMat);

	// if material isn't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	//bool SetBuffers(const IVertexBuffer* pVB, const IIndexBuffer* pIB);

	bool SetVertexBuffer(const IVertexBuffer_Export* pVB,bool bFree);
	bool SetIndexBuffer(const IIndexBuffer* pIB,bool bFree);

	bool SetDrawCallParameters(int iStart, int iCount,int iBaseVertexIndex, int iMinIndex,int iVertsNum);

	void	SetIndices_Data(const UINT* pIndices);
	const UINT*	GetIndices_Data(void) const;


    const IVertexBuffer_Export* GetVB(void) const          { return m_pVertexBuffer;};
    const IIndexBuffer*  GetIB(void)  const        { return m_pIndexBuffer;};

	int GetIndicesStart(void) const ;	
	int GetIndicesCount(void) const;
	int GetVerticesNum(void) const;
	int GetBaseVertexIndex(void) const;
	int GetMinIndex(void) const;

	bool BindMaterial(void) const;
	//bool Validate(CSmartPointer<IMaterial_Export> spMat,const IVertexBuffer_Export* pVB);
	const IMaterial_Export* GetMaterial(void) const { return m_spMaterial;};
	// expects column based layout !
	void SetTransform(const float* pMat);
	const float* GetTransform(void) const;



	void ClearEffectingLights(void);

	void AddEffectingLight(ILight_Export* pLight);
	const std::set<ILight_Export*>* GetEffectingLights() const;

	// if the returned value is NULL it means always render
	const BBox* GetBBox(void) const;

	void SetBBox(BBox& bbox);

	void SetDontRender(bool bDontRender);
	bool GetDontRender(void) const;

	bool IsShadowCaster(void) const { return m_bShadowCaster;};
	void SetShadowCaster(bool bSet) {m_bShadowCaster = bSet;};

	unsigned int GetSort() const;

private:

	BBox* m_pBBox;
	bool m_bDontRender;

	
	CMatrix        m_Transform;
	float*         m_pfTransform; // will be removed when i change my matrix system to column based...
	bool m_bTransformIsIdentity;

	CSmartPointer<IMaterial_Export>     m_spMaterial;

	bool           m_bFreeVB;
	const IVertexBuffer_Export* m_pVertexBuffer;
	bool           m_bFreeIB;
	const IIndexBuffer*  m_pIndexBuffer;
	
	int m_iIndicesStart;
	int m_iIndicesCount;
	int m_iVertsNum;

	int m_iBaseVertexIndex;
	int m_iMinIndex;

	bool m_bShadowCaster;

	const UINT* m_pIndicesData;

	std::set<ILight_Export*> m_EffectingLights;

	
};


#endif