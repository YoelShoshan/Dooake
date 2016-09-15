#include "stdafx.h"
#include "CTimer.h"
#include <windows.h>

#include "Mmsystem.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

void CTimer::TimerInit(void)										// Initialize Our Timer (Get It Ready)
{
	m_fDeltaTime = 0.f;
	m_fDeltaTimeFAKE = 0.f;

	m_fFPS = 0.f;

	fpsAverageCounter = 0;

	// restore 
	m_fFakeSlowMotion = 1.f;
	//m_fFakeSlowMotion = 0.2f;
	


	m_fTotalTimeFAKE = 0;
	//memset(&timer, 0, sizeof(Timer));						// Clear Our Timer Structure

	/*cpu_speed = GetCPUSpeed();		
	cpu_speed_inv = 1000.0 * (1.0f/(cpu_speed*1000000.0f));*/

	DWORD t1 = GetPentiumCounter();
	Sleep(1000);
	DWORD t2 = GetPentiumCounter();
	correctionfactor=(t2-t1)/1000.0;

	cpu_speed_inv=1.0/correctionfactor;
	
	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	//if (!QueryPerformanceFrequency((LARGE_INTEGER *) &frequency))
	/*if (1)*/
	{
		// No Performace Counter Available
		performance_timer	= FALSE;					// Set Performance Timer To FALSE
		mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		reso_pentiumCounter		= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		frequency_pentium			= 1000;						// Set Our Timer Frequency To 1000
		mm_timer_elapsed	= mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	//else
	{
		QueryPerformanceFrequency((LARGE_INTEGER *) &frequency_perf);

		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &performance_timer_start);
		performance_timer			= TRUE;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		reso_perf				= (double) (((double)1.0f)/((double)frequency_perf));
		// Set The Elapsed Time To The Current Time
		performance_timer_elapsed	= performance_timer_start;
	}

	m_fTimerStart = GetTimeFAKE();

	pentium_timer_start = GetPentiumCounter();
}

// always uses the faster pentium counter
double CTimer::GetTime_FAST()
{
	__int64 time;											// time Will Hold A 64 Bit Integer
	//return( (double) ( timeGetTime() - mm_timer_start) * reso_pentiumCounter);
	//return( (double) ( timeGetTime() - mm_timer_start));

	//return( (double) ( GetPentiumCounter() - pentium_timer_start) * 0.000001);

	return (double)(double(GetPentiumCounter()-pentium_timer_start)*cpu_speed_inv);
	
}

double CTimer::GetTime()										// Get Time In Milliseconds
{
	//reso_perf
	//reso_pentiumCounter

	__int64 time;											// time Will Hold A 64 Bit Integer

	if (performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (double) ( time - performance_timer_start) * reso_perf)*1000.f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (double) ( timeGetTime() - mm_timer_start) * reso_pentiumCounter)*1000.f;
	}
}

double CTimer::GetTimeSeconds()
{
	__int64 time;											// time Will Hold A 64 Bit Integer

	if (performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (double) ( time - performance_timer_start) * reso_perf);
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (double) ( timeGetTime() - mm_timer_start) * reso_pentiumCounter);
	}

}

double CTimer::GetTimeFAKE()
{	
	return GetTime()*m_fFakeSlowMotion;  // original
}



double CTimer::GetTimeSecondsFAKE()
{
	return GetTime()*m_fFakeSlowMotion*0.001f; //original
}


// should happen per frame - updates per frame member variables
void CTimer::Update(void)
{
	m_fNow =  GetTime();
		m_fNowFAKE = GetTimeFAKE();
	m_fDeltaTime = m_fNow - m_fTimerStart;
		//m_fDeltaTimeFAKE = m_fNowFAKE - m_fTimerStartFAKE;
	m_fDeltaTimeFAKE = m_fDeltaTime * m_fFakeSlowMotion;


	m_fFPS = 1000.0 / m_fDeltaTime;

	m_fTimeForFrame = 1000.f / m_fFPS;

	m_fTimerStart = GetTime();
		//m_fTimerStartFAKE = GetTimeFAKE();
	m_fTimerStartFAKE = m_fTimerStart * m_fFakeSlowMotion;




	m_fTimeSinceStart+=m_fDeltaTime;



	// averaged FPS
	temp_fps+=m_fFPS;

	if (fpsAverageCounter==AVERAGE_FPS_SAMPLES)
	{
		fpsAverageCounter = 0;
		m_fAveragedFPS = temp_fps/AVERAGE_FPS_SAMPLES;
		temp_fps=0.f;

	}

	fpsAverageCounter++;
}
