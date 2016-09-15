#include "stdafx.h"
#include "Uniform.h"
#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "ce.h"

/*#define CR_FLOAT1     0
#define CR_FLOAT2     1
#define CR_FLOAT3     2
#define CR_FLOAT4     3
#define CR_BYTE4      4
#define CR_FLOAT16X16 5*/

tUniform::tUniform(DWORD dwType)
{
	if (dwType==CR_FLOAT1)
		m_pfValue = new float[1];
	else
	if (dwType==CR_FLOAT2)
		m_pfValue = new float[2];
	else
	if (dwType==CR_FLOAT3)
		m_pfValue = new float[3];
	else
	if (dwType==CR_FLOAT4)
		m_pfValue = new float[4];
	else
	if (dwType==CR_FLOAT4X4)
		m_pfValue = new float[16];
	else
		g_pLogFile->OutPutPlainText("tUniform::tUniform::AutoUniform Type isn't recognized",LOG_MESSAGE_ERROR);		

	m_dwType = dwType;
	
}