#ifndef CRIMSON_ENGINE_RENDERABLE_H
#define CRIMSON_ENGINE_RENDERABLE_H

#pragma once

#include "Material_IE.h"
//#include "VertexDecleration.h"
#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"

#include "Math3D.h"

#include "SmartPointer.h"

#include "Renderable_IE.h"


// CEngineRenderable class -
// an inner engine class that supports generation of basic shapes
// for example - Full screen quad for the post processing effects,
//               Spheres for general debugging purposes etc.


#define ENGINE_RENDERABLE_FULL_SCREEN_QUAD 1
/*#define ENGINE_RENDERABLE_TESSELATED_QUAD  2*/
#define ENGINE_RENDERABLE_SPHERE           3 // place holder - not implemented yet


#define SCALE_DOWN 4


class CEngineRenderable : public IRenderable_Export
{
public:	
	CEngineRenderable();
	~CEngineRenderable();


	bool Generate(DWORD dwType/*,int iTessLevel=0*/);

	

	unsigned GetSort(void) const;


	// if buffers aren't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	bool SetMaterial(CSmartPointer<IMaterial_Export> pMat);

	// if material isn't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	//bool SetBuffers(const IVertexBuffer* pVB, const IIndexBuffer* pIB);

	bool SetVertexBuffer(const IVertexBuffer_Export* pVB,bool bFree);
	bool SetIndexBuffer(const IIndexBuffer* pIB,bool bFree);

	bool SetDrawCallParameters(int iStart, int iCount, int iBaseVertexIndex, int iMinIndex,int iVertsNum);

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

	const std::set<ILight_Export*>* GetEffectingLights() const
	{
		// Lighting is irrelevant here
		return NULL;
	}

	virtual void AddEffectingLight(ILight_Export* pLight)
	{
		// Lighting is irrelevant here
	}

	virtual void ClearEffectingLights()
	{
		// Lighting is irrelevant here
	}


	const IMaterial_Export* GetMaterial(void) const { return m_spMaterial;};

	// expects column based layout !
	void SetTransform(const float* pMat);

	const float* GetTransform(void) const;


	// dummy
	const BBox* GetBBox(void) const
	{		
		return NULL;
	}

	// dummy
	void SetBBox(BBox& bbox)
	{

	}

	void SetDontRender(bool bDontRender)
	{

	}

	bool GetDontRender(void) const
	{
		return false;
	}

	bool IsShadowCaster(void) const { return false;};
	void SetShadowCaster(bool bSet) {/*dummy*/};


private:

	bool GenerateFullScreenQuad();

	/*bool GenerateTesselatedQuad(int iTessLevel);*/

	bool GenerateSphere();

	
	
	CMatrix        m_Transform;
	float*         m_pfTransform; // will be removed when i change my matrix system to column based...
	bool m_bTransformIsIdentity;

	CSmartPointer<IMaterial_Export>     m_spMaterial;

	bool           m_bFreeVB;
	const IVertexBuffer_Export* m_pVertexBuffer;
	bool           m_bFreeIB;
	const IIndexBuffer*  m_pIndexBuffer;
	const UINT* m_pIndicesData;
	
	int m_iIndicesStart;
	int m_iIndicesCount;
	int m_iVertsNum;
	int m_iBaseVertexIndex;
	int m_iMinIndex;

	
};


#endif