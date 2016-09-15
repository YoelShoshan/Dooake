#ifndef YOEL_SMART_POINTER_H
#define YOEL_SMART_POINTER_H

#pragma once

#include <Windows.h>

#include <assert.h>
#include <crtdbg.h>


template <class T>
class CSmartPointer
{
public:

	////////////////
	// Constructers
	////////////////

	CSmartPointer()
	{	
		m_pObject=NULL; 
	};

	CSmartPointer(T* pObject)
	{	
		m_pObject=NULL; 
		(*this) = pObject;
	};


	CSmartPointer(const CSmartPointer<T>& sp)
	{
		m_pObject=NULL; 
		(*this) = sp;
	};

	////////////////
	// Destructer
	////////////////

	~CSmartPointer()
	{
		ReleasePointer();
	};

	////////////////////////////////////////////////////////
	// ReleasePointer makes sure that (if it's right time)
	// that (*m_pObject) will be deleted AND point to NULL
	///////////////////////////////////////////////////////

	void ReleasePointer(void)
	{
		if (m_pObject) 
		{ 
			m_pObject->Dec(); 
			m_pObject=NULL;
		} 
	}

	////////////////////////////////////////////////////////
	// Convertions
	///////////////////////////////////////////////////////

	//operator T*() const {return m_pObject;};

	////////////////////////////////////////////////////////
	// Assignmens
	///////////////////////////////////////////////////////

	CSmartPointer& operator= (T* pObject)
	{		
		ReleasePointer();

		//assert(_CrtCheckMemory());
		if (pObject) 
			{ 
				//assert(_CrtCheckMemory());
				pObject->Inc(); 
				//assert(_CrtCheckMemory());
				m_pObject=pObject; 
				
			} 
		else 
			m_pObject=NULL; 

	return *this; 
	}

	inline CSmartPointer<T>& operator = (const CSmartPointer<T>& sp)
	{
		*this=sp.m_pObject;
		//m_pObject = sp.m_pObject;
		//assert(_CrtCheckMemory());
		return *this;
	}

	/*bool IsNull(void) const
	{
		if (m_pObject==NULL)
			return true;

		return false;
	}*/

	////////////////////////////////////////////////////////
	// Operators mimicing regular pointers
	///////////////////////////////////////////////////////

	/* Dereferencing operator */
	T& const operator *        () const { return (*m_pObject); }

	/* Member access operator */
	T* const operator ->       () const { return (m_pObject); }

	/* Conversion operators */
	//operator T*       () const { return (m_pObject); }
	operator const T* () const { return (m_pObject); }

	bool operator == (CSmartPointer<T>& to) const 
	{ 
		  return (m_pObject == to.m_pObject);
	}

	bool operator != (CSmartPointer<T>& to) const 
	{ 
		  return (m_pObject != to.m_pObject);
	}


	/* boolean test for NULL */
	operator bool     () const { return (m_pObject)!=NULL; }

	/** May cause memory leaks if pointer is modified. */
	T** operator &       ()       { return &m_pObject; }

private:
	// the shared object
	T* m_pObject;
};




#endif // YOEL_SMART_POINTER_H