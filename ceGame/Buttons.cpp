#include "stdafx.h"
#include "Buttons.h"


#include "TagLogics.h"
extern CTagLogics g_TagLogics;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "ce.h"

#include <string>
#include <fstream>
#include "Input.h"
#include "ceParser.h"



CButtons::CButtons()
{
	m_iButtonsNum = 0;

}
CButtons::~CButtons()
{

}

bool CButtons::Add(int iButtonNum,tButtonBind* pAddMe)
{
	if (!pAddMe)
	{
		g_pLogFile->OutPutPlainText("CButtons::Add  bind failed because given bind was NULL",LOG_MESSAGE_ERROR);
		//BREAKPOINT(1);
		return false;
	}
	
	BUTTON_BINDS_MAP_IT it;

	it = m_ButtonBinds.find(iButtonNum);

	if (it!=m_ButtonBinds.end()) // it's already in the map!
	{
		g_pLogFile->OutPutPlainText("CButtons::Add  bind failed because tagID already existed in map",LOG_MESSAGE_ERROR);
		//BREAKPOINT(1);
		return false;
	}


	m_ButtonBinds[iButtonNum] = pAddMe;

	m_iButtonsNum++;
	return true;

}

bool CButtons::IsActive(int iButtonNum)
{
	BUTTON_BINDS_MAP_IT it;

	it = m_ButtonBinds.find(iButtonNum);

	if (it==m_ButtonBinds.end()) 
	{
		g_pLogFile->OutPutPlainText("CButtons::IsActive checking non bound button num!",LOG_MESSAGE_ERROR);
		return false; 
	}

	if(st_Input->KeyPressed(it->second->key))
		return true;

	if (g_TagLogics.IsTagActive(it->second->i64TagID))
		return true;

	return false;

}

bool CButtons::SetConfig(const char* pchFileName)
{
	char s1[264],s2[264],s3[64];

	//streamoff sStreamOff;
			
	std::ifstream configFile;

	configFile.open(pchFileName);


	if(configFile.fail())
	{	
		g_pLogFile->OutPutPlainText("CButtons::SetConfig opening of binds config file failed!",LOG_MESSAGE_ERROR);
		return false; 						
	}	

	int num_args =0;
	//int iCounter = 0;

	std::string t_CurrentLine;

	int iButtonsReadNum = 0;
	int iButtonsNum = 0;

	while(std::getline(configFile, t_CurrentLine))
	{
		if (t_CurrentLine[0]=='/')  // double check...
			continue;

		num_args=sscanf(t_CurrentLine.c_str(),"%s %s %s",s1,s2,s3);

		if (num_args<=0) 
		{
			s1[0]=s2[0]=s3[0]=NULL;
			continue;
		}
	
	
		if (s1[0]=='/')  // only supporting c comments for now
		{
			s1[0]=s2[0]=s3[0]=NULL;
			continue;
		}


		if (strcmp(s1,"buttonsnum")==0)     // nomipmap
		{
			iButtonsNum = atoi(s2);
			continue;
		}

		if (num_args<3) // not enough arguments for bind!
		{
			s1[0]=s2[0]=s3[0]=NULL;
			continue;
		}

		// if we are here then we are at an actual bind

		tButtonBind* pBind = new tButtonBind();
		pBind->key = atoi(s3);
		//pBind->tagID = atoi(s2);
		pBind->i64TagID = CParser::Hex16StringToINT64(&s2[2]);
		if (!Add(atoi(s1),pBind))
		{
			SafeDelete(pBind);
			g_pLogFile->OutPutPlainText("CButtons::SetConfig failed adding a bind from config file.",LOG_MESSAGE_ERROR);
		}
		else
			iButtonsReadNum++;


		sTag* pTag = new sTag();
		pTag->i64TagID    = pBind->i64TagID;
		if (!g_TagLogics.Add(pTag->i64TagID,pTag))
		{
			SafeDelete(pTag);

			g_pLogFile->OutPutPlainText("CButtons::SetConfig failed adding a Tag to TagLogics.",LOG_MESSAGE_ERROR);
		}
				

		if (iButtonsReadNum==iButtonsNum)
			break;
	}

	return true;
};