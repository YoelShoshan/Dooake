#ifndef CRIMSON_TAG_LOGICS_H
#define CRIMSON_TAG_LOGICS_H

#pragma once

#include <map>
#include <string>

#include <Windows.h>

#include "SerialCommunication.h"




struct sTag
{
	__int64 i64TagID;
//	bool  bActive;
	float fTimeLastActive;
};

typedef std::map<__int64,sTag*>                     TAGS_MAP;
typedef std::map<__int64,sTag*>::iterator           TAGS_MAP_IT;


#define ALIVE_DELTA_MS 75.f
#define BUFFER_CLEAN_INTERVAL_MS 100


#define FOUND_MESSAGE_SIZE  34 //"0x0181   ISO ID=0xE007000012C01AA3"


#define NONE_MESSAGE_SIZE   12 // "0x2060 None"


class CTagLogics
{
public:
	CTagLogics();
	~CTagLogics();

	bool    Add(__int64 i64ID,sTag* pAddMe);
	sTag*   Get(__int64 i64ID);
	
	void    ResetTags(void);

	void    ParseDataBuffer(std::string& strDataBuffer);

	void    ParseLine(const char* Line);

	int     GetCharHexValue(char c);

	// 
	bool    IsTagActive(__int64 i64ID);

	int     InitSerialCommunication(void);

	void    PerfromReadParse(void);


	//int GetTest(void);

private:

	CSerialCommunication m_SerialCom;
	
	TAGS_MAP m_Tags;
	char m_CurrentLine[1000];

	std::string m_CurrentUnParsedBuffer;

	float m_fTimeSinceLastBufferClean;



};



#endif