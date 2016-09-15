#ifndef CRIMSON_VERTEX_DECLERATION_D3D_H
#define CRIMSON_VERTEX_DECLERATION_D3D_H

#pragma once

#include "VertexDecleration.h"

class CVertDecl_D3D : public CVertDecl
{
public:
	CVertDecl_D3D();
	~CVertDecl_D3D();


	void Allocate(void);
	void Bind(void) const;

private:

	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
};














#endif














