#ifndef CRIMSON_GENERAL_MATH_H
#define CRIMSON_GENERAL_MATH_H

#pragma once

#include <Windows.h>
#include <math.h>

#include <string>

//#define PI (3.1415926535897932384626433832795028841971693993751)
#define PI    (3.14159265358979)
//#define TWOPI (6.283185307179586476925286766558)
#define TWOPI (6.28318530717958)
//#define TINY_EPSILON    0.00000000001f
//#define TINY_EPSILON    0.000000001f
#define EPSILON         0.0078125f
#define GIANT_EPSILON   7.f
#define PIOVER180 (0.0174532925)
#define DEG2RAD( x ) ( (x) * PI / 180.0f )
#define RAD2DEG( x ) ( (180.f) * x /PI   )

#define RANDOM_NUM        ((float)rand()/(RAND_MAX+1))

bool IsOdd(int iTest);

// Wave Functions

#define WF_NONE            0
#define WF_SIN             1
#define WF_TRIANGLE        2
#define WF_SQUARE          3
#define WF_SAWTOOTH        4
#define WF_INVERSESAWTOOTH 5
#define WF_NOISE           6

class CWave
{
public:
	CWave();
	CWave(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	~CWave();

	void SetParams(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);

	float ComputeSin(float fTime);
	float ComputeTriangle(float time);
	float ComputeSquare(float time);
	float ComputeSawTooth(float time);
	float ComputeInverseSawTooth(float time);
	float ComputeNoise(float time);
	float Compute(float fTime);

	std::string ComputeSin_String();
	std::string ComputeTriangle_String();
	std::string ComputeSquare_String();
	std::string ComputeSawTooth_String();
	std::string ComputeInverseSawTooth_String();
	std::string ComputeNoise_String();
	std::string Compute_String();

	UINT  m_uiWaveType;
	float m_fBase;
	float m_fAmplitude;
	float m_fPhase;
	float m_fFreq;
};


//dtag blur reference
template <class T>
float Blur(int x, int y, T* eingabe, int breite, int hoehe,float sigma)
	{
		static float filter[5][5];
		static bool done=false;
		static float sumfacs=0;
		if (!done)
		{
			for (int i=-2;i<=2;i++)
			{
				for (int j=-2;j<=2;j++)
				{
					filter[i+2][j+2]=exp(-(i*i+j*j)/(2*sigma*sigma));
					sumfacs+=filter[i+2][j+2];
				}
			}

			done=true;
		}
		float sum=0.0f;
		for (int c=-2;c<=2;c++)
		{
			for (int d=-2;d<=2;d++)
			{
				sum+=filter[d+2][c+2]*(float)eingabe[(y+c)*breite+(x+d)];
			}
		}
		return sum/sumfacs;

	}



#endif // YOEL_GENERAL_MATH_H