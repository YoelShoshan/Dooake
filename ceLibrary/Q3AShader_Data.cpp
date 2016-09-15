#include "stdafx.h"
#include "Q3AShader_Data.h"

#include <GL/gl.h>

STCModObject::STCModObject()
{
	m_iNum=0;
	m_pfRotate=NULL;
	m_pfScale=NULL;
	m_pfScroll=NULL;
	m_pStretch=NULL;
	m_pfTransform=NULL;
	m_pfTurb=NULL;
};

STCModObject::~STCModObject()
{

	SafeDeleteArray(m_pfRotate);
	//SafeDelete(m_pfRotate);
	SafeDeleteArray(m_pfScale);
	SafeDeleteArray(m_pfScroll);
	SafeDeleteArray(m_pStretch);
	//SafeDelete(m_pStretch);
	SafeDeleteArray(m_pfTransform);
	SafeDeleteArray(m_pfTurb);


}; 



void STCModObject::AllocAndFillScroll(float fSSpeed,float fTSpeed)
{
	m_dwOrder[m_iNum] = TCMOD_SCROLL;
	m_pfScroll = new float[2];	
	m_pfScroll[0] = fSSpeed;
	m_pfScroll[1] = fTSpeed;
	m_iNum++;
}

void STCModObject::AllocAndFillRotate(float fDeg)
{
	m_dwOrder[m_iNum] = TCMOD_ROTATE;
	m_pfRotate = new float[1];	
	m_pfRotate[0] = fDeg;
	m_iNum++;
}

void STCModObject::AllocAndFillScale(float fSScale,float fTScale)
{
	m_dwOrder[m_iNum] = TCMOD_SCALE;
	m_pfScale = new float[2];	
	m_pfScale[0] = fSScale;
	m_pfScale[1] = fTScale;
	m_iNum++;
}

void STCModObject::AllocAndFillStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_dwOrder[m_iNum] = TCMOD_STRETCH;
	m_pStretch = new CWave[1];
	m_pStretch[0].SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
	m_iNum++;
}


void STCModObject::AllocAndFillTransform(float f0,float f1,float f2,float f3,float f4,float f5)
{
	m_dwOrder[m_iNum] = TCMOD_TRANSFORM;
	m_pfTransform = new float[6];
	m_pfTransform[0] = f0; m_pfTransform[1] = f1; m_pfTransform[2] = f2;
	m_pfTransform[3] = f3; m_pfTransform[4] = f4; m_pfTransform[5] = f5;
	m_iNum++;
}

void STCModObject::AllocAndFillTurb(float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_dwOrder[m_iNum] = TCMOD_TURB;
	m_pfTurb = new float[4];
	m_pfTurb[0] = fBase;
	m_pfTurb[1] = fAmplitude;
	m_pfTurb[2] = fPhase;
	m_pfTurb[3] = fFreq;
	m_iNum++;
}


/////////////////////////////////////
//
//  CQ3A_ShaderPass class implementation
//
/////////////////////////////////////


CQ3A_ShaderPass::CQ3A_ShaderPass()
{
	m_uiTextureNum=1;
	m_fFreq=1.f;
	//m_uiTexturesIDs    = new UINT[MAX_ANIM_TEX];
	//m_pTextures = new Texture[MAX_ANIM_TEX];


	//TODO: change this so no memory would be wasted (only alloc if needed)
	/*m_ppTextures = new Texture*[MAX_ANIM_TEX];
	for (int i=0;i<MAX_ANIM_TEX;i++)
	m_ppTextures[i] = new Texture;*/



	m_uiActiveFrame = 0;
	m_fPassTime = 0;
	/////////////////

	m_bFarBoxUsed=false;

	m_dwFlags = SPF_DEPTHWRITE;

	m_dwSrcBlend = GL_ONE;
	m_dwDstBlend = GL_ZERO;

	m_dwAlphaFunc = ALPHAFUNC_NONE;

	m_dwDEPTHFunc = DEPTHFUNC_LEQUAL;

	m_uiRGBGen=RGBGEN_IDENTITY;
	m_uiALPHAGen=ALPHAGEN_IDENTITY;
	m_dwTCGen=TCGEN_NONE;


	bTCModUsed = false;
}

CQ3A_ShaderPass::~CQ3A_ShaderPass()
{

}


void CQ3A_ShaderPass::SetRGBGen(UINT uiRGBGen)
{
	m_uiRGBGen = uiRGBGen;
}

/*UINT CQ3A_ShaderPass::GetRGBGen(void)
{
return m_uiRGBGen;
}*/



void CQ3A_ShaderPass::SetRGBGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_RGBWave.SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

float CQ3A_ShaderPass::ComputeRGBGenWave(float fTime)
{
	return m_RGBWave.Compute(fTime);
}




void CQ3A_ShaderPass::SetALPHAGen(UINT uiALPHAGen)
{
	m_uiALPHAGen = uiALPHAGen;
}

/*UINT CQ3A_ShaderPass::GetALPHAGen(void)
{
return m_uiALPHAGen;
}*/

void CQ3A_ShaderPass::SetALPHAGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_ALPHAWave.SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

float CQ3A_ShaderPass::ComputeALPHAGenWave(float fTime)
{
	return m_ALPHAWave.Compute(fTime);
}


void CQ3A_ShaderPass::AddTCModScroll(float fSSpeed, float fTSpeed)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillScroll(fSSpeed,fTSpeed);
}

void CQ3A_ShaderPass::AddTCModRotate(float fDeg)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillRotate(fDeg);
}

void CQ3A_ShaderPass::AddTCModScale(float fSScale,float fTScale)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillScale(fSScale,fTScale);
}

void CQ3A_ShaderPass::AddTCModStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillStretch(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CQ3A_ShaderPass::AddTCModTransform(float f0,float f1,float f2,float f3,float f4,float f5)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillTransform(f0,f1,f2,f3,f4,f5);
}

void CQ3A_ShaderPass::AddTCModTurb(float fBase,float fAmplitude,float fPhase,float fFreq)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillTurb(fBase,fAmplitude,fPhase,fFreq);
}

void CQ3A_ShaderPass::SetAnim(UINT uiNum,float fFreq)
{
	m_uiTextureNum = uiNum;
	m_fFreq        = fFreq;
}


/// I HAVE TO TAKE CARE OF THIS SOMEHOW FOR SMART POINTERS!!!!!!!
void CQ3A_ShaderPass::SetAlreadyAllocatedNoAnimTexture(UINT uiTextureID) // for lightmaps
{
	m_uiTextureNum = 1;
	//m_spTextures[0] = NULL;
	//TODO: here I'm supposed to load the actual lightmaps!

	//Explanation - I cannot pass a NULL because then there will be made an access from NULL which would crash,
	// that's why I'm using this "fake" texture to prevent that
	// basically I'm supposed to search through the texturemanager queue list and find a texture with that id,
	// and if it isn't found, I should create a "fake" new texture, containing no data, and only the dimentions and
	// opengl texID !!!!!!!!!!!!!
	// (all that inside the manager)


	//m_spTextures[0] = CSingleton<TextureManager>::Inst()->LoadTexAndPush("ceImages/white.jpg",false,false,true);
	m_spTextures[0] = "ceData/white.jpg";

	m_bClamp[0] = true;
}

void CQ3A_ShaderPass::AddNoAnimTexture(char* caTexName,bool bMipMap,bool bClamp)
{
	m_uiTextureNum = 1;

	//m_ppTextures[0] = CSingleton<TextureManager>::Instance()->LoadTexAndPush(caTexName,bMipMap,bClamp);

	//m_spTextures[0] = CSingleton<TextureManager>::Inst()->LoadTexAndPush(caTexName,bMipMap,bClamp,true);
	m_spTextures[0] = caTexName;
	m_bClamp[0] = bClamp;
}

void CQ3A_ShaderPass::AddFrameTexture(UINT uiNum,char* caTexName)
{
	//m_ppTextures[uiNum] = CSingleton<TextureManager>::Instance()->LoadTexAndPush(caTexName,true,false);

	//m_spTextures[uiNum] = CSingleton<TextureManager>::Inst()->LoadTexAndPush(caTexName,true,false,true);
	m_spTextures[uiNum] = caTexName;

	m_bClamp[uiNum] = true;
}


void CQ3A_ShaderPass::SetBlendValues(DWORD dwSrcBlend,DWORD dwDstBlend)
{
	m_dwSrcBlend = dwSrcBlend;
	m_dwDstBlend = dwDstBlend;
}

/*DWORD CQ3A_ShaderPass::GetSrcBlend(void)
{
return m_dwSrcBlend;
}

DWORD CQ3A_ShaderPass::GetDstBlend(void)
{
return m_dwDstBlend;
}
*/

bool CQ3A_ShaderPass::IsBlended(void)
{
	if (m_dwSrcBlend != GL_ONE || m_dwDstBlend != GL_ZERO)
		return true;

	return false;
}

const char* CQ3A_ShaderPass::GetTextureName(UINT uiNum)
{
	return m_spTextures[uiNum].c_str();	
}

bool	CQ3A_ShaderPass::GetTextureClamp(UINT uiNum)
{
	return m_bClamp[uiNum];
}

/*
void CQ3A_ShaderPass::CalcActiveFrame(void)
{	

if (m_uiTextureNum>1)
{
float t_fTemp1 =  st_Timer::Inst()->TimerGetTimeFAKE() / 1000.f * m_fFreq;
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
*/

UINT CQ3A_ShaderPass::GetActiveFrameTexID(void)
{
	if (m_spTextures[m_uiActiveFrame].length()>1)
		//return m_spTextures[m_uiActiveFrame]->GetID();
		return 0;
	else
		return 0; // Todo: return the tex id of white
}



SDeformVertexesObject::SDeformVertexesObject()
{
	m_iMoveNum=m_iWaveNum=m_iNormalNum=m_iBulgeNum=0;

	// move
	m_ppMove = new CWave*[DV_MOVE_MAX];
	m_MoveVert = new CVec3f*[DV_MOVE_MAX];

	// wave

	m_ppWave = new CWave*[DV_WAVE_MAX];
	m_pWaveDivs = new float[DV_WAVE_MAX];

	// autoSprite

	m_bIsAutoSprite = false;

	// bulge

	m_ppfBulgeParms = new float*[DV_BULGE_MAX]; 
}

SDeformVertexesObject::~SDeformVertexesObject()
{
	for (int i=0;i<m_iMoveNum;i++)
	{
		SafeDelete(m_ppMove[i]);
		SafeDelete(m_MoveVert[i]);
	}

	SafeDeleteArray(m_ppMove);
	SafeDeleteArray(m_MoveVert);

	for (int i=0;i<m_iWaveNum;i++)
	{
		SafeDelete(m_ppWave[i]);		
	}

	SafeDeleteArray(m_pWaveDivs);
	SafeDeleteArray(m_ppWave);


	for (int i=0;i<m_iBulgeNum;i++)
	{
		SafeDeleteArray(m_ppfBulgeParms[i]);		
	}

	SafeDeleteArray(m_ppfBulgeParms);



}

void SDeformVertexesObject::AddMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	assert(m_iMoveNum<DV_MOVE_MAX-1);


	m_ppMove[m_iMoveNum] = new CWave;
	m_ppMove[m_iMoveNum]->SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);

	m_MoveVert[m_iMoveNum] = new CVec3f(x,y,z);
	m_iMoveNum++;
}

void SDeformVertexesObject::AddWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	assert(m_iWaveNum<DV_WAVE_MAX-1);


	m_ppWave[m_iWaveNum] = new CWave;
	m_ppWave[m_iWaveNum]->SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);

	m_pWaveDivs[m_iWaveNum] = fDiv;
	m_iWaveNum++;
}

void SDeformVertexesObject::AddBulge(float f1,float f2,float f3)
{
	assert(m_iBulgeNum<DV_BULGE_MAX-1);

	m_ppfBulgeParms[m_iBulgeNum] = new float[3];

	m_ppfBulgeParms[m_iBulgeNum][0] = f1;
	m_ppfBulgeParms[m_iBulgeNum][1] = f2;
	m_ppfBulgeParms[m_iBulgeNum][2] = f3;

	m_iBulgeNum++;
}
