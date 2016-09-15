#include "stdafx.h"
#include "cgShaderProgram.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include "RendererBackend.h"
extern CRendererBackend* g_pRendererBack;

void CCGShaderProgram::ParseParameters(void)
{
	SafeDelete(m_pVD);

	m_pVD = g_pRendererBack->GenerateVertexDecleration();

	CGparameter cur=cgGetFirstParameter(m_Program,CG_PROGRAM);
	do
	{
		CGtype type=cgGetParameterType(cur);
		if (type==CG_STRUCT)
		{
			CGparameter curStruct=cgGetFirstStructParameter(cur);
			do
			{
				if (cgGetParameterType(curStruct)==CG_STRUCT)
				{
					g_pLogFile->OutPutPlainText("structs within structs aren't supported! (yet ;) )",LOG_MESSAGE_ERROR);				
				}

				ParseParameter(curStruct);
			} while (curStruct=cgGetNextParameter(curStruct));
		}
		else
		{
			ParseParameter(cur);
		}
	}
	while (cur=cgGetNextParameter(cur));

	m_pVD->Allocate();

}

void CCGShaderProgram::ParseParameter(CGparameter param)
{

	tStreamMap sMap;

	const char* pName=cgGetParameterName(param);
	const char* pSemantic=cgGetParameterSemantic(param);

	CGenum dir=cgGetParameterDirection(param);
	if (dir!=CG_IN) // if it's not incoming, we don't care about it
		return;

	CGenum var=cgGetParameterVariability(param);

	CGtype type =  cgGetParameterType(param);
		
	const char* pType = cgGetTypeString(type);

	if (var==CG_UNIFORM)
	{
		//TODO:... use this the create a fast lookup of uniform variables
	} else
	if (var==CG_VARYING)
	{
		
		if ( (strcmp(pSemantic,"POSITION")==0) ||  (strcmp(pSemantic,"POSITION0")==0))
		//if (strstr(pSemantic,"POSITION")!=0)
		{
			//m_MaxVD.elements[STREAM_POSITION].bActive = true;
			sMap.dwStream      = STREAM_POSITION;
			sMap.chElementType = CR_FLOAT3;

			/*if (strcmp(pType,"float4")!=0)
				g_pLogFile->OutPutPlainText("position must be float4!",LOG_MESSAGE_ERROR);*/
				
			//m_MaxVD.elements[STREAM_POSITION].chStreamDataType = CR_FLOAT3;									
			sMap.chElementType = CR_FLOAT3;

			m_pVD->AddStreamMap(sMap);
		}
		else if ( (strcmp(pSemantic,"COLOR0")==0) || (strcmp(pSemantic,"COLOR")==0)) // check for color
		{
			/*m_MaxVD.elements[STREAM_COLOR].bActive = true;
			m_MaxVD.elements[STREAM_COLOR].chStreamDataType = CR_BYTE4;*/
			sMap.dwStream      = STREAM_COLOR;
			sMap.chElementType = CR_BYTE4;	
			m_pVD->AddStreamMap(sMap);
		}
		else if (strcmp(pSemantic,"NORMAL")==0) // check for color
		{
			/*if (strcmp(pType,"float3")!=0)
				g_pLogFile->OutPutPlainText("normal must be float3!",LOG_MESSAGE_ERROR);*/
			/*m_MaxVD.elements[STREAM_NORMAL].bActive = true;
			m_MaxVD.elements[STREAM_NORMAL].chStreamDataType = CR_FLOAT3;*/
			sMap.dwStream      = STREAM_NORMAL;
			sMap.chElementType = CR_FLOAT3;	
			m_pVD->AddStreamMap(sMap);
		}
		else if (!strnicmp(pSemantic,"TEXCOORD",8))
		{
			//int iType=VERTEXDECL_FLOAT1+(-1)+cgD3D9TypeToSize(cgGetParameterType(param));
			int iNum=atoi(pSemantic+8);

			//m_MaxVD.elements[STREAM_TEXCOORD0+iNum].bActive = true;

			sMap.dwStream      = STREAM_TEXCOORD0+iNum;


			if (strcmp(pType,"float")==0)
				//m_MaxVD.elements[STREAM_TEXCOORD0+iNum].chStreamDataType = CR_FLOAT1;
				sMap.chElementType = CR_FLOAT1;
			else if (strcmp(pType,"float2")==0)
				//m_MaxVD.elements[STREAM_TEXCOORD0+iNum].chStreamDataType = CR_FLOAT2;
				sMap.chElementType = CR_FLOAT2;
			else if (strcmp(pType,"float3")==0)
				//m_MaxVD.elements[STREAM_TEXCOORD0+iNum].chStreamDataType = CR_FLOAT3;
				sMap.chElementType = CR_FLOAT3;
			else if (strcmp(pType,"float4")==0)
				//m_MaxVD.elements[STREAM_TEXCOORD0+iNum].chStreamDataType = CR_FLOAT4;
				sMap.chElementType = CR_FLOAT4;
			else
			{
				char message[400];
				sprintf_s(message,400,"Urecognized parameter type for texcoord: %s !",pType);
				g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);

			}
			m_pVD->AddStreamMap(sMap);

		}
		else
		{
			char temp[1024];
			sprintf(temp,"CDXCGVertexShader::ParseParam '%s' contains an unknown varying semantic '%s : %s'",GetName(),pName,pSemantic);
			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);				
			//g_pLogFile->Warning("CDXCGVertexShader::ParseParam '%s' contains an unknown varying semantic '%s : %s'",m_chName,pName,pSemantic);
			BREAKPOINT(1);

		}
	}
}