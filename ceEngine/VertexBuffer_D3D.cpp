#include "stdafx.h"
#include "VertexBuffer_D3D.h"
#include "LogFile.h"
#include "Defines.h"
#include "Profiler.h"

#include "d3d9.h"
#include "d3dx9.h"
extern LPDIRECT3DDEVICE9			g_pDirect3DDevice;

#include "ce.h"
#include <assert.h>

CVertexBuffer_D3D::CVertexBuffer_D3D()
{
	m_uiLength=0;	
	m_iLockAccess = CR_UNLOCKED;
	m_D3Dvb=NULL;
	m_dwVBFlags=0;
	m_pMergedData=NULL;
}
CVertexBuffer_D3D::~CVertexBuffer_D3D()
{	
	//SAFE_RELEASE(m_D3Dvb);
}

UINT CVertexBuffer_D3D::GetLength(void) const
{
	return m_uiLength;
}

// AddStream( STREAM_POSITION, 3, GL_FLOAT)
void CVertexBuffer_D3D::AddStream(DWORD dwStreamType,DWORD dwElementType)
{
	CR_ERROR((dwStreamType >= STREAM_MAX),"Trying to allocate VB of size 0!"); 
	m_Streams[dwStreamType].m_bUsed       = true;

	m_Streams[dwStreamType].crType = dwElementType;

	switch(dwElementType)
	{
	case CR_FLOAT1:
		m_Streams[dwStreamType].uiElementsNum = 1;
		m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_FLOAT1;
	break;
	case CR_FLOAT2:
		m_Streams[dwStreamType].uiElementsNum = 2;
		m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_FLOAT1;
	break;
	case CR_FLOAT3:
		m_Streams[dwStreamType].uiElementsNum = 3;
		m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_FLOAT1;
	break;
	case CR_FLOAT4:
		m_Streams[dwStreamType].uiElementsNum = 4;
		m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_FLOAT1;
	break;
	case CR_BYTE4:
		m_Streams[dwStreamType].uiElementsNum = 1;
		//m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_D3DCOLOR;
		m_Streams[dwStreamType].dwElementType = D3DDECLTYPE_UBYTE4N;
	break;
	}
}

UINT CVertexBuffer_D3D::GetElementTypeSizeBytes(DWORD dwElementType) const
{
	switch(dwElementType)
	{
	case D3DDECLTYPE_FLOAT1:
		return sizeof(float);
		break;

	//case D3DDECLTYPE_D3DCOLOR:
	case D3DDECLTYPE_UBYTE4N:
		return sizeof(DWORD);
		break;

	// i need to support other types as well...
	}

	CR_ERROR(1,"an unkown element type used inside a VB"); // not a type we know...

	return sizeof(float); // defeault
}
	

// AllocateStreams(100, VB_MEM_GPU | VB_MERGED , GL_STATIC_DRAW_ARB)
void CVertexBuffer_D3D::AllocateStreams(int iLength,DWORD dwVBFlags,int iUsageType)
{
	m_iUsageType = iUsageType;

	HRESULT hr;
	// currently ignoring single/merged request, and only using single...


	m_uiLength = iLength;
	CR_ERROR(m_uiLength==0,"Trying to allocate VB of size 0!");

	m_dwVBFlags = dwVBFlags;

	int iOffsetBytes=0;
	int iTotalSizeBytes=0;


	for (int i=0;i<STREAM_MAX;i++)
	{	
		if (!m_Streams[i].m_bUsed)
			continue;
		m_Streams[i].iOffset = iOffsetBytes;
		m_Streams[i].dwElementSizeBytes = GetElementTypeSizeBytes(m_Streams[i].dwElementType);
		iOffsetBytes+= m_uiLength * m_Streams[i].dwElementSizeBytes * m_Streams[i].uiElementsNum;
	}



	iTotalSizeBytes = iOffsetBytes; // just for clarity...

	DWORD dwUsage;
	D3DPOOL d3dpool;

	if (m_dwVBFlags&VB_MEM_SYS)
	{
		// Only for debugging !!!
		assert(0);
		d3dpool = D3DPOOL_SYSTEMMEM;
	}
	else  //if (m_dwVBFlags&CR_STREAM_DYNAMIC) 
	{
		d3dpool = D3DPOOL_DEFAULT;
	}
	/*else
		d3dpool = D3DPOOL_MANAGED;*/

	if (iUsageType==CR_STREAM_DYNAMIC)
	{
		d3dpool = D3DPOOL_DEFAULT;

		dwUsage = D3DUSAGE_DYNAMIC;
	}
	else //if (iUsageType==CR_STREAM_STATIC)  DEFAULT
		dwUsage = D3DUSAGE_WRITEONLY;

	if (D3DPOOL_DEFAULT == d3dpool)
		dwUsage |= D3DUSAGE_WRITEONLY;


	hr = g_pDirect3DDevice->CreateVertexBuffer( iTotalSizeBytes ,dwUsage,0,d3dpool,&m_D3Dvb,NULL);		
	assert(SUCCEEDED(hr));
		
}
	

void* CVertexBuffer_D3D::GetStream(DWORD dwStreamType) const
{
	CR_ERROR(m_iLockAccess == CR_UNLOCKED,"Trying to get a non locked vb stream!");
	
	CR_ERROR(m_pMergedData==NULL,"Trying to get a non mapped stream!");
	return (char*) m_pMergedData + m_Streams[dwStreamType].iOffset;	
}

void* CVertexBuffer_D3D::StreamCopySubData(const char* pData, DWORD dwStreamType,int iLength, int iOffset)
{
	// dummy dummy dummy dummy dummy dummy 
	return NULL;
}

void CVertexBuffer_D3D::Lock(int iAccess)
{

	HRESULT hr;

	m_iLockAccess = iAccess;
	
	if (!m_D3Dvb)
	{
		CR_ERROR(1,"Trying to Lock an empty VB!");
		return;
	}

	//D3DLOCK_READONLY
	if (iAccess== CR_LOCK_READ_ONLY)
		hr = m_D3Dvb->Lock(0,0,&m_pMergedData,D3DLOCK_READONLY);
	else if (iAccess== CR_LOCK_WRITE_ONLY)
	{
		if (m_iUsageType==CR_STREAM_DYNAMIC && (m_dwVBFlags & CR_STREAM_DISCARD))
			hr = m_D3Dvb->Lock(0,0,&m_pMergedData,D3DLOCK_DISCARD);  
		else
			hr = m_D3Dvb->Lock(0,0,&m_pMergedData,0);  // is there something suitable in D3D ???
	}
		
	else // if (iAccess== CR_LOCK_READ_WRITE) DEFAULT
		hr = m_D3Dvb->Lock(0,0,&m_pMergedData,0);

	assert(SUCCEEDED(hr));
}

void CVertexBuffer_D3D::Unlock(void)
{
	HRESULT hr;


	CR_ERROR(!m_D3Dvb,"Trying to Unlock an empty VB!");

	if (m_D3Dvb)
	{
		hr = m_D3Dvb->Unlock();
		assert(SUCCEEDED(hr));
	}

	m_iLockAccess = CR_UNLOCKED;
}

// binds all streams that are present in the given vertdecl
void CVertexBuffer_D3D::BindStreams(const CVertDecl* pVD) const
{
	for (int i=0;i<STREAM_MAX;i++)
		if (pVD->IsStreamActive(i))
		{
			//BREAKPOINT(i==1);
			BindStream(i,pVD);
		}
}


char CVertexBuffer_D3D::GetStreamCRtype(DWORD dwStream) const
{
	return m_Streams[dwStream].crType;

}

bool CVertexBuffer_D3D::IsStreamActive(DWORD dwStream) const
{
	return (m_Streams[dwStream].m_bUsed);
}

void CVertexBuffer_D3D::BindStream(DWORD dwStreamType, const CVertDecl* pVD) const
{
	//g_pDirect3DDevice->SetStreamSource(dwStreamType,m_D3Dvb,m_Streams[dwStreamType].iOffset,m_Streams[dwStreamType].dwElementSizeBytes*m_Streams[dwStreamType].uiElementsNum);

	HRESULT hr;

	//BREAKPOINT(1);

	int k=3000;
	k++;

	//BREAKPOINT(dwStreamType>10);

	unsigned int uiID = pVD->GetStreamMap(dwStreamType).uiNum;

	hr = g_pDirect3DDevice->SetStreamSource(uiID,m_D3Dvb,
		m_Streams[dwStreamType].iOffset,
		m_Streams[dwStreamType].dwElementSizeBytes*m_Streams[dwStreamType].uiElementsNum);
	

	k++;

	assert(SUCCEEDED(hr));
}

UINT CVertexBuffer_D3D::GetVertexSizeBytes(void)
{
	return 0;
}


void CVertexBuffer_D3D::CalculateTBN(int iIndexCount,const unsigned int* puiIndices,int iVertxOffset,int iVertCount,const CVec3f* pPosition,const CVec2f* pTexCoords,CVec3f* pNormals, CVec3f* pTangents, CVec3f* pBinormals)const
	{

	static CProfileStep TBN_PS("TBN");	
	TBN_PS.Begin();

				
	ZeroMemory(pNormals+iVertxOffset,sizeof(CVec3f)*iVertCount);
	ZeroMemory(pTangents+iVertxOffset,sizeof(CVec3f)*iVertCount);
	ZeroMemory(pBinormals+iVertxOffset,sizeof(CVec3f)*iVertCount);


	for (long a = 0; a < iIndexCount/3; a++)
	{   
		UINT i1=puiIndices[a*3+0];
		UINT i2=puiIndices[a*3+1];
		UINT i3=puiIndices[a*3+2];
		const CVec3f& v1 = pPosition[i1];   		
		const CVec3f& v2 = pPosition[i2];   
		const CVec3f& v3 = pPosition[i3];   
		const CVec2f& w1 = pTexCoords[i1];  
		const CVec2f& w2 = pTexCoords[i2];   
		const CVec2f& w3 = pTexCoords[i3];  
		
		float x1 = v2.v[0] - v1.v[0];   	
		float x2 = v3.v[0] - v1.v[0];    
		float y1 = v2.v[1] - v1.v[1];   
		float y2 = v3.v[1] - v1.v[1];  
		float z1 = v2.v[2] - v1.v[2];   
		float z2 = v3.v[2] - v1.v[2];   
		float s1 = w2.v[0] - w1.v[0];   
		float s2 = w3.v[0] - w1.v[0];  
		
		float t1 = w2.v[1] - w1.v[1];    
		float t2 = w3.v[1] - w1.v[1];  
		float r = 1.0F / ( (s1 * t2) - (s2 * t1));    
		CVec3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);   
		CVec3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);  
		pTangents[i1] += sdir;  
		pTangents[i2] += sdir;   
		pTangents[i3] += sdir;    
		pBinormals[i1] += tdir;   
		pBinormals[i2] += tdir;    
		pBinormals[i3] += tdir;

		//if (bGenNormals)
		{
			//CVec3f n=Cross(v1-v2,v3-v2);
			CVec3f n = (v1-v2)^(v3-v2);
			pNormals[i1]+=n;
			pNormals[i2]+=n;
			pNormals[i3]+=n;
		}

	}   


	

	//for (long a = 0; a < iVertCount; a++)   
	//{ 
	//	//if (bGenNormals)
	//	pNormals[a]=Normalize(pNormals[a]);

	//	const CVec3f& n = pNormals[a];
	//	const CVec3f& t = pTangents[a];  
	//	pTangents[a] = Normalize((t - n * (n * t)));

	//	float f=(((n ^ t) * pBinormals[a]) < 0.0F) ? -1.0F : 1.0F;

	//	// debug debug
	//	//f = 1.f;

	//	pBinormals[a]=Normalize(n^t)*f;

	//} 

	

	for ( int i = iVertxOffset; i < iVertxOffset+iVertCount; i++ )
	{
		CVec3f &v = pNormals[i];
	

		v.Normalize();

		{
			CVec3f &t = pTangents[i];

			t -= v * ( t * v ) ;
			
			t.Normalize();
		}
		{
			CVec3f &t = pBinormals[i];

			t -= v * ( t * v );
			t.Normalize();
		}
	}

	TBN_PS.End();

}


/*

#define _alloca16( x )					((void *)((((int)_alloca( (x)+15 )) + 15) & ~15))

void CVertexBuffer_D3D::CalculateTBN(int iIndexCount,
									 const unsigned int* puiIndices,
									 int iVertxOffset,
									 int iVertxOffset,
									 int iVertCount,
									 const CVec3f* pPosition,
									 const CVec2f* pTexCoords,
									 CVec3f* pNormals, 
									 CVec3f* pTangents, 
									 CVec3f* pBinormals)const
{
	int i;

	bool *used = (bool *)_alloca16( iVertCount * sizeof( used[0] ) );
	memset( used, 0, iVertCount * sizeof( used[0] ) );

	//idPlane *planesPtr = planes;
	 

	for ( i = 0; i < iIndexCount; i += 3 ) 
	{
		//idDrawVert *a, *b, *c;

		const CVec3f *vA, *vB, *vC;
		const CVec2f *tcA, *tcB, *tcC;
		

		unsigned long signBit;
		float d0[5], d1[5], f, area;
		CVec3f n, t0, t1;

		int v0 = puiIndices[i + 0];
		int v1 = puiIndices[i + 1];
		int v2 = puiIndices[i + 2];

		vA = pPosition + v0;
		vB = pPosition + v1;
		vC = pPosition + v2;

		tcA = pTexCoords + v0;
		tcB = pTexCoords + v1;
		tcC = pTexCoords + v2;



		d0[0] = vB->v[0] - vA->v[0];
		d0[1] = vB->v[1] - vA->v[1];
		d0[2] = vB->v[2] - vA->v[2];
		d0[3] = tcB->v[0] - tcA->v[0];
		d0[4] = tcB->v[1] - tcA->v[1];

		d1[0] = vC->v[0] - vA->v[0];
		d1[1] = vC->v[1] - vA->v[1];
		d1[2] = vC->v[2] - vA->v[2];
		d1[3] = tcC->v[0] - tcA->v[0];
		d1[4] = tcC->v[1] - tcA->v[1];

		// normal
		n.v[0] = d1[1] * d0[2] - d1[2] * d0[1];
		n.v[1] = d1[2] * d0[0] - d1[0] * d0[2];
		n.v[2] = d1[0] * d0[1] - d1[1] * d0[0];

		//f = idMath::RSqrt( n.x * n.x + n.y * n.y + n.z * n.z );
		//n.x *= f;
		//n.y *= f;
		//n.z *= f;

		n.Normalize();


		//planesPtr->SetNormal( n );
		//planesPtr->FitThroughPoint( a->xyz );
		//planesPtr++;

		// area sign bit
		area = d0[3] * d1[4] - d0[4] * d1[3];
		signBit = ( *(unsigned long *)&area ) & ( 1 << 31 );

		// first tangent
		t0.v[0] = d0[0] * d1[4] - d0[4] * d1[0];
		t0.v[1] = d0[1] * d1[4] - d0[4] * d1[1];
		t0.v[2] = d0[2] * d1[4] - d0[4] * d1[2];

		

		t0.Normalize();
		//f= signBit ? -1.0F : 1.0F;
		f= signBit ? 1.0F : -1.0F;
		t0 *= f;




		// second tangent
		t1.v[0] = d0[3] * d1[0] - d0[0] * d1[3];
		t1.v[1] = d0[3] * d1[1] - d0[1] * d1[3];
		t1.v[2] = d0[3] * d1[2] - d0[2] * d1[3];

		
		t1.Normalize();
		//f= signBit ? -1.0F : 1.0F;
		f= signBit ? 1.0F : -1.0F;
		t1 *= f;



		if ( used[v0] ) {
			pNormals[v0]   += n;
			pTangents[v0]  += t0;
			pBinormals[v0] += t1;

		} else {
			
			pNormals[v0]   = n;
			pTangents[v0]  = t0;
			pBinormals[v0] = t1;
			used[v0] = true;

		}

		if ( used[v1] ) {

			pNormals[v1]   += n;
			pTangents[v1]  += t0;
			pBinormals[v1] += t1;

		} else {
			
			pNormals[v1]   = n;
			pTangents[v1]  = t0;
			pBinormals[v1] = t1;

			used[v1] = true;
		}

		if ( used[v2] ) {

			pNormals[v2]   += n;
			pTangents[v2]  += t0;
			pBinormals[v2] += t1;

		} else {		
			pNormals[v2]   = n;
			pTangents[v2]  = t0;
			pBinormals[v2] = t1;

			used[v2] = true;
		}


	}

	for ( int i = 0; i < iVertCount; i++ )
	{
		CVec3f &v = pNormals[i];
	

		v.Normalize();

		{
			CVec3f &t = pTangents[i];

			t -= v * ( t * v ) ;
			
			t.Normalize();
		}
		{
			CVec3f &t = pBinormals[i];

			t -= v * ( t * v );
			t.Normalize();
		}
	}

}*/