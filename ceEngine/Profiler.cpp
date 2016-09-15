#include "stdafx.h"
#include "Profiler.h"
//extern IProfiler* g_pProfiler;


#include <vector>
#include <algorithm>
#include "ce.h"
#include "ceLibrary.h"


CProfiler::CProfiler() 
{
};

CProfiler::~CProfiler() 
{
};


int CProfiler::GetNumSteps() const
{
	return m_Steps.size();
}











const char* CProfileStep::GetName() const
{
	return m_pName;
}

double CProfileStep::GetTime() const
{
	return m_dTotal;
}


/////////////////////////////////////////////////////////////

/*
bool CProfiler::tStep::operator <(tStep &t2)
{
	return pStep->GetTime()>t2.pStep->GetTime();
};

bool CProfiler::tStep::operator >(tStep &t2)
{
	return pStep->GetTime()<t2.pStep->GetTime();
};
*/



void CProfiler::Register(CProfileStep* ps)
{
	/*tStep& s=m_Steps.Append();
	s.pStep=&ps;*/
	m_Steps.push_back(ps);
}

// assumes allocated!
void CProfiler::OutPutInfoText(char* pText)
{
	char* pTravel = pText;

	char entry[100];


	PROFILE_STEP_VECTOR_I it = m_Steps.begin();

	while (it != m_Steps.end())
	{
		sprintf(entry,"%s:%1.2f\n",(*it)->GetName(),(*it)->GetTime());
		memcpy(pTravel,entry,sizeof(char) * strlen(entry));
		pTravel+=strlen(entry);
		it++;
	}

	(*pTravel) = NULL;

}


 void CProfiler::Sort()
{
	//m_Steps.QuickSort();
	std::sort(m_Steps.begin(), m_Steps.end(), tProfileStepSmaller());
}

 void CProfiler::Flush()
{
	for (int i=0;i<m_Steps.size();i++)
	{
		m_Steps[i]->m_dTotal=0.0f;
	}
}


bool CProfiler::tProfileStepSmaller::operator ()(CProfileStep*& a, CProfileStep*& b)
{
	return a->GetTime() > b->GetTime();
}
  	  	  	  	 
  	  	  	  	 
