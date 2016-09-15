#ifndef CRIMSON_PARSER_H
#define CRIMSON_PARSER_H

#pragma once

#include "windows.h"

////////////////////////
// white space is:

#define IS_WHITE_SPACE(x) (x==32 || x==10 || x==11 || x==13 || x==9)

#define IS_WHITE_SPACE_AND_NOT_NL(x) (x==32 || x==11 || x==13 || x==9)

#define IS_NL(x) (x==10 || x==13)

// 32 - space
// 10 - new line
// 11 - vertical tab
// 13 - carriage return



class CParser
{
public:
	// include in the size the ending NULL !
	CParser(const char* pData,int iSize);	
	~CParser();

	void NextLine(void);

	void NextWord(void);

	// ignore comments
	void NextWord_IC(void);

	void EatWhiteSpace(void);

	int  GetCurrentLineLength(void);

	int  GetCurrentWordLength(void);

	int GetCurrentQuetuePhraseLength(void);

	bool ReachedEnd(void);

	void RestartTraveler(void);

	// assumes fillMe has enough allocated space !!
	void FillCurrentLine(char* pchFillMe);

	void FillCurrentWord(char* pchFillMe);

	void FillCurrentQuetuePhrase(char* pchFillMe);

	const char* GetLastLine(void);

	static int   GetCharHexValue(char c);
	static DWORD Hex8StringToDWORD(char* pcHex);
	static __int64 Hex16StringToINT64(char* pcHex);

	const char* GetTravel(void) { return m_pcTravel;};

private:
	CParser();

private:
	int   m_iSize;
	const char* m_pcData;
	const char* m_pcTravel;

};


#endif
