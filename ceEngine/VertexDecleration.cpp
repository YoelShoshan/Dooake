#include "stdafx.h"
#include "VertexDecleration.h"
#include "Defines.h"
#include "LogFile.h"
#include <assert.h>


#include "ce.h"


CVertDecl::CVertDecl()
{
	m_uiStreamMapsNum = 0;
}

CVertDecl::~CVertDecl()
{
}





void CVertDecl::AddStreamMap(const tStreamMap& map)
{
	if (m_uiStreamMapsNum>=STREAM_MAX)
	{
		CR_ERROR(1,"Adding too many Stream Maps to a Vertex Decleration!");
		return;
	}
	
	m_StreamsMaps[map.dwStream].bActive          = true;
	m_StreamsMaps[map.dwStream].chElementType    = map.chElementType;
	m_StreamsMaps[map.dwStream].dwStream         = map.dwStream;
	
	m_uiStreamMapsNum++;

}

bool CVertDecl::IsStreamActive(DWORD dwStream) const
{
	return (m_StreamsMaps[dwStream].bActive);
}

tStreamMap& CVertDecl::GetStreamMap(DWORD dwStream)
{
	CR_ERROR(dwStream>=STREAM_MAX,"GetStreamMap() Failed - stream requested bigger then STREAM_MAX");
	CR_ERROR(!m_StreamsMaps[dwStream].bActive==CR_STREAM_NON_INIT,"GetStreamMapIndex() Failed - stream requested wasn't allocated");
	return m_StreamsMaps[dwStream];
}

const tStreamMap& CVertDecl::GetStreamMap(DWORD dwStream)const
{
	CR_ERROR(dwStream>=STREAM_MAX,"GetStreamMap() Failed - stream requested bigger then STREAM_MAX");
	CR_ERROR(!m_StreamsMaps[dwStream].bActive==CR_STREAM_NON_INIT,"GetStreamMapIndex() Failed - stream requested wasn't allocated");
	return m_StreamsMaps[dwStream];
}


unsigned int CVertDecl::GetStreamMapsNum(void)
{
	return m_uiStreamMapsNum;
}


