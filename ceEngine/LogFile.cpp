#include "stdafx.h"
#include "LogFile.h"
#include <fstream>

#include "Defines.h"
///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

#include "ce.h"

CLogFile::CLogFile()
{
	// default to show everything
	m_dwErrorFlags = LOG_MESSAGE_WARNING_LVL_0 | LOG_MESSAGE_WARNING_LVL_1 | LOG_MESSAGE_WARNING_LVL_2 |
		LOG_MESSAGE_WARNING_LVL_3 |  LOG_MESSAGE_ERROR | LOG_MESSAGE_INFORMATIVE;

	m_strFileName = "LOG.html";
	m_LogFile.open(m_strFileName.c_str());

	m_LogFile<<"<html><head><title>ZeDuS Engine Log File</title></head><body><font face=""Arial"" size=""5"" color=""#000099""><b><u>ZeDuS Engine Log File</u></b></font>";
	m_LogFile<<"<br>\n";

	OutPutPlainText("CLogFile::Logging process started (Engine v1.34).","4","000000",true,LOG_MESSAGE_INFORMATIVE);
	OutPutPlainText("*****************************************","4","000000",true,LOG_MESSAGE_INFORMATIVE);
	OutPutPlainText(" ","4","000000",true,LOG_MESSAGE_INFORMATIVE);


}

CLogFile::~CLogFile()
{
	m_LogFile<<"</body></html>";
	m_LogFile.close();
}

void CLogFile::OutPutPlainText(const char* strOutput,DWORD dwType)
{

	if (dwType & LOG_MESSAGE_INFORMATIVE)
		OutPutPlainText(strOutput,"4","000000",true,dwType);
	else if (dwType & LOG_MESSAGE_ERROR)
		OutPutPlainText(strOutput,"4","FF0000",true,dwType);
	else if (dwType & LOG_MESSAGE_WARNING_LVL_0)
		OutPutPlainText(strOutput,"4","770000",true,dwType);
	else if (dwType & LOG_MESSAGE_WARNING_LVL_1)
		OutPutPlainText(strOutput,"4","990000",true,dwType);
	else if (dwType & LOG_MESSAGE_WARNING_LVL_2)
		OutPutPlainText(strOutput,"4","BB0000",true,dwType);
	else if (dwType & LOG_MESSAGE_WARNING_LVL_3)
		OutPutPlainText(strOutput,"4","DD0000",true,dwType);
	else // default
		OutPutPlainText(strOutput,"4","000000",true,dwType);


	m_LogFile.flush();

	if (dwType & LOG_MESSAGE_ERROR)	
		BREAKPOINT_NO_LOG(1);


}

void CLogFile::OutPutPlainText(const char* strOutput,char* strFontSize,char* strFontColor,bool bEndLine,DWORD dwType)
{
	if (strOutput==NULL) 
		return;

	if (! (dwType & m_dwErrorFlags) )
		return;

	m_LogFile<<"<font face=""Arial"" size=""";
	m_LogFile<<strFontSize;
	m_LogFile<<""" color=""#";
	m_LogFile<<strFontColor;
	m_LogFile<<"""><b>";
	m_LogFile<<strOutput;
	m_LogFile<<"</b></font>";
	if (bEndLine==true)
		m_LogFile<<"<br>\n";

	m_LogFile.flush();

	if (dwType & LOG_MESSAGE_ERROR)	
	//if (dwType != LOG_MESSAGE_INFORMATIVE)

		BREAKPOINT_NO_LOG(1);
}


