#ifndef CRIMSON_GRAPHICS_API_INTERFACE_H
#define CRIMSON_GRAPHICS_API_INTERFACE_H

#pragma once

#include <windows.h>

#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"
#include "ShaderProgram_IE.h"
#include "Texture.h"
#include "VertexDecleration.h"

#include "GraphicsAPI_IE.h"
#include "SmartPointer.h"
#include "ShaderProgram.h"
#include "SmartPointer.h"



class IGraphicsAPI : public IGraphicsAPI_Export
{
public:
	IGraphicsAPI() {};
	virtual ~IGraphicsAPI() {};



	virtual void DrawPrimitives(int iIndicesStart,int iIndicesCount,int iBaseVertexIndex, int iMinIndex,int iVerticesNum) const = 0;


	virtual CVertDecl*         GenerateVertexDecleration(void)            const = 0;

	// caps
	virtual int GetSamplersNum(void) const = 0;

	// samplers state

	virtual void BindTexture(int iSamplerNum,CSmartPointer<ITexture>& tex) = 0;

	virtual void UnBindTexUnit(int iSamplerNum) = 0;

	virtual void BindVertexShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms) = 0;
	virtual void BindFragmentShader(CSmartPointer<IShaderProgram> spShaderProgram,CShaderUniforms* pUniforms) = 0;

	virtual void BindVB(IVertexBuffer_Export* pVB, const CVertDecl* pVD) = 0;
	virtual void BindIB(const IIndexBuffer* pIB) = 0;


	virtual void SetClamping(int iSamplerNum,char cClamping) = 0;

	virtual void SetFiltering(int iSamplerNum,char cFiltering,char cAnisotropicLevel) = 0;

	// renderer state
	
	virtual void  SetDepthTest(bool bDepthTest) = 0;
	virtual DWORD GetDepthTest(void) const = 0;

	virtual void SetDepthFunc(DWORD dwDepthFunc) = 0 ;
	virtual DWORD GetDepthFunc(void) const = 0;

	virtual void SetDepthWrite(bool bDepthWrite) = 0;
	virtual DWORD GetDepthWrite(void) const = 0;
	
	
	

	virtual void SetCullFunc(DWORD dwCullFunc) = 0 ;
	virtual DWORD GetCullFunc(void) const = 0 ;

	virtual void UpdateAutoUniforms(void) = 0;

	


	// debug
	



};




#endif