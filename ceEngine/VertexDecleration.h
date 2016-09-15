#ifndef CRIMSON_VERTEX_DECLERATION_H
#define CRIMSON_VERTEX_DECLERATION_H

#pragma once
#include "Windows.h"

#include "d3d9.h"
#include "d3dx9.h"

#include "ce.h"

// for example: 
// dwStream=STREAM_TEXCOORD0
// dwDestination=DEST_TEXCOORD2
// dwElementType=CR_FLOAT2

// destination value is ignored when stream is different 
// (position will only be used for position etc.)
struct tStreamMap
{

	DWORD dwStream;
	byte  chElementType;
	bool  bActive;
	unsigned int   uiNum;

	tStreamMap(){ ZeroMemory(this,sizeof(tStreamMap));};
};

// used only for validation !!
struct tMaxVDelement
{
	tMaxVDelement():bActive(false) {};
	char chStreamDataType; // float3, byte4, etc.
	bool bActive;
};

struct tMaxVD
{
	tMaxVDelement elements[STREAM_MAX];
};


class CVertDecl
{
public:
	CVertDecl();
	~CVertDecl();
	
	unsigned int GetStreamMapsNum(void);

	void AddStreamMap(const tStreamMap& map);
	tStreamMap& GetStreamMap(DWORD dwStream);
	const tStreamMap& GetStreamMap(DWORD dwStream)const;

	virtual void Bind(void) const= 0;
	virtual void Allocate(void) = 0;

	bool IsStreamActive(DWORD dwStream) const;
	


protected:
	
	unsigned int m_uiStreamMapsNum;
	tStreamMap   m_StreamsMaps[STREAM_MAX];
};


#endif