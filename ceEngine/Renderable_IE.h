#ifndef CRIMSON_RENDERABLE_INTERFACE_EXPORT_H
#define CRIMSON_RENDERABLE_INTERFACE_EXPORT_H

#pragma once



#define RENDERABLE_GENERATE_NONE 0
#define RENDERABLE_GENERATE_PPL  1


#include "Material_IE.h"
#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"
#include "Light_IE.h"


#include "SmartPointer.h"

#include <set>

class CLight;
class IRenderable_Export
{
public:
	IRenderable_Export() {};
	virtual ~IRenderable_Export() {};

	// if buffers aren't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	virtual bool SetMaterial(CSmartPointer<IMaterial_Export> pMat)= 0;

	// if material isn't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	//bool SetBuffers(const IVertexBuffer* pVB, const IIndexBuffer* pIB);

	virtual bool SetVertexBuffer(const IVertexBuffer_Export* pVB,bool bFree)= 0;
	virtual bool SetIndexBuffer(const IIndexBuffer* pIB,bool bFree)= 0;
	virtual bool SetDrawCallParameters(int iStart, int iCount, int iBaseVertexIndex, int iMinIndex,int iVrtsNum) = 0;

	virtual void	SetIndices_Data(const UINT* pIndices)	= 0;
	virtual const UINT*	GetIndices_Data(void) const				= 0;

	virtual  const IVertexBuffer_Export* GetVB(void) const	= 0;
	virtual  const IIndexBuffer*  GetIB(void) const			= 0;

	virtual int GetIndicesStart(void) const = 0;
	virtual int GetIndicesCount(void) const = 0;
	virtual int GetVerticesNum(void) const = 0;

	virtual int GetBaseVertexIndex(void) const = 0;
	virtual int GetMinIndex(void) const = 0;

	

	//virtual bool Validate(CSmartPointer<IMaterial_Export> spMat,const IVertexBuffer_Export* pVB) = 0;

	virtual bool BindMaterial(void) const= 0;
	
	virtual const IMaterial_Export* GetMaterial(void) const = 0;

	// expects column based layout !
	virtual void SetTransform(const float* pMat)= 0;

	virtual const float* GetTransform(void) const = 0;

	virtual unsigned int GetSort(void) const = 0;


	// lights

	// if renderable doesn't care about lighting, it can override this function and always return NULL

	virtual void ClearEffectingLights() = 0;
	virtual const std::set<ILight_Export*>* GetEffectingLights() const = 0;
	virtual void AddEffectingLight(ILight_Export* pLight) = 0;

	virtual const BBox* GetBBox(void) const = 0;

	virtual void SetBBox(BBox& bbox) = 0;

	virtual void SetDontRender(bool bDontRender) = 0;
	virtual bool GetDontRender(void) const = 0;
	
	virtual bool IsShadowCaster(void) const = 0;
	virtual void SetShadowCaster(bool bSet) = 0;
};






#endif