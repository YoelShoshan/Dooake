#ifndef CRIMSON_RENDERABLE_INTERFACE_H
#define CRIMSON_RENDERABLE_INTERFACE_H

#pragma once

#include "Renderable_IE.h"


class IRenderable : public IRenderable_Export
{
public:
	IRenderable() {};
	virtual ~IRenderable() {};

	// if buffers aren't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	virtual bool SetMaterial(CSmartPointer<IMaterial_Export> pMat) = 0;

	// if material isn't null, then the material is TESTED against the decleration,
	// and returns FALSE if theres a MISMATCH !
	//bool SetBuffers(const IVertexBuffer* pVB, const IIndexBuffer* pIB);

	virtual bool SetVertexBuffer(IVertexBuffer* pVB,bool bFree) = 0;
	virtual bool SetIndexBuffer(IIndexBuffer* pIB,bool bFree) = 0;

	virtual const IVertexBuffer* GetVB(void)    = 0;
	virtual const IIndexBuffer*  GetIB(void)     = 0;


	virtual bool BindMaterial(void) = 0;

	virtual bool Validate(CSmartPointer<IMaterial_Export> spMat,const IVertexBuffer* pVB) = 0;

	virtual const IMaterial_Export* GetMaterial(void) const  = 0;

	// expects column based layout !
	virtual void SetTransform(const float* pMat) = 0;

	virtual const float* GetTransform(void) = 0;

};

#endif