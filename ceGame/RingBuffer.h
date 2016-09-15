#ifndef CRIMSON_RING_BUFFER_H
#define CRIMSON_RING_BUFFER_H

#pragma once

#define BUFFER_SIZE 0xFFFA
//#define BUFFER_SIZE 0xFFFF
//#define BUFFER_SIZE 100
//#define BUFFER_SIZE 0xFFFFF


//#define BUFFER_SIZE 10

// TODO: test this more heavily! especially the case where the message is physically split between end part and first part.
class CRingBuffer
{
public:
	CRingBuffer();
	~CRingBuffer();

	void Push(const char* pData, unsigned int iLength);

	// return true if another chunk should be sent
	// (due to chunk being split between end and start)
	bool GetCurrSend(char*& pData,int &iLength);

	void InformDataSent(unsigned int iAmount);

	// debug
	void SetPrintDebugInfo()  { m_bPrintDebugInfo = true;};

private:
	char m_Buffer[BUFFER_SIZE];
	

	int m_iWrite;
	int m_iSend;

	// debug
	int m_iLongestMessageToBeSent;
	bool m_bPrintDebugInfo;
};




#endif