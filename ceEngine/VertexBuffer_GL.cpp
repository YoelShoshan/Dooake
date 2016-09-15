#include "stdafx.h"
#include "VertexBuffer_GL.h"
#include "Defines.h"
#include "assert.h"
#include "Defines.h"
#include "LogFile.h"
//extern CLogFile*  g_pLogFile;

#include <gl\gl.h>	
#include "ce.h"
#include "Profiler.h"


CVertexBuffer_GL::CVertexBuffer_GL()
{
	m_uiLength=0;
	m_dwVBFlags=0;
	m_uiVertexSizeBytes=0;
	m_iLockAccess = CR_UNLOCKED;
	m_pMergedData=NULL;
	m_uiMergedVBOID=0;
}
CVertexBuffer_GL::~CVertexBuffer_GL()
{
	if (m_dwVBFlags&VB_MEM_SYS)
	{
		if (m_dwVBFlags & VB_SINGLE)
		{
			for (int i=0;i<STREAM_MAX;i++)
			{	
				if (!m_Streams[i].m_bUsed)
					continue;
				SafeDeleteArray(m_Streams[i].pData);
			}
		}			
		else // if (m_dwVBFlags & VB_MERGED) DEFAULT
			SafeDeleteArray(m_pMergedData);
	}
	else //if (m_dwVBFlags&VB_MEM_GPU)
	{
		if (m_dwVBFlags & VB_SINGLE)
		{
			for (int i=0;i<STREAM_MAX;i++)
			{	
				if (!m_Streams[i].m_bUsed)
					continue;
				glDeleteBuffersARB(1,&m_Streams[i].uiVBOID);
			}
		}
		else // if (m_dwVBFlags & VB_MERGED) DEFAULT
		{
			glDeleteBuffersARB(1,&m_uiMergedVBOID);
		}

	}

	
}

UINT CVertexBuffer_GL::GetLength(void) const
{
	return m_uiLength;
}



// AddStream( STREAM_POSITION, CR_FLOAT3)
void CVertexBuffer_GL::AddStream(DWORD dwStreamType,DWORD dwElementType)
{
	CR_ERROR((dwStreamType >= STREAM_MAX),"Trying to allocate VB of size 0!"); 
	m_Streams[dwStreamType].m_bUsed       = true;


	m_Streams[dwStreamType].crType = dwElementType;

	switch(dwElementType)
	{
	case CR_FLOAT1:
		m_Streams[dwStreamType].uiElementsNum = 1;
		m_Streams[dwStreamType].dwElementType = GL_FLOAT;
	break;
	case CR_FLOAT2:
		m_Streams[dwStreamType].uiElementsNum = 2;
		m_Streams[dwStreamType].dwElementType = GL_FLOAT;
	break;
	case CR_FLOAT3:
		m_Streams[dwStreamType].uiElementsNum = 3;
		m_Streams[dwStreamType].dwElementType = GL_FLOAT;
	break;
	case CR_FLOAT4:
		m_Streams[dwStreamType].uiElementsNum = 4;
		m_Streams[dwStreamType].dwElementType = GL_FLOAT;
	break;
	case CR_BYTE4:
		m_Streams[dwStreamType].uiElementsNum = 4;
		m_Streams[dwStreamType].dwElementType = GL_UNSIGNED_BYTE;
	break;
	}
}

UINT CVertexBuffer_GL::GetElementTypeSizeBytes(DWORD dwElementType) const
{
	switch(dwElementType)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
		break;

	case GL_BYTE:
		break;

	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);
		break;

	case GL_SHORT:
		return sizeof(GLshort);
		break;	

	case GL_UNSIGNED_SHORT:
		return sizeof(GLushort);
		break;

	case GL_INT:
		return sizeof(GLint);
		break;

	case GL_UNSIGNED_INT:
		return sizeof(GLuint);
		break;

	case GL_DOUBLE:
		return sizeof(GLdouble);
		break;
	}

	CR_ERROR(1,"an unkown element type used inside a VB"); // not a type we know...

	return sizeof(GLfloat); // defeault
}

void CVertexBuffer_GL::AllocateStreamsSINGLE(int iUsageType, int iLength)
{
	m_uiVertexSizeBytes=0; // not relevant...

	for (int i=0;i<STREAM_MAX;i++)
	{	
		if (!m_Streams[i].m_bUsed)
			continue;
		m_Streams[i].dwElementSizeBytes = GetElementTypeSizeBytes(m_Streams[i].dwElementType);
		m_Streams[i].iOffset = 0;				
	}


	if (m_dwVBFlags&VB_MEM_SYS)
	{
		for (int i=0;i<STREAM_MAX;i++)
		{
			if (!m_Streams[i].m_bUsed)
				continue;
			m_Streams[i].pData = new char[m_uiLength * m_Streams[i].uiElementsNum*m_Streams[i].dwElementSizeBytes];			
		}
		return;
	}
	else// if (m_dwVBFlags&VB_MEM_GPU)              DEFAULT
	{
		for (int i=0;i<STREAM_MAX;i++)
		{
			if (!m_Streams[i].m_bUsed)
				continue;
			glGenBuffersARB( 1, &m_Streams[i].uiVBOID );
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_Streams[i].uiVBOID);
			
			if (iUsageType==CR_STREAM_DYNAMIC)
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_uiLength * m_Streams[i].uiElementsNum*m_Streams[i].dwElementSizeBytes, NULL,GL_STATIC_DRAW_ARB);
			else //if (iUsageType==CR_STREAM_STATIC) DEFAULT
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_uiLength * m_Streams[i].uiElementsNum*m_Streams[i].dwElementSizeBytes, NULL,GL_DYNAMIC_DRAW_ARB);
						
		}
		
		return;
	}

	
}

void CVertexBuffer_GL::AllocateStreamsMERGED(int iUsageType, int iLength)
{
	//void* pData;
	int iOffset=0;
	for (int i=0;i<STREAM_MAX;i++)
	{	
		if (!m_Streams[i].m_bUsed)
			continue;

		m_Streams[i].iOffset = iOffset;
		m_Streams[i].dwElementSizeBytes = GetElementTypeSizeBytes(m_Streams[i].dwElementType);
		iOffset+= m_uiLength * m_Streams[i].dwElementSizeBytes * m_Streams[i].uiElementsNum;
	}

	m_uiVertexSizeBytes = iOffset; // just for clarity...

	m_pMergedData = new char[m_uiLength * m_uiVertexSizeBytes];

	if (m_dwVBFlags&VB_MEM_SYS)
	{
		for (int i=0;i<STREAM_MAX;i++)
		{
			if (!m_Streams[i].m_bUsed)
				continue;
			m_Streams[i].pData = ((char*) m_pMergedData) + m_Streams[i].iOffset;
		}
		return;
	}
	else// if (m_dwVBFlags&VB_MEM_GPU)              DEFAULT
	{
		glGenBuffersARB( 1, &m_uiMergedVBOID );
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_uiMergedVBOID);
			
		if (iUsageType==CR_STREAM_DYNAMIC)
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_uiLength * m_uiVertexSizeBytes, NULL,GL_DYNAMIC_DRAW_ARB);
		else // if (iUsageType==CR_STREAM_STATIC)
			glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_uiLength * m_uiVertexSizeBytes , NULL,GL_STATIC_DRAW_ARB);
		return;
	}


	 
}

// AllocateStreams(VB_MEM_GPU | VB_MERGED , GL_STATIC_DRAW_ARB)
void CVertexBuffer_GL::AllocateStreams(int iLength,DWORD dwVBFlags,int iUsageType)
{

	// NOT USING THE VD YET!

	m_uiLength = iLength;
	CR_ERROR(m_uiLength==0,"Trying to allocate VB of size 0!");

	m_dwVBFlags = dwVBFlags;

	if (m_dwVBFlags & VB_SINGLE)
		AllocateStreamsSINGLE(iUsageType,iLength);
	else //	if (m_dwVBFlags & VB_MERGED) // DEFAULT
		AllocateStreamsMERGED(iUsageType,iLength);
}

void* CVertexBuffer_GL::GetStream(DWORD dwStreamType) const
{			
	CR_ERROR(m_iLockAccess == CR_UNLOCKED,"Trying to get a non locked vb stream!");
	
	if (m_dwVBFlags & VB_SINGLE)
	{
		CR_ERROR(m_Streams[dwStreamType].pData==NULL,"Trying to get a non mapped stream!");
		return m_Streams[dwStreamType].pData;
	}
	else //if (m_dwVBFlags & VB_MERGED)
	{
		CR_ERROR(m_pMergedData==NULL,"Trying to get a non mapped stream!");
		return (char*) m_pMergedData + m_Streams[dwStreamType].iOffset;
	}
}

void CVertexBuffer_GL::Lock(int iAccess)
{
	m_iLockAccess = iAccess;

	if (m_dwVBFlags&VB_MEM_SYS)
		return;
	else //if (m_dwVBFlags&VB_MEM_GPU) // default
	{
		if (m_dwVBFlags&VB_SINGLE)
		{
			for (int i=0;i<STREAM_MAX;i++)
			{
				if (!m_Streams[i].m_bUsed)
					continue;
				glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_Streams[i].uiVBOID);	
				if (iAccess==CR_LOCK_WRITE_ONLY)			
					m_Streams[i].pData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
				else
				if (iAccess==CR_LOCK_READ_ONLY)
				{
					m_Streams[i].pData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_ONLY_ARB);
				}
				else // if (m_dwVNFlags& CR_LOCK_READ_WRITE) DEFAULT
				{
					m_Streams[i].pData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
				}
			}
		}
		else // if (m_dwVBFlags&VB_MERGED) DEFAULT
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_uiMergedVBOID);

			if (iAccess== CR_LOCK_WRITE_ONLY)
				m_pMergedData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
			else
			if (iAccess== CR_LOCK_READ_ONLY)
				m_pMergedData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_ONLY_ARB);
			else // if (m_dwVNFlags& CR_LOCK_READ_WRITE) DEFAULT
				m_pMergedData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
			
		}
	}
	
}
void CVertexBuffer_GL::Unlock(void)
{
	if (m_dwVBFlags&VB_MEM_SYS)
	{
		m_iLockAccess = CR_UNLOCKED;
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,NULL);
		return;
	}
	else //if (m_dwVBFlags&VB_MEM_GPU) // DEFAULT
	{
		if (m_dwVBFlags&VB_SINGLE)
		{
			for (int i=0;i<STREAM_MAX;i++)
			{
				if (!m_Streams[i].m_bUsed)
					continue;
				glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_Streams[i].uiVBOID);	
				glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
				m_Streams[i].pData=NULL;
			}
		}
		else // if (m_dwVBFlags&VB_MERGED)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_uiMergedVBOID);	
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
			m_pMergedData=NULL;
		}	

		m_iLockAccess = CR_UNLOCKED;
		return;
	}
}

bool CVertexBuffer_GL::IsStreamActive(DWORD dwStream) const
{
	return (m_Streams[dwStream].m_bUsed);
}

char CVertexBuffer_GL::GetStreamCRtype(DWORD dwStream) const
{
	return m_Streams[dwStream].crType;

}

// binds all streams that are present in the given vertdecl
void CVertexBuffer_GL::BindStreams(const CVertDecl* pVD) const
{
	for (int i=0;i<STREAM_MAX;i++)
		if (pVD->IsStreamActive(i))
			BindStream(i,pVD);

}
void CVertexBuffer_GL::BindStream(DWORD dwStreamType,const CVertDecl* pVD) const
{

	
	if (m_dwVBFlags&VB_MEM_SYS)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,NULL);

		switch(dwStreamType)
		{
			case STREAM_POSITION:
			glEnableClientState(GL_VERTEX_ARRAY);		
			glVertexPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
				0,m_Streams[dwStreamType].pData);		
			break;
			case STREAM_COLOR:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
				0,m_Streams[dwStreamType].pData);		
			break;
			case STREAM_NORMAL:
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(m_Streams[dwStreamType].dwElementType,0,m_Streams[dwStreamType].pData);		
			break;

			case STREAM_TEXCOORD0:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 0);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
			break;
			case STREAM_TEXCOORD1:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD2:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 2);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD3:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 3);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD4:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 4);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD5:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 5);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD6:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 6);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
			case STREAM_TEXCOORD7:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 7);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,m_Streams[dwStreamType].pData);		
				break;
		}					
		
	}
	else //if (m_dwVBFlags&VB_MEM_GPU) // DEFAULT
	{

		if (m_dwVBFlags& VB_SINGLE)
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_Streams[dwStreamType].uiVBOID);
		else // if (m_dwVBFlags& VB_MERGED) DEFAULT
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_uiMergedVBOID);
			
		switch(dwStreamType)
		{
			case STREAM_POSITION:
			glEnableClientState(GL_VERTEX_ARRAY);		
			glVertexPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
				0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_COLOR:

			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
				0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_NORMAL:
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(m_Streams[dwStreamType].dwElementType,0,(char*)NULL + m_Streams[dwStreamType].iOffset);
			break;

			case STREAM_TEXCOORD0:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 0); // should be done on higher lvl! (app should be lazy)
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD1:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD2:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 2);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD3:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 3);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD4:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 4);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD5:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 5);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD6:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 6);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			case STREAM_TEXCOORD7:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + 7);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(m_Streams[dwStreamType].uiElementsNum,m_Streams[dwStreamType].dwElementType,
					0,(char*)NULL + m_Streams[dwStreamType].iOffset);		
			break;
			
		}					

	}

}

void* CVertexBuffer_GL::StreamCopySubData(const char* pData, DWORD dwStreamType,int iLength, int iOffset)
{
	// dummy dummy dummy dummy dummy dummy 
	return NULL;
}

#define _alloca16( x )					((void *)((((int)_alloca( (x)+15 )) + 15) & ~15))

void CVertexBuffer_GL::CalculateTBN(int iIndexCount,
									 const unsigned int* puiIndices,
									 int iVertxOffset,
									 int iVertCount,
									 const CVec3f* pPosition,
									 const CVec2f* pTexCoords,
									 CVec3f* pNormals, 
									 CVec3f* pTangents, 
									 CVec3f* pBinormals)const
{
	int i;

	bool *used = (bool *)_alloca16( iVertCount * sizeof( used[0] ) );
	memset( used, 0, iVertCount * sizeof( used[0] ) );

	//idPlane *planesPtr = planes;
	 

	for ( i = 0; i < iIndexCount; i += 3 ) 
	{
		//idDrawVert *a, *b, *c;

		const CVec3f *vA, *vB, *vC;
		const CVec2f *tcA, *tcB, *tcC;
		

		unsigned long signBit;
		float d0[5], d1[5], f, area;
		CVec3f n, t0, t1;

		int v0 = puiIndices[i + 0];
		int v1 = puiIndices[i + 1];
		int v2 = puiIndices[i + 2];

		vA = pPosition + v0;
		vB = pPosition + v1;
		vC = pPosition + v2;

		tcA = pTexCoords + v0;
		tcB = pTexCoords + v1;
		tcC = pTexCoords + v2;



		d0[0] = vB->v[0] - vA->v[0];
		d0[1] = vB->v[1] - vA->v[1];
		d0[2] = vB->v[2] - vA->v[2];
		d0[3] = tcB->v[0] - tcA->v[0];
		d0[4] = tcB->v[1] - tcA->v[1];

		d1[0] = vC->v[0] - vA->v[0];
		d1[1] = vC->v[1] - vA->v[1];
		d1[2] = vC->v[2] - vA->v[2];
		d1[3] = tcC->v[0] - tcA->v[0];
		d1[4] = tcC->v[1] - tcA->v[1];

		// normal
		n.v[0] = d1[1] * d0[2] - d1[2] * d0[1];
		n.v[1] = d1[2] * d0[0] - d1[0] * d0[2];
		n.v[2] = d1[0] * d0[1] - d1[1] * d0[0];

		//f = idMath::RSqrt( n.x * n.x + n.y * n.y + n.z * n.z );
		//n.x *= f;
		//n.y *= f;
		//n.z *= f;

		n.Normalize();


		//planesPtr->SetNormal( n );
		//planesPtr->FitThroughPoint( a->xyz );
		//planesPtr++;

		// area sign bit
		area = d0[3] * d1[4] - d0[4] * d1[3];
		signBit = ( *(unsigned long *)&area ) & ( 1 << 31 );

		// first tangent
		t0.v[0] = d0[0] * d1[4] - d0[4] * d1[0];
		t0.v[1] = d0[1] * d1[4] - d0[4] * d1[1];
		t0.v[2] = d0[2] * d1[4] - d0[4] * d1[2];

		

		t0.Normalize();
		//f= signBit ? -1.0F : 1.0F;
		f= signBit ? 1.0F : -1.0F;
		t0 *= f;




		// second tangent
		t1.v[0] = d0[3] * d1[0] - d0[0] * d1[3];
		t1.v[1] = d0[3] * d1[1] - d0[1] * d1[3];
		t1.v[2] = d0[3] * d1[2] - d0[2] * d1[3];

		
		t1.Normalize();
		//f= signBit ? -1.0F : 1.0F;
		f= signBit ? 1.0F : -1.0F;
		t1 *= f;



		if ( used[v0] ) {
			pNormals[v0]   += n;
			pTangents[v0]  += t0;
			pBinormals[v0] += t1;

		} else {
			
			pNormals[v0]   = n;
			pTangents[v0]  = t0;
			pBinormals[v0] = t1;
			used[v0] = true;

		}

		if ( used[v1] ) {

			pNormals[v1]   += n;
			pTangents[v1]  += t0;
			pBinormals[v1] += t1;

		} else {
			
			pNormals[v1]   = n;
			pTangents[v1]  = t0;
			pBinormals[v1] = t1;

			used[v1] = true;
		}

		if ( used[v2] ) {

			pNormals[v2]   += n;
			pTangents[v2]  += t0;
			pBinormals[v2] += t1;

		} else {		
			pNormals[v2]   = n;
			pTangents[v2]  = t0;
			pBinormals[v2] = t1;

			used[v2] = true;
		}


	}

	for ( int i = 0; i < iVertCount; i++ )
	{
		CVec3f &v = pNormals[i];
	

		v.Normalize();

		{
			CVec3f &t = pTangents[i];

			t -= v * ( t * v ) ;
			
			t.Normalize();
		}
		{
			CVec3f &t = pBinormals[i];

			t -= v * ( t * v );
			t.Normalize();
		}
	}

}
