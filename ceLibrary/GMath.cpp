#include "stdafx.h"
#include "GMath.h"

///////////////////////////
//#include "ZedTracker.h"

///////////////////////////

#include <stdlib.h>

bool IsOdd(int iTest)
{	
	if ( (iTest%2)==1)
		return true;
	else
		return false;
}

CWave::CWave()
{
}

CWave::CWave(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}


CWave::~CWave()
{
	m_uiWaveType  = WF_SIN;
	m_fBase       = 0.5f;
	m_fAmplitude  = 0.f;
	m_fPhase      = 0.f;
	m_fFreq       = 0.f;
	
}

void CWave::SetParams(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_uiWaveType  = uiWaveType;
	m_fBase       = fBase;
	m_fAmplitude  = fAmplitude;
	m_fPhase      = fPhase;
	m_fFreq       = fFreq;
}


std::string CWave::Compute_String()
{
	if (m_uiWaveType==WF_SIN)
		return ComputeSin_String();
	else
	if (m_uiWaveType==WF_TRIANGLE)
		return ComputeTriangle_String();
	else
	if (m_uiWaveType==WF_SQUARE)
		return ComputeSquare_String();
	else
	if (m_uiWaveType==WF_SAWTOOTH)
		return ComputeSawTooth_String();
	else
	if (m_uiWaveType==WF_INVERSESAWTOOTH)
		return ComputeInverseSawTooth_String();
	else
	if (m_uiWaveType==WF_NOISE)
		return ComputeNoise_String();
	else
		return ComputeSin_String();


}

float CWave::Compute(float fTime)
{
	//// ONLY FOR DEBUG!!!!!!!!
	fTime/=1000.f;
	//////////////////////////

	if (m_uiWaveType==WF_SIN)
		return ComputeSin(fTime);
	else
	if (m_uiWaveType==WF_TRIANGLE)
		return ComputeTriangle(fTime);
	else
	if (m_uiWaveType==WF_SQUARE)
		return ComputeSquare(fTime);
	else
	if (m_uiWaveType==WF_SAWTOOTH)
		return ComputeSawTooth(fTime);
	else
	if (m_uiWaveType==WF_INVERSESAWTOOTH)
		return ComputeInverseSawTooth(fTime);
	else
	if (m_uiWaveType==WF_NOISE)
		return ComputeNoise(fTime);
	else
		return ComputeSin(fTime);
}

std::string CWave::ComputeSin_String()
{
	std::string res = "";	
	res += "wave_val = time;\n";

	char temp[100];
	sprintf(temp,"wave_val = %.2f + (%.2f*wave_val);\n",m_fPhase,m_fFreq);
	res += temp;
	
	res += "wave_val -= floor(wave_val);\n";
	res += "wave_val = sin (wave_val*6.28318530717958);\n";

	sprintf(temp,"wave_val = %.2f + (%.2f*wave_val);\n",m_fBase,m_fAmplitude);
	res += temp;

	return res;
}


float CWave::ComputeSin(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime = sinf (fTime*TWOPI);
	return m_fBase + m_fAmplitude*fTime;
}



std::string CWave::ComputeTriangle_String()
{
	std::string res = "";	
	res += "wave_val = time;\n";

	char temp[100];

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fPhase,m_fFreq);
	res += temp;

	sprintf(temp,"wave_val -= floor(wave_val);\n");
	res += temp;
	
	//sprintf(temp,"wave_val = (wave_val < 0.5) ? 2.0*wave_val : 2.0 - 2.0*wave_val;\n");
	//res += temp;

	/*sprintf(temp,"if (wave_val < 0.5)\n");
	res += temp;
	sprintf(temp,"    wave_val = 2.0 * wave_val;\n");
	res += temp;
	sprintf(temp,"else\n");
	res += temp;
	sprintf(temp,"    wave_val = 2.0 - (2.0 * wave_val);\n");
	res += temp;*/


	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fBase,m_fAmplitude);
	res += temp;
	
	return res;	
}

float CWave::ComputeTriangle(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime = (fTime < 0.5f)? 2.0f*fTime : 2.0f-2.0f*fTime;
	return m_fBase + m_fAmplitude*fTime;
}

std::string CWave::ComputeSawTooth_String()
{
	std::string res = "";	
	res += "wave_val = time;\n";

	char temp[100];

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fPhase,m_fFreq);
	res += temp;

	sprintf(temp,"wave_val -= floor(wave_val);\n");
	res += temp;

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fBase,m_fAmplitude);
	res += temp;

	return res;	
}

float CWave::ComputeSawTooth(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	return m_fBase + m_fAmplitude*fTime;
}


std::string CWave::ComputeInverseSawTooth_String()
{
	std::string res = "";	
	res += "wave_val = time;\n";

	char temp[100];

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fPhase,m_fFreq);
	res += temp;

	sprintf(temp,"wave_val -= floor(wave_val);\n");
	res += temp;

	sprintf(temp,"wave_val=  1.0f-wave_val;\n");
	res += temp;

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fBase,m_fAmplitude);
	res += temp;

	return res;	
}

float CWave::ComputeInverseSawTooth(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime=  1.0f-fTime;
	return m_fBase + m_fAmplitude*fTime;
}

std::string CWave::ComputeSquare_String()
{
	std::string res = "";	
	res += "wave_val = time;\n";

	char temp[100];

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fPhase,m_fFreq);
	res += temp;

	sprintf(temp,"wave_val -= floor(wave_val);\n");
	res += temp;

	//sprintf(temp,"wave_val= (wave_val < 0.5) ? 1.0f : -1.0f;\n");
	//res += temp;

	/*sprintf(temp,"if (wave_val < 0.5)\n");
	res += temp;
	sprintf(temp,"  wave_val = 1.0;\n");
	res += temp;
	sprintf(temp,"else\n");
	res += temp;
	sprintf(temp,"wave_val = -1.0;\n");
	res += temp;*/

	sprintf(temp,"wave_val = %.2f + %.2f*wave_val;\n",m_fBase,m_fAmplitude);
	res += temp;

	return res;	
}

float CWave::ComputeSquare(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime= (fTime < 0.5) ? 1.0f : -1.0f;
	return m_fBase + m_fAmplitude*fTime;
}


std::string CWave::ComputeNoise_String()
{
	// Not supported yet
	std::string res = "";	
	res += "wave_val = 1.0;\n";

	return res;	
}

float CWave::ComputeNoise(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime=(float)rand()*(1.0f/(float)RAND_MAX);
	//fTime=(((float)rand()+(float)rand())/2.0f) / (float)RAND_MAX;

	return m_fBase + m_fAmplitude*fTime;
	////return fTime;
}

