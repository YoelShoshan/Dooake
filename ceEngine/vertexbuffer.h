#ifndef CRIMSON_VERTEX_BUFFER_INTERFACE_H
#define CRIMSON_VERTEX_BUFFER_INTERFACE_H

#pragma once

#include <windows.h>



// a stream can be:

//  (GL_FLOAT GL_FLOAT GL_FLOAT) (GL_FLOAT GL_FLOAT GL_FLOAT) 
// in this case:
// dwElementType = GL_FLOAT
// uiElementsNum = 3
// uiLength      = 2

#include "Math3D.h"

#include "VertexBuffer_IE.h"
#include "VertexDecleration.h"


struct tStream
{
	tStream() { ZeroMemory(this, sizeof(tStream));};
	
	// dimention of element (2/3/4)
	UINT  uiElementsNum;
	// length of the stream (1000_ vertices)
	//UINT  uiLength;

	void* pData;
	
	int iOffset;

	// crimson engine type - CR_FLOAT1,CR_FLOAT2,etc...
	char crType;

	// type of elements (GL_FLOAT)
	DWORD dwElementType;
	UINT  dwElementSizeBytes;
	UINT  uiVBOID;	
	bool  m_bUsed;
};

// DROP THE USAGE!!
class IVertexBuffer : public IVertexBuffer_Export
{
public:
	IVertexBuffer() {};
	virtual ~IVertexBuffer() {};

	virtual void BindStreams(const CVertDecl* pVD) const = 0;

protected:
	tStream m_Streams[STREAM_MAX];

	int m_iLockAccess;
};



#endif