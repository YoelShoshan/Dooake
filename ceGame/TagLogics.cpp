#include "stdafx.h"
#include "TagLogics.h"

#include <windows.h>

#include "ceParser.h"

#include <math.h>

#include "Defines.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;

//#include "crtdbg.h"
#include "assert.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "ce.h"
#include "ceLibrary.h"


CTagLogics::CTagLogics()
{
	m_fTimeSinceLastBufferClean = 0.f;


/*

	//////////////////////////////////////
	// add tags

	//////////////////////////////////////
	sTag* pTag = new sTag();

//	pTag->bActive = false;
	pTag->i64TagID    = 0x009ac85c;

	Add(pTag->dwID,pTag);
	//////////////////////////////////////
	pTag = new sTag();

//	pTag->bActive = false;
	pTag->dwID    = 0x00e226fc;

	Add(pTag->dwID,pTag);
	//////////////////////////////////////
	pTag = new sTag();

//	pTag->bActive = false;
	pTag->dwID    = 0x00000003;

	Add(pTag->dwID,pTag);
	//////////////////////////////////////
	pTag = new sTag();

//	pTag->bActive = false;
	pTag->dwID    = 0x00000004;

	Add(pTag->dwID,pTag);
	//////////////////////////////////////

	ResetTags();

	*/

}

CTagLogics::~CTagLogics()
{

}

int CTagLogics::InitSerialCommunication(void)
{
	int iPort;

	for (int i=9;i>0;i--)
	{
		if (m_SerialCom.Init(i))
		{
			return i;
		}
	}
	return -1;

}

void CTagLogics::ResetTags(void)
{
	TAGS_MAP_IT it;
	it = m_Tags.begin();

	while (it!=m_Tags.end())
	{
		it->second->fTimeLastActive=0.f;
		it++;
	}
}

void CTagLogics::ParseDataBuffer(std::string& strDataBuffer)
{	
	////////////////////////////////////
	// debug - check max size of 

	static int max = 0;

	if (strDataBuffer.size()>max)
		max = strDataBuffer.size();


	if (strDataBuffer.empty())
		return;

	CParser pars(strDataBuffer.c_str(),m_CurrentUnParsedBuffer.size()+1);

	pars.NextLine();

	int iLineSize;

	while (!pars.ReachedEnd())
	{
		// and a found-a-tag-message is 20 letters long

		iLineSize = pars.GetCurrentLineLength();

		pars.FillCurrentLine(m_CurrentLine);

		if (iLineSize >= FOUND_MESSAGE_SIZE) // a found-a-tag-message
			ParseLine(m_CurrentLine);
		else
		{
			if (iLineSize == NONE_MESSAGE_SIZE) // also not a None-message
			{
			}
			
		}

		pars.NextLine();
	}

	// ASSUMES this is done per frame !!!

	m_fTimeSinceLastBufferClean+= g_pTimer->GetDeltaTime();

	if (m_fTimeSinceLastBufferClean>=BUFFER_CLEAN_INTERVAL_MS)
	{
		m_fTimeSinceLastBufferClean-=BUFFER_CLEAN_INTERVAL_MS;

		if (m_fTimeSinceLastBufferClean>=BUFFER_CLEAN_INTERVAL_MS)
			m_fTimeSinceLastBufferClean=0.f;

		const char* pLastLine = pars.GetLastLine();
		if (!pLastLine) // nothing to delete
			return;

		int k=0;

		const char* pTravel = pLastLine;
		while (pTravel[0]!=NULL && pTravel[0]!=10 && pTravel[0]!=13)
		{
			pTravel++;
			k++;
		}

		if (k>= FOUND_MESSAGE_SIZE)
		{
			strDataBuffer.clear();
		}

		std::string strLastLine(pLastLine);
		strDataBuffer.clear();
		strDataBuffer += strLastLine;
	}

}

int CTagLogics::GetCharHexValue(char c)
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

void CTagLogics::ParseLine(const char* Line)
{
	char errorMessage[400];

	assert(_CrtCheckMemory());

	// "0x027D None\n"
	if (Line[7]=='N')
	{
		// RESTORE RESTORE RESTORE
		return;
	}

	//0x0290 ID=0x009AC85C
	if (Line[13]=='I')
	{		
		__int64 i64Mult;
		__int64 i64IDNum=0;
		__int64 i64CharValue=0;   

		const char* pHexID = &Line[18];
		for (__int64 i=0;i<16;i++)
		{
			i64Mult = (__int64) powf(16,15-i);
			i64CharValue = (__int64)GetCharHexValue(pHexID[i]);

			if (i64CharValue==0xFF)
				return;
			i64IDNum+= i64Mult*i64CharValue;							
		}

		sTag* pTag = Get(i64IDNum);

		if(pTag)
		{
//			pTag->bActive=true;
			pTag->fTimeLastActive = g_pTimer->GetTime();
		}
		else
		{
			sprintf(errorMessage,"Didn't find tag num: %16lX",i64IDNum);
			g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_ERROR);
			BREAKPOINT(1);
		}
			
	}
			

	//assert(_CrtCheckMemory());
	

}

bool CTagLogics::Add(__int64 i64ID,sTag* pAddMe)
{

	pAddMe->fTimeLastActive = 0.f;

	TAGS_MAP_IT it;

	it = m_Tags.find(i64ID);

	if (it!=m_Tags.end()) // it's already in the map!
	{
		g_pLogFile->OutPutPlainText("CTagLogics::Add::Tag was already in the tags map!",LOG_MESSAGE_ERROR);
		BREAKPOINT(1);
		return false;
	}


	m_Tags[i64ID] = pAddMe;

	return true;
}

sTag* CTagLogics::Get(__int64 i64ID)
{
	TAGS_MAP_IT it;
	
	it = m_Tags.find(i64ID);

	if (it!=m_Tags.end()) // it's already in the map!
		return it->second;
	else // it wasn't found in the map
	{
		//g_pLogFile->OutPutPlainText("CShaderUniforms::Get::uniform doesn't exist!",LOG_MESSAGE_ERROR);
		return NULL;
	}
}

bool CTagLogics::IsTagActive(__int64 i64ID)
{
	TAGS_MAP_IT it;
	
	it = m_Tags.find(i64ID);

	if (it==m_Tags.end())  // it's not in the map!
		return false; 
	else // it IS in the map
	{		
		if ( (g_pTimer->GetTime() - it->second->fTimeLastActive) < ALIVE_DELTA_MS)
			return true;
	}

	return false;

}

void CTagLogics::PerfromReadParse(void)
{
	const char* FrameBuffer=NULL;
	int iLastReadNum=0;
	m_CurrentUnParsedBuffer += m_SerialCom.Read(iLastReadNum);	
	ParseDataBuffer(m_CurrentUnParsedBuffer);

		

}

/*int CTagLogics::GetTest(void)
{
	if (1==0)
		return 15;

}*/