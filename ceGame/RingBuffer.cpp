#include "stdafx.h"
#include "RingBuffer.h"

#include "ce.h"

CRingBuffer::CRingBuffer()
{
	m_iWrite = 0;
	m_iSend  = 0;

	// debug
	m_iLongestMessageToBeSent = 0;

	m_bPrintDebugInfo = false;
}

CRingBuffer::~CRingBuffer()
{

}

void CRingBuffer::Push(const char* pData, unsigned int iLength)
{	
	if (!iLength)
		return;

	BREAKPOINT(iLength>=BUFFER_SIZE); //you may NOT push that much data!


	if (m_iWrite+iLength < BUFFER_SIZE)
	{
		memcpy(&m_Buffer[m_iWrite],pData,iLength*sizeof(unsigned char));
		m_iWrite+=iLength;
		return;
	}
	
	
	// if we're here then a split occured

	memmove(&m_Buffer[0],&m_Buffer[m_iSend],m_iWrite-m_iSend);

	m_iWrite-=m_iSend;
	m_iSend = 0;


	if (m_iWrite+iLength < BUFFER_SIZE)
	{
		memcpy(&m_Buffer[m_iWrite],pData,iLength*sizeof(unsigned char));
		m_iWrite+=iLength;
		return;
	}

	
	// we have a problem...
	BREAKPOINT(1);
}

bool CRingBuffer::GetCurrSend(char*& pData,int &iLength)
{
	if (m_iWrite==m_iSend) // first case - nothing to send
	{
		pData = NULL;
		iLength = 0;
		return false;
	}

	// debug debug  /////////////////////////////////////////
	char temp[400];
	if (m_iWrite - m_iSend > m_iLongestMessageToBeSent)
	{
		m_iLongestMessageToBeSent = m_iWrite - m_iSend;
		sprintf(temp,"Longest Message So Far: %u",m_iLongestMessageToBeSent);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_INFORMATIVE);
	}
	/////////////////////////////////////////////////////////


	if (m_iWrite>m_iSend) // second case - simple
	{
		pData   = &m_Buffer[m_iSend];
		iLength = m_iWrite-m_iSend;
 
		return false;
	}

	// we have a problem...
	
	BREAKPOINT(1);
	return true;
}

void CRingBuffer::InformDataSent(unsigned int iAmount)
{
	if (!iAmount)
		return;

	m_iSend+=iAmount;

	if (m_iSend==BUFFER_SIZE)
		m_iSend = 0;


}