#ifndef Q3ASHADER_DATA_H
#define Q3ASHADER_DATA_H

#pragma once

#include <windows.h>
#include "Math3d.h"
#include "GMath.h"
#include "SmartPointer.h"
#include "Texture.h"
#include "Timer_IE.h"
#include "Singleton.h"

#include "Q3AShader_Data.h"


#define RGBGEN_NOT_INIT         0
#define RGBGEN_IDENTITY         1
#define RGBGEN_IDENTITYLIGHTING 2
#define RGBGEN_WAVE             3
#define RGBGEN_VERTEX           4
#define RGBGEN_EXACTVERTEX      5
#define RGBGEN_ONEMINUSVERTEX   6
#define RGBGEN_ENTITY           7 // not implemented yet
#define RGBGEN_ONEMINUSENTITY   8 // not implemented yet
#define RGBGEN_LIGHTINGDIFFUSE  9 // not implemented yet
#define RGBGEN_FLARE           10 // not implemented yet
#define RGBGEN_FOG             11 // not implemented yet
#define RGBGEN_CONST           12
#define RGBGEN_MATERIAL        13 // not implemented yet


#define ALPHAGEN_NOT_INIT             0
#define ALPHAGEN_IDENTITY             1
#define ALPHAGEN_IDENTITYLIGHTING     2
#define ALPHAGEN_WAVE                 3
#define ALPHAGEN_VERTEX               4
#define ALPHAGEN_ONEMINUSVERTEX       5
#define ALPHAGEN_ENTITY               6 // not implemented yet
#define ALPHAGEN_ONEMINUSENTITY       7 // not implemented yet
#define ALPHAGEN_LIGHTINGDIFFUSE      8 // not implemented yet
#define ALPHAGEN_PORTAL               9 // not implemented yet
#define ALPHAGEN_LIGHTINGSPECULAR    10 // not implemented yet
#define ALPHAGEN_CONST               11 

#define TCGEN_NONE             0
#define TCGEN_BASE             1
#define TCGEN_LIGHTMAP         2
#define TCGEN_VECTOR           3
#define TCGEN_ENVIRONMENT      4
#define TCGEN_SKY_CUBEMAP      5
#define TCGEN_LIGHTINGDIFFUSE  6


#define SP_TCMOD_MAX        6

#define TCMOD_NONE          0
#define TCMOD_ROTATE        1
#define TCMOD_SCALE         2
#define TCMOD_SCROLL        3
#define TCMOD_STRETCH       4
#define TCMOD_TRANSFORM     5
#define TCMOD_TURB          6

// the current implementation PROHIBITS the artist from writing TWICE in the same pass the same tcmod option
// this can be easily solved, but not that important to me right now (not used anywhere)
// I'm using pointers to floats/cwave for two reasons:
// 1. So I wouldn't allocate unneeded space
// 2. So I will be able to expand this in the future so there will be no limitation

struct STCModObject
{
	STCModObject();

	~STCModObject();


	void AllocAndFillScroll(float fSSpeed,float fTSpeed);
	void AllocAndFillRotate(float fDeg);
	void AllocAndFillScale(float fSScale,float fTScale);
	void AllocAndFillStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	void AllocAndFillTransform(float f0,float f1,float f2,float f3,float f4,float f5);

	void AllocAndFillTurb(float fBase,float fAmplitude,float fPhase,float fFreq);

	int m_iNum;
	DWORD m_dwOrder[SP_TCMOD_MAX];
	float* m_pfRotate; // 1 float
	float* m_pfScale; // 2 floats	
	float* m_pfScroll; // 2 floats
	CWave* m_pStretch; // a wave
	float* m_pfTransform; // 6 floats
	float* m_pfTurb; // 4 floats

};



#define ALPHAFUNC_NONE  (1<<0)
#define ALPHAFUNC_GT0   (1<<1)
#define ALPHAFUNC_LT128 (1<<2)
#define ALPHAFUNC_GE128 (1<<3)

#define DEPTHFUNC_LEQUAL (1<<0)
#define DEPTHFUNC_EQUAL  (1<<1)


////////////////////////////////////
// implementation of CQ3A_ShaderPass
////////////////////////////////////


class CQ3A_ShaderPass
{
#define MAX_ANIM_TEX 8

#define SPF_ISLIGHTMAP (1<<0)
#define SPF_DEPTHWRITE (1<<1)

public:

	CQ3A_ShaderPass();
	~CQ3A_ShaderPass();

	///////////////////
	// texture loading
	///////////////////

	// just a simple 1 texture
	void AddNoAnimTexture(char* caTexName,bool bMipMap,bool bClamp);

	// allocate a texture of a frame
	void AddFrameTexture(UINT uiNum,char* caTexName);

	// if we already know the tex id -  also good for lightmaps
	void SetAlreadyAllocatedNoAnimTexture(UINT uiTextureID);

	// set number of frames and frequency
	void SetAnim(UINT uiNum,float fFreq);





	////////////////
	// Flags
	////////////////

	DWORD GetShaderPassFlags(void) { return m_dwFlags;};
	void  SetShaderPassFlags(DWORD dwFlags) {m_dwFlags = dwFlags;};

	////////////////
	// DEPTHFunc
	////////////////

	void  SetDEPTHFunc(DWORD dwDEPTHFunc) { m_dwDEPTHFunc = dwDEPTHFunc;};
	DWORD GetDEPTHFunc(void) { return m_dwDEPTHFunc;};

	////////////////
	// ALPHAFunc
	////////////////

	void  SetALPHAFunc(DWORD dwALPHAFunc) { m_dwAlphaFunc = dwALPHAFunc;};
	DWORD GetALPHAFunc(void) { return m_dwAlphaFunc;};

	////////////////
	// RGBGen
	////////////////

	void SetRGBGen(UINT uiRGBGen);

	//UINT GetRGBGen(void);


	inline UINT CQ3A_ShaderPass::GetRGBGen(void) const
	{
		return m_uiRGBGen;
	}


	void SetRGBGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	float ComputeRGBGenWave(float fTime);
	CWave* GetRGBGenWave(void) {return &m_RGBWave;};

	CWave* GetAlphaGenWave(void) {return &m_ALPHAWave;};

	////////////////
	// ALPHAGen
	////////////////

	void SetALPHAGen(UINT uiALPHAGen);

	//UINT GetALPHAGen(void);

	inline UINT CQ3A_ShaderPass::GetALPHAGen(void) const
	{
		return m_uiALPHAGen;
	}

	void SetALPHAGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	float ComputeALPHAGenWave(float fTime);


	////////////////
	// TCGen
	////////////////

	void SetTCGen(DWORD TCGen) { m_dwTCGen = TCGen;};
	DWORD GetTCGen(void) { return m_dwTCGen;};


	////////////////
	// TCMod
	////////////////

	bool GetIsTCModUsed(void) { return bTCModUsed;};
	int  GetTCModNum(void) { return m_TCModObject.m_iNum;};
	DWORD GetTCModAt(int iNum) {return m_TCModObject.m_dwOrder[iNum]; };

	STCModObject* GetTCModObject(void) { return &m_TCModObject;};

	void AddTCModScroll(float fSSpeed, float fTSpeed);
	void AddTCModRotate(float fDeg);
	void AddTCModScale(float fSScale,float fTScale);
	void AddTCModStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	void AddTCModTransform(float f0,float f1,float f2,float f3,float f4,float f5);
	void AddTCModTurb(float fBase,float fAmplitude,float fPhase,float fFreq);


	////////////////
	// blendFunc
	////////////////

	void SetBlendValues(DWORD dwSrcBlend,DWORD dwDstBlend);
	bool IsBlended(void);

	/*DWORD GetSrcBlend(void);
	DWORD GetDstBlend(void);*/

	inline DWORD CQ3A_ShaderPass::GetSrcBlend(void) const
	{
		return m_dwSrcBlend;
	}

	inline DWORD CQ3A_ShaderPass::GetDstBlend(void) const
	{
		return m_dwDstBlend;
	}


	const char* GetTextureName(UINT uiNum); 

	bool	GetTextureClamp(UINT uiNum);

	//void CalcActiveFrame(void);    


	inline void CQ3A_ShaderPass::CalcActiveFrame(float fTimeFake)
	{	

		if (m_uiTextureNum>1)
		{
			float t_fTemp1 =  fTimeFake / 1000.f * m_fFreq;
			float t_fTemp2=(int)fmodf((float)t_fTemp1,(float)m_uiTextureNum); 

			if (t_fTemp2-floorf(t_fTemp2)>=0.5f)
				m_uiActiveFrame = floorf(t_fTemp2)+1;
			else
				m_uiActiveFrame = floorf(t_fTemp2);
		}
		else
		{
			// TODO: possible to move?
			m_uiActiveFrame = 0;
		}		
	}


	UINT GetActiveFrameTexID(void);

	// used by my skybox system
	void SetFarBoxUsed(bool bSet){m_bFarBoxUsed = bSet;};
	bool GetFarBoxUsed(void){return m_bFarBoxUsed;};

private:

	std::string m_spTextures[MAX_ANIM_TEX];
	bool		m_bClamp[MAX_ANIM_TEX];

	UINT  m_uiTextureNum;
	float m_fFreq;
	UINT  m_uiActiveFrame;
	float m_fPassTime;

	// skybox

	bool m_bFarBoxUsed;

	// blendfunc
	DWORD m_dwSrcBlend;
	DWORD m_dwDstBlend;

	// alphafunc

	DWORD m_dwAlphaFunc;

	// depthfunc

	DWORD m_dwDEPTHFunc;

	// flags

	DWORD m_dwFlags;

	// RGBGen + ALPHAGen

	UINT    m_uiRGBGen;
	CWave  m_RGBWave;
	UINT    m_uiALPHAGen;
	CWave  m_ALPHAWave;

	DWORD   m_dwTCGen;

	// TCMod

	STCModObject m_TCModObject;
	bool         bTCModUsed;


};


//TODO: How am I supposed to deal with so much??

#define SURFACEPARM_NONE          (1<<0)
#define SURFACEPARM_TRANS         (1<<1)
#define SURFACEPARM_WATER         (1<<2)
#define SURFACEPARM_SKY           (1<<3)
#define SURFACEPARM_NODRAW        (1<<4)


/*#define SURFACEPARM_FOG         (1<<5)
#define SURFACEPARM_LAVA          (1<<6)
#define SURFACEPARM_ALPHASHADOW   (1<<7)
#define SURFACEPARM_AREAPORTAL    (1<<8)
#define SURFACEPARM_CLUSTERPORTAL (1<<9)
#define SURFACEPARM_DONOTENTER    (1<<10)
#define SURFACEPARM_FLESH         (1<<11)
#define SURFACEPARM_METALSTEPS    (1<<12)
#define SURFACEPARM_NODAMAGE      (1<<13)
#define SURFACEPARM_NODLIGHT      (1<<14)
#define SURFACEPARM_NODROP        (1<<15)
#define SURFACEPARM_NOIMPACT      (1<<16)
#define SURFACEPARM_NOMARKS       (1<<17)
#define SURFACEPARM_NOLIGHTMAP    (1<<18)
#define SURFACEPARM_NOSTEPS       (1<<19)
#define SURFACEPARM_NONSOLID      (1<<20)
#define SURFACEPARM_ORIGIN        (1<<21)
#define SURFACEPARM_PLAYERCLIP    (1<<22)
#define SURFACEPARM_SLICK         (1<<23)
#define SURFACEPARM_SLIME         (1<<24)
#define SURFACEPARM_STRUCTURAL    (1<<25)*/


static const DWORD MAX_PASS_COUNT=8; 
static const DWORD MAX_DEFORMVERTEX_COUNT=6; 

// ShaderFlags 

#define FLAG_LIGHTMAP       (1<<1)
#define FLAG_NOCULL         (1<<2)
#define FLAG_CULLFRONT      (1<<3)
#define FLAG_CULLBACK       (1<<4)
#define FLAG_NOMIPMAP       (1<<5)
#define FLAG_DEPTHWRITE     (1<<6)

// Sort Keywords 

static const UINT SORT_NOT_INIT=0; 
static const UINT SORT_ONE=1; 
static const UINT SORT_PORTAL=1; 
static const UINT SORT_SKY=2; 
static const UINT SORT_OPAQUE=3; 
static const UINT SORT_BANNER=6; 
static const UINT SORT_UNDERWATER=8; 
static const UINT SORT_ADDITIVE=9; 
static const UINT SORT_NEAREST=16;

#define DV_MOVE_MAX   3
#define DV_BULGE_MAX  3
#define DV_WAVE_MAX   3
#define DV_NORMAL_MAX 3

struct SDeformVertexesObject
{
	SDeformVertexesObject();
	~SDeformVertexesObject();

	void AddMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	void AddWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	void AddAutoSprite(void) {m_bIsAutoSprite=true;};
	void AddBulge(float f1,float f2,float f3);


	int m_iNormalNum; // not implemented yet 

	// move

	int m_iMoveNum;
	CWave**      m_ppMove;
	CVec3f**  m_MoveVert;

	// wave

	int m_iWaveNum;
	CWave** m_ppWave;
	float*  m_pWaveDivs;

	// buldge

	int m_iBulgeNum;
	float** m_ppfBulgeParms;







	// autoSprite

	bool m_bIsAutoSprite;
};


// used to seperate functionality from implementation
struct CQ3A_Shader_Data
{
public:
	bool            m_bAllBlended;  
	DWORD           m_dwShaderFlags; 
	UINT            m_uiShaderSort; 
	DWORD           m_dwSurfaceParm;

	bool                  m_bDVUsed;
	SDeformVertexesObject m_DVObject;

	bool            m_bFarBoxUsed;

	char*           m_cpShaderName; 
	UINT            m_uiPassesNum; 

	CQ3A_ShaderPass**   m_ppPasses; 
};


#endif