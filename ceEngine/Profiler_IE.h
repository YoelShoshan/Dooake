#ifndef CRIMSON_PROFILER_INTERFACE_H
#define CRIMSON_PROFILER_INTERFACE_H

#pragma once


class CProfileStep;
class IProfiler
{
public:
	IProfiler() {};
	virtual ~IProfiler() {};

	virtual void Register(CProfileStep* ps) = 0;
	virtual int GetNumSteps() const = 0;
	virtual void Sort() = 0;
	virtual void Flush() = 0;
	virtual void OutPutInfoText(char* pText) = 0;
};


#endif