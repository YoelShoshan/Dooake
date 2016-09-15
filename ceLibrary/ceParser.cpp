#include "stdafx.h"
#include "ceParser.h"

#include <windows.h>

#include "crtdbg.h"
#include "assert.h"

#include "Defines.h"

#include "math.h"


CParser::CParser(const char* pData,int iSize)
{
	m_pcData = pData;
	//m_pcData   = NULL;
	//m_pcTravel = NULL;

	//assert(_CrtCheckMemory());
	//m_pcData = new char[iSize];
	//memcpy(m_pcData,pData,sizeof(char)*iSize);

	m_pcTravel = m_pcData;

	m_iSize = iSize;
	//assert(_CrtCheckMemory());
}

CParser::~CParser()
{
	//SafeDeleteArray(m_pcData);

}

CParser::CParser()
{
	m_pcData=NULL;
	m_pcTravel=NULL;
}

void CParser::NextLine(void)
{

	//assert(_CrtCheckMemory());
	while (m_pcTravel[0]!=NULL)
	{
		if (IS_NL(m_pcTravel[0])) // if new line
		{
			m_pcTravel++;
			EatWhiteSpace();
			return;
		}

		m_pcTravel++;
	}

	EatWhiteSpace();
	//assert(_CrtCheckMemory());
}

void CParser::EatWhiteSpace(void)
{
	//while (m_pcTravel[0]==10 || m_pcTravel[0]==13 || m_pcTravel[0]==15)
	while (IS_WHITE_SPACE(m_pcTravel[0]))
	{
		m_pcTravel++;
		if (m_pcTravel[0]==NULL)
			return;
	}

}

void CParser::NextWord(void)
{
	if (m_pcTravel[0]==NULL)
			return;

	//assert(_CrtCheckMemory());
	// if we are not in a word, eat white space
	if (IS_WHITE_SPACE(m_pcTravel[0])) //if white space
		EatWhiteSpace();


	// if we are in a word, then advance until we are in a white space,
	while (!(IS_WHITE_SPACE(m_pcTravel[0])))    //if NOT white space
	{
		m_pcTravel++;
		if (m_pcTravel[0]==NULL)
			return;
	}

	// and then eat white space
	EatWhiteSpace();


	//assert(_CrtCheckMemory());
}


void CParser::NextWord_IC(void)
{
	if (m_pcTravel[0]==NULL)
			return;

	if (m_pcTravel[0]=='/')
	{
		while (m_pcTravel[0]=='/')
		{
			if (m_pcTravel[0]==NULL)
				return;

			while (m_pcTravel[0]=='/')
			{
				if (m_pcTravel[0]==NULL)
					return;

				NextLine();
				EatWhiteSpace();
			}
		}
		return;
	}

	//assert(_CrtCheckMemory());
	// if we are not in a word, eat white space
	if (IS_WHITE_SPACE(m_pcTravel[0])) //if white space
	{
		EatWhiteSpace();	
	}

	if (m_pcTravel[0]=='/')
	{
		while (m_pcTravel[0]=='/')
		{
			if (m_pcTravel[0]==NULL)
				return;

			while (m_pcTravel[0]=='/')
			{
				if (m_pcTravel[0]==NULL)
					return;

				NextLine();
				EatWhiteSpace();
			}
		}
		return;
	}

	// if we are in a word, then advance until we are in a white space,
	while (!(IS_WHITE_SPACE(m_pcTravel[0])))    //if NOT white space
	{
		m_pcTravel++;
		if (m_pcTravel[0]==NULL)
			return;
	}

	// and then eat white space
	EatWhiteSpace();

	while (m_pcTravel[0]=='/')
	{
		NextLine();
		EatWhiteSpace();
	}


	//assert(_CrtCheckMemory());
}

	

const char* CParser::GetLastLine(void)
{
	//assert(_CrtCheckMemory());
	for (int k=m_iSize-1;k>=0;k--)
	{
		if (m_pcData[k]==10)
			return &m_pcData[k+1];
	}

	return NULL;
//	assert(_CrtCheckMemory());
}



int CParser::GetCurrentLineLength(void)
{
	//assert(_CrtCheckMemory());
	const char* pTempTravel = m_pcTravel;

	int iCount = 0;

	while ( (!IS_NL(pTempTravel[0])) && pTempTravel[0]!=NULL)
	{
		pTempTravel++;
		iCount++;
	}

	return iCount;
//	assert(_CrtCheckMemory());
}

int CParser::GetCurrentWordLength(void)
{
	//assert(_CrtCheckMemory());
	const char* pTempTravel = m_pcTravel;

	int iCount = 0;

	while ( (!IS_WHITE_SPACE(pTempTravel[0])) && pTempTravel[0]!=NULL)
	{
		pTempTravel++;
		iCount++;
	}

	return iCount;
	//assert(_CrtCheckMemory());
}

int CParser::GetCurrentQuetuePhraseLength(void)
{
	//assert(_CrtCheckMemory());
	const char* pTempTravel = m_pcTravel;

	if (pTempTravel[0]!=34)
		return 0;

	int iCount = 0;

	pTempTravel++;
	iCount++;
	
	while (pTempTravel[0]!=34)
	{

		if (pTempTravel[0]==NULL)
			return 0;

		pTempTravel++;
		iCount++;
	}

	iCount++;

	return iCount;
	//assert(_CrtCheckMemory());

}

bool CParser::ReachedEnd(void)
{
//	assert(_CrtCheckMemory());
	if (m_pcTravel[0]==NULL)
		return true;

	return false;
//	assert(_CrtCheckMemory());
}

void CParser::RestartTraveler(void)
{
	//assert(_CrtCheckMemory());
	m_pcTravel = m_pcData;
//	assert(_CrtCheckMemory());
}

// assumes fillMe has enough allocated space !!
void CParser::FillCurrentLine(char* pchFillMe)
{
	//assert(_CrtCheckMemory());
	int iLength = GetCurrentLineLength();
	if (iLength==0)
	{
		pchFillMe[0]=NULL;
		return;
	}
	memcpy(pchFillMe,m_pcTravel,sizeof(char)*iLength);

	pchFillMe[iLength]=NULL;

	//assert(_CrtCheckMemory());

}

void CParser::FillCurrentWord(char* pchFillMe)
{
	//assert(_CrtCheckMemory());
	int iLength = GetCurrentWordLength();
	if (iLength==0)
	{
		pchFillMe[0]=NULL;
		return;
	}
	memcpy(pchFillMe,m_pcTravel,sizeof(char)*iLength);

	pchFillMe[iLength]=NULL;

	//assert(_CrtCheckMemory());
}

void CParser::FillCurrentQuetuePhrase(char* pchFillMe)
{
	//assert(_CrtCheckMemory());
	int iLength = GetCurrentQuetuePhraseLength();
	if (iLength==0)
	{
		pchFillMe[0]=NULL;
		return;
	}

	memcpy(pchFillMe,&m_pcTravel[1],sizeof(char)*iLength-2);

	pchFillMe[iLength-2]=NULL;

	//assert(_CrtCheckMemory());

}

DWORD CParser::Hex8StringToDWORD(char* pcHex)
{
	DWORD dwMult;
	DWORD dwIDNum=0;
	DWORD dwCharValue=0;   
	
	for (DWORD i=0;i<8;i++)
	{
		dwMult = (DWORD) powf(16,7-i);
		dwCharValue = (DWORD)GetCharHexValue(pcHex[i]);
		if (dwCharValue==0xFF)
			return 0x99999999;
		dwIDNum+= dwMult*dwCharValue;							
	}

	return dwIDNum;
}

__int64 CParser::Hex16StringToINT64(char* pcHex)
{
	__int64 i64Mult;
	__int64 i64IDNum=0;
	__int64 i64CharValue=0;   
	
	for (__int64 i=0;i<16;i++)
	{
		i64Mult = (__int64) powf(16,15-i);
		i64CharValue = (__int64)GetCharHexValue(pcHex[i]);
		if (i64CharValue==0xFF)
			return 0x9999999999999999;
		i64IDNum+= i64Mult*i64CharValue;							
	}

	return i64IDNum;
}

int CParser::GetCharHexValue(char c)
{	
	// if it's 0...9
	if (c>=48 && c<=58)
		return c-48;

	// A...Z
	if (c>=65 && c<=90)
		return c-65+10;

	// a...z
	if (c>=97 && c<=122)
		return c-97+10;
		

	// letter wasn't a part of the hexa alphabet
	// return special error value
	return 0xFF; 
}



