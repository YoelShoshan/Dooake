#ifndef CRIMSON_SERIAL_COMMUNICATION_H
#define CRIMSON_SERIAL_COMMUNICATION_H

#pragma once

#include <String>

#include <windows.h>


class CSerialCommunication
{
public:
	CSerialCommunication();
	~CSerialCommunication();

	bool Init(int iComNum);

	const char* Read(int &iNumRead);



private:

	HANDLE m_PortHandle;

	COMSTAT m_ComStat;

	char m_CurrentIncomingBuffer[10000];

	std::string m_FrameBuffer;

};




#endif