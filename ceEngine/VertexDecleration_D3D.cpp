#include "stdafx.h"
#include "VertexDecleration_D3D.h"

extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;

#include "LogFile.h"

#include "d3d9.h"
#include "d3dx9.h"

#include "assert.h"
//#include "crtdbg.h"

#include "ce.h"

CVertDecl_D3D::CVertDecl_D3D()
{

}


CVertDecl_D3D::~CVertDecl_D3D()
{

}


void CVertDecl_D3D::Bind(void) const
{
	CR_ERROR(!m_pVertexDeclaration,"Trying to bind a null Vertex Decleration!");
	g_pDirect3DDevice->SetVertexDeclaration(m_pVertexDeclaration);
}


void CVertDecl_D3D::Allocate(void)
{
	D3DVERTEXELEMENT9* vertDecl= new D3DVERTEXELEMENT9[m_uiStreamMapsNum+1];

	BYTE chUsage,chUsageIndex;

	

	unsigned int iStreamMapsProcessed=0;


	//assert(_CrtCheckMemory());

	//while (iStreamMapsProcessed<m_uiStreamMapsNum)
	for (int i=0;i<STREAM_MAX && iStreamMapsProcessed<m_uiStreamMapsNum;i++)
	{
		//assert(_CrtCheckMemory());
		

		if (!m_StreamsMaps[i].bActive)
			continue;

		//assert(_CrtCheckMemory());

		m_StreamsMaps[i].uiNum = iStreamMapsProcessed;

		vertDecl[iStreamMapsProcessed].Stream = iStreamMapsProcessed;
		vertDecl[iStreamMapsProcessed].Offset = 0;

		//assert(_CrtCheckMemory());
				
		switch (m_StreamsMaps[i].chElementType)
		{
		case CR_FLOAT1:
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_FLOAT1;
			break;
		case CR_FLOAT2:
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_FLOAT2;
			break;
		case CR_FLOAT3:
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_FLOAT3;
			break;
		case CR_FLOAT4:
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_FLOAT4;
			break;
		case CR_BYTE4:
			//vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_D3DCOLOR;
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_UBYTE4N;
			break;

		default:
			vertDecl[iStreamMapsProcessed].Type = D3DDECLTYPE_FLOAT3;
			break;
		}

		//assert(_CrtCheckMemory());

		vertDecl[iStreamMapsProcessed].Method = D3DDECLMETHOD_DEFAULT;

		switch (m_StreamsMaps[i].dwStream)
		{
		case STREAM_POSITION:
			chUsage=D3DDECLUSAGE_POSITION;
			chUsageIndex=0;
		break;

		case STREAM_NORMAL:
			chUsage=D3DDECLUSAGE_NORMAL;
			chUsageIndex=0;
		break;

		case STREAM_COLOR:
			chUsage=D3DDECLUSAGE_COLOR;
			chUsageIndex=0;
		break;

		case STREAM_TEXCOORD0:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=0;
		break;

		case STREAM_TEXCOORD1:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=1;
		break;

		case STREAM_TEXCOORD2:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=2;
		break;

		case STREAM_TEXCOORD3:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=3;
		break;

		case STREAM_TEXCOORD4:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=4;
		break;

		case STREAM_TEXCOORD5:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=5;
		break;

		case STREAM_TEXCOORD6:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=6;
		break;

		case STREAM_TEXCOORD7:
			chUsage=D3DDECLUSAGE_TEXCOORD;
			chUsageIndex=7;
		break;
		};

		//assert(_CrtCheckMemory());
		vertDecl[iStreamMapsProcessed].Usage      = chUsage;
		vertDecl[iStreamMapsProcessed].UsageIndex = chUsageIndex;

		//assert(_CrtCheckMemory());


		iStreamMapsProcessed++;
	}


	//assert(_CrtCheckMemory());
	// add the D3DDECL_END() --> {0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}
	


	vertDecl[m_uiStreamMapsNum].Stream     = 0xFF;
	vertDecl[m_uiStreamMapsNum].Offset     = 0;
	vertDecl[m_uiStreamMapsNum].Type       = D3DDECLTYPE_UNUSED;
	vertDecl[m_uiStreamMapsNum].Method     = D3DDECLMETHOD_DEFAULT;
	vertDecl[m_uiStreamMapsNum].Usage      = 0;
	vertDecl[m_uiStreamMapsNum].UsageIndex = 0;
	

	//assert(_CrtCheckMemory());
	
	
	g_pDirect3DDevice->CreateVertexDeclaration(vertDecl, &m_pVertexDeclaration);
	//g_pDirect3DDevice->SetVertexDeclaration(m_pVertexDeclaration);

	SafeDeleteArray(vertDecl);

	// should i delete vertDecl now ????

}