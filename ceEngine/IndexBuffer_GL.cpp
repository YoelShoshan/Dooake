#include "stdafx.h"
#include "IndexBuffer_GL.h"
#include "Defines.h"
#include "LogFile.h"
#include <assert.h>

#include "ce.h"
CIndexBuffer_GL::CIndexBuffer_GL()
{
	m_pData = NULL;
	m_iLength=0;
	m_iLockAccess = CR_UNLOCKED;
	m_dwIBFlags = 0;
	m_uiVBOID=0;

}

CIndexBuffer_GL::~CIndexBuffer_GL()
{
	if (m_dwIBFlags & IB_MEM_GPU)
		glDeleteBuffersARB(1, &m_uiVBOID );
}

void* CIndexBuffer_GL::GetData(void)
{
	CR_ERROR(m_iLength==0,"Trying to get data from a 0 length Index Buffer!");
	CR_ERROR(m_iLockAccess==CR_UNLOCKED,"Trying to get data from an unlocked Index Buffer!");

	if (m_dwIBFlags & IB_MEM_SYS)
	{
		CR_ERROR(m_pData==NULL,"Trying to get a Null system Index Buffer!");
		return m_pData;
		
	}
	else // if (dwIBFlags & IB_MEM_GPU) // DEFAULT
	{
		CR_ERROR(m_pData==NULL,"Trying to get a non mapped system Index Buffer!");
		return m_pData;
	} 
}

int CIndexBuffer_GL::GetLength(void) const
{
	return m_iLength;

}

// Allocate(30,IB_MEM_SYS)
void CIndexBuffer_GL::Allocate(int iLength,DWORD dwIBFlags)
{
	CR_ERROR(iLength==0,"Trying to load a 0 length data into an Index Buffer!");

	m_dwIBFlags = dwIBFlags;

	m_iLength = iLength;
	
	if (m_dwIBFlags & IB_MEM_SYS)
	{		
		m_pData = new unsigned int[m_iLength];
	}
	else // if (dwIBFlags & IB_MEM_GPU) // default is gpu
	{
		glGenBuffersARB( 1, &m_uiVBOID );
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_uiVBOID);
		if (dwIBFlags & IB_DYNAMIC)
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_iLength * sizeof(unsigned int),NULL,GL_DYNAMIC_DRAW_ARB);			
		else //if (dwIBFlags & IB_STATIC) == default is static
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_iLength * sizeof(unsigned int),NULL,GL_STATIC_DRAW_ARB);
	}

}

const void* CIndexBuffer_GL::Bind(void)const
{
	if (m_dwIBFlags&IB_MEM_SYS)
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,NULL);
		CR_ERROR(m_pData==0,"Trying to bind IndexBuffer but data is NULL");
		return m_pData;
	}
	else //	if (m_dwFlags&VBFLAG_GPUMEM)
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_uiVBOID);
		return m_pData;
	}

	return NULL;
}

void CIndexBuffer_GL::Lock(int iAccess,UINT uiOffset, UINT uiSize)
{
	assert( (uiOffset==0) && (uiSize==0)); //not supported in opengl yet

	m_iLockAccess = iAccess;

	if (m_dwIBFlags & IB_MEM_SYS)
	{
		m_iLockAccess = iAccess;
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,NULL);
		return;
	}
	else // if (dwIBFlags & IB_MEM_GPU) // DEFAULT
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_uiVBOID);	

		if (iAccess==CR_LOCK_WRITE_ONLY)			
			m_pData = (unsigned int*) glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
		else
		if (iAccess==CR_LOCK_READ_ONLY)
		{
			m_pData = (unsigned int*) glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_ONLY_ARB);
		}
		else // if (m_dwVNFlags& CR_LOCK_READ_WRITE) DEFAULT
		{
			m_pData = (unsigned int*) glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
		}		 		
	} 

}
void CIndexBuffer_GL::Unlock(void)
{
	if (m_dwIBFlags & IB_MEM_SYS)
	{
		m_iLockAccess = CR_UNLOCKED;
	}
	else // if (dwIBFlags & IB_MEM_GPU) // DEFAULT
	{
		m_iLockAccess = CR_UNLOCKED;

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_uiVBOID);	
		glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);

		m_pData = NULL;
	} 

}
	
	