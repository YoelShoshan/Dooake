#ifndef CRIMSON_LOGFILE_H
#define CRIMSON_LOGFILE_H

#pragma once

#include <fstream>

#include <string>
#include <windows.h>



#include "LogFile_I.h"

class CLogFile : public ILogFile
{
public:
	CLogFile();
	~CLogFile();

	void  SetErrorFlags(DWORD dwSet) { m_dwErrorFlags = dwSet; } ;
	DWORD GetErrorFlags(void) { return m_dwErrorFlags;};

	// "translates" simple text to html (always adds <br> in the end)
	void OutPutPlainText(const char* strOutput,char* strFontSize,char* strFontColor,bool bEndLine,DWORD dwType);

	void OutPutPlainText(const char* strOutput,DWORD dwType);

private:
	std::ofstream m_LogFile;
	std::string   m_strFileName;
	DWORD m_dwErrorFlags;
	char CharBuffer[400];
};


#endif // YOEL_LOGFILE_H