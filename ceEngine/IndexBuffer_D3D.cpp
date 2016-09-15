#include "stdafx.h"
#include "IndexBuffer_D3D.H"

#include "Defines.h"

#include "LogFile.h"

extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;
#include "ce.h"
#include <assert.h>

CIndexBuffer_D3D::CIndexBuffer_D3D()
{
	m_pData = NULL;
	m_iLength=0;
	m_iLockAccess = CR_UNLOCKED;
	m_dwIBFlags = 0;
	m_D3Dib=NULL;
}
CIndexBuffer_D3D::~CIndexBuffer_D3D()
{
	SAFE_RELEASE(m_D3Dib);	
}

void* CIndexBuffer_D3D::GetData(void)
{
	CR_ERROR(m_iLength==0,"Trying to get data from a 0 length Index Buffer!");
	CR_ERROR(m_iLockAccess==CR_UNLOCKED,"Trying to get data from an unlocked Index Buffer!");
	CR_ERROR(m_pData==NULL,"Trying to get a non mapped system Index Buffer!");
	
	return m_pData;
}

int CIndexBuffer_D3D::GetLength(void) const
{
	return m_iLength;
}
const void* CIndexBuffer_D3D::Bind(void)const
{
	HRESULT hr;
		

	CR_ERROR(!m_D3Dib,"Trying to bind a non inited Index Buffer!");
	hr = g_pDirect3DDevice->SetIndices(m_D3Dib);
	assert(SUCCEEDED(hr));

	return NULL;
}
		

// Notice: uiOffset and uiSize are in ELEMENTS not BYTES !!
void CIndexBuffer_D3D::Lock(int iAccess,UINT uiOffset, UINT uiSize)
{
	HRESULT hr;

	m_iLockAccess = iAccess;

	//m_pData = (unsigned int*) glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	switch(iAccess)
	{
	case CR_LOCK_WRITE_ONLY:
		hr = m_D3Dib->Lock(uiOffset*sizeof(UINT),uiSize*sizeof(UINT),&m_pData,0);
		break;

	case CR_LOCK_WRITE_ONLY_NO_OVERWRITE:
		hr = m_D3Dib->Lock(uiOffset*sizeof(UINT),uiSize*sizeof(UINT),&m_pData,D3DLOCK_NOOVERWRITE);
		break;

	case CR_LOCK_WRITE_ONLY_DISCARD:
		hr = m_D3Dib->Lock(uiOffset*sizeof(UINT),uiSize*sizeof(UINT),&m_pData,D3DLOCK_DISCARD);
		break;

	case CR_LOCK_READ_ONLY:
		hr = m_D3Dib->Lock(uiOffset*sizeof(UINT),uiSize*sizeof(UINT),&m_pData,D3DLOCK_READONLY);		
		break;

	default:
		hr = m_D3Dib->Lock(uiOffset*sizeof(UINT),uiSize*sizeof(UINT),&m_pData,0);			
		break;


	}


	assert(SUCCEEDED(hr));
}

void CIndexBuffer_D3D::Unlock(void)
{
	HRESULT hr;

	if (m_D3Dib)
	{
		hr = m_D3Dib->Unlock();
		assert(SUCCEEDED(hr));
	}

	m_iLockAccess = CR_UNLOCKED;
	m_pData = NULL;
}
	
// Allocate(30,IB_STATIC)
void CIndexBuffer_D3D::Allocate(int iLength,DWORD dwIBFlags)
{
	HRESULT hr;

	// ignoring sys/gpu mem requests

	CR_ERROR(iLength==0,"Trying to load a 0 length data into an Index Buffer!");

	m_dwIBFlags = dwIBFlags;

	m_iLength = iLength;


	if (dwIBFlags & IB_MEM_SYS)
	{
		// Only for debugging !!!
		assert(0);

		if (dwIBFlags&IB_DYNAMIC)
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_SYSTEMMEM, 
					 	&m_D3Dib, NULL );
		else // if (dwIBFlags&IB_STATIC) == default is static
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_SYSTEMMEM, 
				 	&m_D3Dib, NULL );
	}
	else // if (dwIBFlags & IB_MEM_GPU)
	{
		/*if (dwIBFlags&IB_DYNAMIC)
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_DEFAULT, 
					 	&m_D3Dib, NULL );
		else // if (dwIBFlags&IB_STATIC) == default is static
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, 
				 	&m_D3Dib, NULL );*/

		if (dwIBFlags&IB_DYNAMIC)
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_DEFAULT, 
				&m_D3Dib, NULL );
		else // if (dwIBFlags&IB_STATIC) == default is static
			hr = g_pDirect3DDevice->CreateIndexBuffer( iLength *sizeof(unsigned int),
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX32,D3DPOOL_DEFAULT , 
				&m_D3Dib, NULL );
	}


	assert(SUCCEEDED(hr));
}