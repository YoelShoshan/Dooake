#ifndef CRIMSON_TIMER_H
#define CRIMSON_TIMER_H



#include "ce.h"
#include "Timer_IE.h"
//#define AVERAGE_FPS_SAMPLES 80.f

#define AVERAGE_FPS_SAMPLES 280.f


class CTimer : public ITimer 
{
public:

	CTimer() {};
	~CTimer() {};

	void TimerInit(void);
    double GetTime();
	double GetTimeSeconds();
	

	////////////////////////////////////////////////////////////////////
	// uses accurate query performance counter
	double GetTimeFAKE();
	double GetTimeSecondsFAKE();

	void  SetFakeSlowMotion(double fSet) {m_fFakeSlowMotion=fSet;};
	double GetFakeSlowMotion(void) {return m_fFakeSlowMotion;};

	
	//double GetDeltaTime(void) {return m_fDeltaTime;};	

	double GetDeltaTime(void) {return GetDeltaTimeFAKE();};	

	double GetDeltaTimeFAKE(void) {return m_fDeltaTimeFAKE;};
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// uses fast but less accurate pentium counter
	// (can be used inside small functions)

	double GetTime_FAST();



	__forceinline  DWORD GetPentiumCounter()
	{
		__asm
		{
			xor   eax,eax	          // VC won't realize that eax is modified w/out this
									//   instruction to modify the val.
									//   Problem shows up in release mode builds
			_emit 0x0F		          // Pentium high-freq counter to edx;eax
			_emit 0x31		          // only care about low 32 bits in eax
	
			xor   edx,edx	          // so VC gets that edx is modified
		}
	}

	__forceinline double GetCPUSpeed()
{
	/*
	RdTSC:
	It's the Pentium instruction "ReaD Time Stamp Counter". It measures the
	number of clock cycles that have passed since the processor was reset, as a
	64-bit number. That's what the <CODE>_emit lines do.*/
	#define RdTSC __asm _emit 0x0f __asm _emit 0x31

	// variables for the clock-cycles:
	__int64 cyclesStart = 0, cyclesStop = 0;
	// variables for the High-Res Preformance Counter:
	unsigned __int64 nCtr = 0, nFreq = 0, nCtrStop = 0;


	    // retrieve performance-counter frequency per second:
	    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &nFreq)) return 0;

		// retrieve the current value of the performance counter:
		QueryPerformanceCounter((LARGE_INTEGER *) &nCtrStop);

	    // add the frequency to the counter-value:
	    nCtrStop += nFreq;
	

		_asm
			{// retrieve the clock-cycles for the start value:
				RdTSC
				mov DWORD PTR cyclesStart, eax
	            mov DWORD PTR [cyclesStart + 4], edx
			}

			do{
			// retrieve the value of the performance counter
			// until 1 sec has gone by:
				QueryPerformanceCounter((LARGE_INTEGER *) &nCtr);
			}while (nCtr < nCtrStop);

		_asm
			{// retrieve again the clock-cycles after 1 sec. has gone by:
				RdTSC
				mov DWORD PTR cyclesStop, eax
				mov DWORD PTR [cyclesStop + 4], edx
		    }

	// stop-start is speed in Hz divided by 1,000,000 is speed in MHz
	return    ((double)cyclesStop-(double)cyclesStart) / 1000000.0f;
	}



	void Update(void);

	double GetFPS() { return m_fFPS; };

	double GetAveragedFPS() { return m_fAveragedFPS;};
	

	double m_fTimerStart, m_fFPS, m_fAveragedFPS,m_fDeltaTime, m_fNow;
	double m_fTimerStartFAKE,m_fDeltaTimeFAKE,m_fNowFAKE;
	double m_fTimeForFrame;
	double m_fTotalTimeFAKE;

	double temp_fps;
	int    fpsAverageCounter;
	

	// seperating game logic from rendering

	double m_fTimeSinceStart;

	

private:

	double cpu_speed;
	double cpu_speed_inv;

	__int64       frequency_perf;									// Timer Frequency
	__int64       frequency_pentium;									// Timer Frequency
	//double         resolution;									// Timer Resolution
	double         reso_perf;									// Timer Resolution
	double         reso_pentiumCounter;
	DWORD        pentium_timer_start;
	double       correctionfactor;
	unsigned long mm_timer_start;								// Multimedia Timer Start Value
	unsigned long mm_timer_elapsed;							// Multimedia Timer Elapsed Time
	bool			performance_timer;							// Using The Performance Timer?
	__int64       performance_timer_start;					// Performance Timer Start Value
	__int64       performance_timer_elapsed;					// Performance Timer Elapsed Time

	double m_fFakeSlowMotion;      // Result = realtime / fFakeSlowMotion

};													// Structure Is Named timer



#endif // CRIMSON_TIMER_H
