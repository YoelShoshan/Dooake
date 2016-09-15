#ifndef CRIMSON_VERTEX_DECLERATION_GL_H
#define CRIMSON_VERTEX_DECLERATION_GL_H

#pragma once

#include "VertexDecleration.h"

class CVertDecl_GL : public CVertDecl
{
public:
	CVertDecl_GL();
	~CVertDecl_GL();
	void Allocate(void);
	void Bind(void) const;
};



#endif