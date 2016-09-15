#ifndef CRIMSON_TIMER_INTERFACE_H
#define CRIMSON_TIMER_INTERFACE_H

#pragma once



class ITimer
{
public:
	ITimer() {};
	virtual ~ITimer() {};

	virtual void TimerInit() = 0;
    virtual double GetTime() = 0;
	virtual double GetTimeSeconds() = 0;
	

	////////////////////////////////////////////////////////////////////
	// uses accurate query performance counter
	virtual double GetTimeFAKE() = 0;
	virtual double GetTimeSecondsFAKE() = 0;

	virtual void  SetFakeSlowMotion(double fSet)  = 0;
	virtual double GetFakeSlowMotion(void)  = 0;

	
	virtual double GetDeltaTime()  = 0	;
	virtual double GetDeltaTimeFAKE()  = 0;
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// uses fast but less accurate pentium counter
	// (can be used inside small functions)

	virtual double GetTime_FAST() = 0;

	

	virtual void Update(void) = 0;

	virtual double GetFPS() = 0;

	virtual double GetAveragedFPS() = 0;

};


#endif