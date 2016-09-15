#include "stdafx.h"
#include "SerialCommunication.h"
#include "Defines.h"

#include "Timer_IE.h"

#include <tchar.h>

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

CSerialCommunication::CSerialCommunication()
{
}

CSerialCommunication::~CSerialCommunication()
{
	CloseHandle(m_PortHandle);
}

bool CSerialCommunication::Init(int iComNum)
{
	char errorMessage[400];
	/*if (iComNum==4)
		return false;*/

	if (iComNum>9)
	{
		sprintf(errorMessage," Trying to open a port # bigger then 9");
		g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_ERROR);
		return false;
	}

	LPCTSTR const PortNames[] = 
    {
      _T("COM1:"),
      _T("COM2:"),
	  _T("COM3:"),
	  _T("COM4:"),
	  _T("COM5:"),
	  _T("COM6:"),
	  _T("COM7:"),
	  _T("COM8:"),
	  _T("COM9:")
    };


	m_PortHandle = CreateFile(PortNames[iComNum-1],GENERIC_READ,0,NULL,OPEN_EXISTING,0,0);

	if (m_PortHandle == INVALID_HANDLE_VALUE)
		return false;


	COMMPROP commprop;
	
	GetCommProperties(m_PortHandle,&commprop);


	DCB dcb;

	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);

	LPCTSTR const BuildString = _T("baud=19200 parity=N data=8 stop=1");

	if (!BuildCommDCB(BuildString, &dcb))    
		// Couldn't build the DCB. Usually a problem
		// with the communications specification string.
		return FALSE;
	else
		if (!SetCommState(m_PortHandle, &dcb))
			return false;
   
        
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	
	if (!SetCommTimeouts( m_PortHandle, &CommTimeOuts ))
			return false;

	return true;
}

const char* CSerialCommunication::Read(int &iNumRead)
{
	m_FrameBuffer.clear();
	int iTemp;

	DWORD dwErrorFlags,dwRet;

	ClearCommError( m_PortHandle, &dwErrorFlags, &m_ComStat );

	if (m_ComStat.cbInQue==0)
	{
		iNumRead=0;
		return m_FrameBuffer.c_str();
	}
		
    if (!ReadFile(m_PortHandle, m_CurrentIncomingBuffer, m_ComStat.cbInQue, &dwRet, NULL)) 
	{
      if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
	  {
		  // todo: consider what to do here
	  }
	     
	}

	m_CurrentIncomingBuffer[dwRet] = 0;

	m_FrameBuffer+=m_CurrentIncomingBuffer;

	iTemp = m_FrameBuffer.size();

	return m_FrameBuffer.c_str();
}