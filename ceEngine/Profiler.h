#ifndef PROFILER_H
#define PROFILER_H

#pragma once

#include "Timer_IE.h"
#include <vector>

#include "CTimer.h"
extern ITimer* g_pTimer;

#include "ce.h"

#include "Profiler_IE.h"
extern IProfiler* g_pProfiler;
//#define st_Profiler  CProfiler::Inst()

class CProfileStep;
class CProfiler : public IProfiler
//class CProfiler 
{
	friend class CProfileStep;
public:
	CProfiler();
	~CProfiler();

	void Register(CProfileStep* ps);
	int GetNumSteps() const;
	//const CProfileStep& GetStep(int i) const;
	void Sort();
	void Flush();
	void OutPutInfoText(char* pText);

	// Ascending date sorting function
	
     struct tProfileStepSmaller 
     { 
          bool operator()(CProfileStep*& a, CProfileStep*& b) ;
     };

protected:


	typedef std::vector<CProfileStep*>::iterator PROFILE_STEP_VECTOR_I;

	std::vector<CProfileStep*> m_Steps;
};


class CProfileStep
{
	friend class CProfiler;
public:
	CProfileStep::CProfileStep(const char* pName)
	{
		m_pName=new char[strlen(pName)+1];
		strcpy(m_pName,pName);
		g_pProfiler->Register(this);	
	}
	/*CProfileStep::CProfileStep(const char* pName)
	{
		m_pName=new char[strlen(pName)+1];
		strcpy(m_pName,pName);
		st_Profiler->Register(this);
	}*/

	CProfileStep::~CProfileStep()
	{
		SafeDeleteArray(m_pName);
	}

	void CProfileStep::Begin()
	{
		//m_dStart=CProfiler::Inst()->m_Timer.GetElapsedTime();
		m_dStart = g_pTimer->GetTime_FAST();
	}

	void CProfileStep::End()
	{
		//m_dTotal+=CProfiler::Inst()->m_Timer.GetElapsedTime()-m_dStart;
		m_dTotal+=g_pTimer->GetTime_FAST()-m_dStart;
	}
	const char* GetName() const;
	double GetTime() const;
protected:
	char* m_pName;
	double m_dStart;
	double m_dTotal;
};





#endif

  	  	  	  	 
  	  	  	  	 
