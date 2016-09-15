#include "stdafx.h"
#include "Compress_Dummy.h"

#include "ce.h"

CCompressDummy::CCompressDummy()
{
	m_iBufferSize = 3000;
	m_pBuffer = new char[m_iBufferSize];

};

CCompressDummy::~CCompressDummy()
{
	SafeDeleteArray(m_pBuffer);


};

tCompressedData CCompressDummy::Compress(const char* pCompressMe, int iSize)
{
	if (m_iBufferSize<iSize)
	{
		SafeDeleteArray(m_pBuffer);
		m_iBufferSize = iSize;
		m_pBuffer = new char[m_iBufferSize];		
	}

	
	memcpy(m_pBuffer,pCompressMe,iSize);

	tCompressedData ans;
	ans.iSize = iSize;
	ans.pData = m_pBuffer;

	return ans;
}