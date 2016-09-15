#ifndef YOEL_REFERENCE_COUNT_H
#define YOEL_REFERENCE_COUNT_H

#pragma once

#include "stdafx.h"

#include <assert.h>
///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

class CReferenceCount
{
public:
			
	CReferenceCount()
	{
		m_iRefCount=0;
	};
	virtual ~CReferenceCount()
	{
		// In all cases, (REGULAR or MANAGER) m_iRefCount should be equal to zero here.
		assert(m_iRefCount==0);

		//BREAKPOINT(m_iRefCount!=0);		
	};

	
	void Inc(void)
	{
		m_iRefCount++;
	};
	

	void Dec(void)
	{
		m_iRefCount--;

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// After decremented, the meaning of the m_iRefCount is different for:
		//
		// REGULAR - 
		//         m_iRefCount>0     means that this object still needs to stay alive
		//         m_iRefCount==0    means that this object should be killed
		//
		// MANAGER =
		//         m_iRefCount>1     means that this object still needs to stay alive
		//         m_iRefCount==1    means that this object should be killed (it ONLY exists in the manager)
		//         m_iRefCount==0    should be ignored because it only occures because of the destruction of
		//                           the last smartpointer, who is only used by the queue.
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		assert (m_iRefCount>=0);
		//BREAKPOINT (m_iRefCount<0);
	
		// REGULAR Version
		//if (m_iRefCount==0)		
		// MANAGER Version
		if (m_iRefCount==0)
		{
			/////////////////////////
			/*gex_iDebugSPDeleteCalls++;*/
			/////////////////////////
			delete this;						
			//delete (void*) this;						

			int k=13;
		}
		
	};
	
	const int GetCount() 
	{
		return m_iRefCount; 
	}; 

private:
// NOT IMPLEMENTED ON PURPOSE!!!!!!! so that no one would use that...
//CReferenceCount& operator = (const CReferenceCount& ReferenceCount); 
int m_iRefCount;

};


#endif //YOEL_REFERENCE_COUNT_H