#ifndef CRIMSON_LOG_FILE_INTERFACE_H
#define CRIMSON_LOG_FILE_INTERFACE_H

#pragma once

#define LOG_MESSAGE_WARNING_LVL_0 1<<0
#define LOG_MESSAGE_WARNING_LVL_1 1<<1
#define LOG_MESSAGE_WARNING_LVL_2 1<<2
#define LOG_MESSAGE_WARNING_LVL_3 1<<3
#define LOG_MESSAGE_ERROR         1<<4
#define LOG_MESSAGE_INFORMATIVE   1<<5

#include <windows.h>

class ILogFile
{
public:
	ILogFile() {};
	virtual ~ILogFile() {};

	virtual void  SetErrorFlags(DWORD dwSet) = 0;
	virtual DWORD GetErrorFlags(void) = 0;

	// "translates" simple text to html (always adds <br> in the end)
	virtual void OutPutPlainText(const char* strOutput,char* strFontSize,char* strFontColor,bool bEndLine,DWORD dwType) = 0;

	virtual void OutPutPlainText(const char* strOutput,DWORD dwType) = 0;

};



#endif