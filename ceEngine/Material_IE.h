#ifndef CRIMSON_INTERFACE_EXPORT_MATERIAL_H
#define CRIMSON_INTERFACE_EXPORT_MATERIAL_H

#pragma once

#include "ReferenceCount.h"


#include "RenderToTexture.h"
#include "Q3AShader_Data.h"

#define MATERIAL_SORT_OPAQUE              0
#define MATERIAL_SORT_OPAQUE_TRIGGER_POSTPROCESS 1
#define MATERIAL_SORT_BLEND               2
#define MATERIAL_SORT_BLEND_TRIGGER_POSTPROCESS 3
#define MATERIAL_SORT_POSTPROCESS         4

class IMaterial_Export : public CReferenceCount
{
public:
	IMaterial_Export() {};
	virtual ~IMaterial_Export() {};

	virtual bool Load(const char* pchFileName) = 0;

	virtual bool GenerateSimpleTextureMaterial(const char* pchTexName) = 0;

	virtual bool GenerateAlphaBlendedAlwaysDrawTextureMaterial(const char* pchTexName) = 0;
	
	virtual bool GeneratePerPixelLitMaterial(const char* pchTexName) = 0;

	virtual bool GenerateFromQ3AShader(const CQ3A_Shader_Data* pQ3AShader) = 0;
	
	virtual void Bind(void) const = 0;

	virtual const char* GetName(void) const = 0;

	virtual unsigned int GetID(void) const = 0;

	virtual unsigned int GetSort(void) const = 0;
	
	virtual void InjectRTT(IRenderToTexture* pRTT) = 0;

	virtual bool IsTransparent() = 0;

};


#endif