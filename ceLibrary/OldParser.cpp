#include "stdafx.h"
#include "OldParser.h"

#include <Windows.h>
//#include "Defines.h"
#include "ce.h"

#include "stdlib.h"
///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

COldParser::COldParser()
{
	m_pStart=NULL;
	m_pTravel=NULL;
	m_iSize=0;


	m_pTemp = new char[1024];
}

COldParser::~COldParser()
{
	SafeDeleteArray(m_pTemp);
}

void COldParser::SetAll(char* pSet)
{
	m_pStart=m_pTravel=pSet;
	m_iSize=sizeof(pSet)+1;
}

char* COldParser::NextLine(void)
{
	if (m_pTravel[0]==NULL)
	return NULL;

	while (m_pTravel[0]!='\n')
	{
		m_pTravel++;
		if (m_pTravel[0]==NULL)
			return NULL;
	}

	return m_pTravel; // huh? i only put that because not all paths lead to a return value...
}

char* COldParser::NextWord(void)
{
	if (m_pTravel[0]==NULL)
		return NULL;
    
	// finish this word

	while(m_pTravel[0]!=32 &&
		m_pTravel[0]!='\n' &&
		m_pTravel[0]!='\r' &&
		m_pTravel[0]!= 9)
	{
		m_pTravel++;
		if (m_pTravel[0]==NULL)
			return NULL;
			//break;
	}

	//skip white space

	while(m_pTravel[0]==32 ||
		m_pTravel[0]=='\n' ||
		m_pTravel[0]=='\r' ||
		m_pTravel[0]== 9)
	{
		m_pTravel++;
		if (m_pTravel[0]==NULL)
			return NULL;
		//break;
	}

	return m_pTravel;
}

char* COldParser::GoTo(char* find)
{
	if (m_pTravel[0]==NULL)
		return NULL;

	m_pTravel = strstr(m_pTravel,find);

	return m_pTravel;
}

char* COldParser::NextWordIgnoreComments(void)
{
	bool bCommentStarted=false;

	if (m_pTravel[0]==NULL)
		return NULL;

	// finish this word

	while((bCommentStarted==true) || 
		(m_pTravel[0]!=32 &&
		m_pTravel[0]!='\n' &&
		m_pTravel[0]!='\r' &&
		m_pTravel[0]!= 9))
	{
		///////////
		//check if comment
		if (bCommentStarted==false)
		{
			if (m_pTravel[1]!=NULL)
				if (m_pTravel[0]=='/' && m_pTravel[1]=='*')
				{
					bCommentStarted=true;
				}
		}
		else // bCommentStart==true
		{
			if (m_pTravel[1]!=NULL)
				if (m_pTravel[0]=='*' && m_pTravel[1]=='/')
				{
					bCommentStarted=false;
					m_pTravel++;
					m_pTravel++;
					break;
				}
		}
		
		m_pTravel++;
		if (m_pTravel[0]==NULL)
			return NULL;
		//break;
	}

	//skip white space

	while(m_pTravel[0]==32 ||
		m_pTravel[0]=='\n' ||
		m_pTravel[0]=='\r' ||
		m_pTravel[0]== 9)
	{
		m_pTravel++;
		if (m_pTravel[0]==NULL)
			return NULL;
		//break;
	}

	if (m_pTravel[0]!=NULL)
		if (m_pTravel[1]!=NULL)
			if (m_pTravel[0]=='/' && m_pTravel[1]=='*')
			{
				NextWordIgnoreComments();
			}


	// check if the new found word is starting a comment




	return m_pTravel;
	
}

int COldParser::GetInt(void)
{
	int i=0;
	while(m_pTravel[i]!=32 &&
		m_pTravel[i]!='\n' &&
		m_pTravel[i]!='\r' &&
		m_pTravel[i]!= 9)
	{
		m_pTemp[i] = m_pTravel[i];
		i++;
	}
	m_pTemp[i]=NULL;

	return atoi(m_pTemp);
}

float COldParser::GetFloat(void)
{
	int i=0;
	while(m_pTravel[i]!=32 &&
		m_pTravel[i]!='\n' &&
		m_pTravel[i]!='\r' &&
		m_pTravel[i]!= 9 &&
		m_pTravel[i]!= NULL)
	{
		m_pTemp[i] = m_pTravel[i];
		i++;
	}
	m_pTemp[i]=NULL;

	return atof(m_pTemp);
}


// ignore comments etc.
// assumes string is starting at m_pTravel[0]
// NO alloc
void COldParser::GetString(char*& get) // ignore comments etc.
{
	if (m_pTravel[0]==NULL)
	{
		get[0]=NULL;
		return;
	}

	char* pTemp;
	int iCount=0;

	pTemp=m_pTravel;

	while(pTemp)
	{
		if (pTemp[0]==32 ||	pTemp[0]=='\n' ||	pTemp[0]=='\r' ||	pTemp[0]== 9) // if white space
			break;

		if (pTemp[0]==NULL)
			break;

		if (pTemp[1])
			if ((pTemp[0]=='/' && pTemp[1]=='*') || (pTemp[0]=='*' && pTemp[1]=='/'))
				break;

		pTemp++;
		iCount++;        
	}

	if (iCount<1)
	{
		get = NULL;
		return;
	}

	//get = new char[iCount+1];

	memcpy(get,m_pTravel,iCount);
	get[iCount]=NULL;
}

void COldParser::GetStringIgnoreSpace(char*& get)
{
	if (m_pTravel[0]==NULL)
	{
		get[0]=NULL;
		return;
	}



	char* pTemp;
	int iCount=0;

	pTemp=m_pTravel;

	while(pTemp)
	{
		if (pTemp[0]=='\n' ||	pTemp[0]=='\r' ||	pTemp[0]== 9) // if white space
			break;

		if (pTemp[1])
			if ((pTemp[0]=='/' && pTemp[1]=='*') || (pTemp[0]=='*' && pTemp[1]=='/'))
				break;

		pTemp++;
		iCount++;        
	}

	if (iCount<1)
	{
		get = NULL;
		return;
	}

	//get = new char[iCount+1];

	memcpy(get,m_pTravel,iCount);
	get[iCount]=NULL;

}



// ignore comments etc.
// assumes string is starting at m_pTravel[0]
void COldParser::GetStringAndAlloc(char*& get) // ignore comments etc.
{
	char* pTemp;
	int iCount=0;

	pTemp=m_pTravel;
    
	while(pTemp)
	{
		if (pTemp[0]==32 ||	pTemp[0]=='\n' ||	pTemp[0]=='\r' ||	pTemp[0]== 9) // if white space
			break;

		if (pTemp[1])
			if ((pTemp[0]=='/' && pTemp[1]=='*') || (pTemp[0]=='*' && pTemp[1]=='/'))
				break;

		pTemp++;
		iCount++;        
	}

	if (iCount<1)
	{
		get = NULL;
		return;
	}

	get = new char[iCount+1];

	memcpy(get,m_pTravel,iCount);
	get[iCount]=NULL;
}


void COldParser::RemoveParenthesis(char*& rem)
{
	int iSize = strlen(rem);
	memcpy(rem,&rem[1],sizeof(char)*iSize);
	rem[iSize-2]=NULL;
}

bool COldParser::GetNextCurlyBracket(char*& get)
{
	char *pTemp1, *pTemp2;
	pTemp1 = strstr(m_pTravel,"{");
	pTemp2 = strstr(m_pTravel,"}");

	if ((!pTemp1) && (!pTemp2))
	{
		get[0]=NULL;
		return false;
	}

	if (!pTemp2)
	{
		m_pTravel = &pTemp1[1];
		get[0] = m_pTravel[-1];
		get[1] = NULL;
		return true;
	}

	if (!pTemp1)
	{
		m_pTravel = &pTemp2[1];
		get[0] = m_pTravel[-1];
		get[1] = NULL;
		return true;
	}

	if (strlen(pTemp1)>strlen(pTemp2))
		m_pTravel = &pTemp1[1];
	else
		m_pTravel = &pTemp2[1];

	get[0] = m_pTravel[-1];
	get[1] = NULL;

	return true;
	
}