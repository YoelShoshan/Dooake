#include "stdafx.h"
#include "MD5Anim.h"


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "ceParser.h"

#include "AnimationController.h"



CMD5Anim::CMD5Anim()
{
	m_iFramesNum = 0;
	m_iJointsNum = 0;

	m_pJoints = NULL;
	m_pFrames = NULL;

	m_bLoopable = false;

	m_iLegsPriority = -1;
	m_iTorsoPriority = -1;

}

CMD5Anim::~CMD5Anim()
{
	SafeDeleteArray(m_pJoints);
	SafeDeleteArray(m_pFrames);
}

void  CMD5Anim::SetLoopable(bool bLoopable)
{
	m_bLoopable = bLoopable;
}

bool CMD5Anim::GetLoopable(void)
{
	return m_bLoopable;
}

void CMD5Anim::SetAniType(DWORD dwType)
{
	m_dwAniType = dwType;
}

DWORD CMD5Anim::GetAniType()
{
	return m_dwAniType;
}

bool CMD5Anim::Load(const char* pchFileName)
{
	char message[300];
	sprintf(message,"CMD5Anim::Load loading %s md5anim file...",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","0000EE",true,LOG_MESSAGE_INFORMATIVE);


	FILE* pFile;
	if ((pFile=fopen (pchFileName, "rb"))== NULL)
	{
		sprintf(message,"CMD5Mesh::Create opening of MD5Anim file [%s] failed!",pchFileName);

		g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);
		return false;
	}

	m_pchName = new char[strlen(pchFileName)+1];
	strcpy(m_pchName,pchFileName);

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iFileLength+1];
	fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength] = NULL;

	char currentWord[500];

	// create a parser containing it
	CParser par(pText,iFileLength+1);

	par.EatWhiteSpace();

	while (!par.ReachedEnd())
	{
		par.FillCurrentWord(currentWord);
		if (strcmp(currentWord,"numFrames")==0)
			break;

		par.NextWord_IC();	
	}

	par.NextWord_IC();
	par.FillCurrentWord(currentWord);


	m_iFramesNum = atoi(currentWord);

	m_pFrames = new tMD5AnimFrame[m_iFramesNum];

	par.NextWord_IC();
	par.NextWord_IC();
	par.FillCurrentWord(currentWord);

	m_iJointsNum = atoi(currentWord);

	m_pJoints = new tMD5Joint[m_iJointsNum];


	par.NextWord_IC();
	par.NextWord_IC();
	par.FillCurrentWord(currentWord);

	m_fFrameRate = atof(currentWord);

	int iAnimatedComponentsNum;

	par.NextWord_IC();
	par.NextWord_IC();
	par.FillCurrentWord(currentWord);

	iAnimatedComponentsNum = atoi(currentWord);



	par.NextWord_IC(); // hierarchy   <-- joints
	par.NextWord_IC(); // '{"
	
	// read in the joints

	int iJointsRead=0;
	int iCurrentWordLength=0;
	const char* traveler;

	par.NextWord_IC(); 

	while (iJointsRead<m_iJointsNum)
	{
		//////////////////////////////////////////////////////////////////////////////
		// Read in the joint name 		
		iCurrentWordLength = par.GetCurrentWordLength()-2; // removing the ""
		traveler = par.GetTravel();
				
		m_pJoints[iJointsRead].pchName = new char[iCurrentWordLength+1];

		for (int i=0;i<iCurrentWordLength;i++)
			m_pJoints[iJointsRead].pchName[i] = traveler[1+i];

		m_pJoints[iJointsRead].pchName[iCurrentWordLength] = NULL;
		//////////////////////////////////////////////////////////////////////////////
		// Read in the parent node
		par.NextWord_IC(); 
		par.EatWhiteSpace();
		traveler = par.GetTravel();
		if (traveler[0]=='-')
		{
			m_pJoints[iJointsRead].pParent=NULL;
			m_pJoints[iJointsRead].iParentIndex = -1;
		}
		else
		{
			par.FillCurrentWord(currentWord);

			m_pJoints[iJointsRead].pParent = &m_pJoints[atoi(currentWord)];
			m_pJoints[iJointsRead].iParentIndex = atoi(currentWord);
		}

		//////////////////////////////////////////////////////////////////////////////
		par.NextWord_IC();  // read anim flags
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].dwAnimFlags = atoi(currentWord);

		/*m_pJoints[iJointsRead].chAnimatedElements=0;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_TRANS_X)
			m_pJoints[iJointsRead].chAnimatedElements++;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_TRANS_Y)
			m_pJoints[iJointsRead].chAnimatedElements++;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_TRANS_Z)
			m_pJoints[iJointsRead].chAnimatedElements++;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_ROT_X)
			m_pJoints[iJointsRead].chAnimatedElements++;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_ROT_Y)
			m_pJoints[iJointsRead].chAnimatedElements++;
		if (m_pJoints[iJointsRead].dwAnimFlags & ANIMATED_NODE_ROT_Z)
			m_pJoints[iJointsRead].chAnimatedElements++;*/




		par.NextWord_IC();  // read start index
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].iStartIndex = atoi(currentWord);
		


		par.NextWord_IC();  // read start index
		
		
		iJointsRead++;
	}


	par.NextWord_IC();  // skip '}'



	

	while (!par.ReachedEnd())
	{
		par.FillCurrentWord(currentWord);
		if (strcmp(currentWord,"baseframe")==0)
			break;

		par.NextWord_IC();	
	}



	par.NextWord_IC(); // skip '{'

	///////////////////////////////////////////////////////
	// Read "baseframe"
	///////////////////////////////////////////////////////

	iJointsRead=0;

	while (iJointsRead<m_iJointsNum)
	{
		//////////////////////////////////////////////////////////////////////////////
		// Read in the position
		par.NextWord_IC();  // move to '('

		par.NextWord_IC(); // move to x value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].pos.v[0] = atof(currentWord);

		par.NextWord_IC(); // move to y value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].pos.v[1] = atof(currentWord);

		par.NextWord_IC(); // move to z value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].pos.v[2] = atof(currentWord);

		// Read in the position
		par.NextWord_IC();  // move to ')'

		//////////////////////////////////////////////////////////////////////////////
		// Read in the rotational quat
		// those are units quaternions, so we can compute the w ourselves

		// Read in the position
		par.NextWord_IC();  // move to '('

		par.NextWord_IC(); // move to x value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].rot.x = atof(currentWord);

		par.NextWord_IC(); // move to y value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].rot.y = atof(currentWord);

		par.NextWord_IC(); // move to z value
		par.FillCurrentWord(currentWord);
		m_pJoints[iJointsRead].rot.z = atof(currentWord);

		m_pJoints[iJointsRead].rot.ComputeW();


		par.NextWord_IC(); // move to z value

		iJointsRead++;
	}

	par.NextWord_IC(); // skip "}"
	
	int iFramesRead = 0;

	while (iFramesRead<m_iFramesNum)
	{
		par.NextWord_IC(); // skip "frame"
		par.NextWord_IC(); // skip frame num
		par.NextWord_IC(); // skip "{"

		m_pFrames[iFramesRead].pData = new float[iAnimatedComponentsNum];

		for (int i=0;i<iAnimatedComponentsNum;i++)
		{
			par.NextWord_IC(); // skip "{"
			par.FillCurrentWord(currentWord);
			m_pFrames[iFramesRead].pData[i] = atof(currentWord);

		}

		par.NextWord_IC(); // skip "}"

		iFramesRead++;
	}



	m_pFrameActions = new tFrameActions[m_iFramesNum];


	sprintf(message,"////CMD5Anim::Load loading of %s md5anim file Finished.",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","0000EE",true,LOG_MESSAGE_INFORMATIVE);


	return true;
}

void CMD5Anim::ReverseFramesOrder(void)
{
	tMD5AnimFrame* pTempFrames;
	pTempFrames = new tMD5AnimFrame[m_iFramesNum];

	for (int i=0;i<m_iFramesNum;i++)
		pTempFrames[i].pData = m_pFrames[i].pData;

	for (int i=0;i<m_iFramesNum;i++)
		m_pFrames[i].pData = pTempFrames[m_iFramesNum-i-1].pData;




}

void CMD5Anim::SetPriority(int iChannel,int iPrio)
{
	if (iChannel == CHANNEL_LEGS)
		m_iLegsPriority = iPrio;
	else if (iChannel == CHANNEL_TORSO)
		m_iTorsoPriority = iPrio;
	else
		BREAKPOINT(1);	
}

int CMD5Anim::GetPriority(int iChannel) const
{
	if (iChannel == CHANNEL_LEGS)
		return m_iLegsPriority;
	else if (iChannel == CHANNEL_TORSO)
		return m_iTorsoPriority;
	else
		BREAKPOINT(1);
}


void CMD5Anim::SetFrameActions(int iFrameIndex,tFrameActions& frameActions)
{

	if (iFrameIndex>=m_iFramesNum)
	{
		char message[300];
		sprintf(message,"CMD5Anim::SetFrameActions ERROR! trying to set frame action for a frame that doesn't exist!");		
		g_pLogFile->OutPutPlainText(message,"4","0000EE",true,LOG_MESSAGE_WARNING_LVL_3);
		sprintf(message,"Trying to set frame:[%d] while there are only [%d] frames in this animation!",iFrameIndex,m_iFramesNum);		
		g_pLogFile->OutPutPlainText(message,"4","0000EE",true,LOG_MESSAGE_WARNING_LVL_3);
		return;
	}
	

	BREAKPOINT(frameActions.iActionsNum < 0);

	memcpy(&m_pFrameActions[iFrameIndex],&frameActions,sizeof(tFrameActions));
}

const tFrameActions* CMD5Anim::GetFramesActions(int iFrameIndex)
{
	if (m_pFrameActions && m_pFrameActions[iFrameIndex].iActionsNum)
	{
		// debug

		BREAKPOINT(m_pFrameActions[iFrameIndex].iActionsNum < 0);

		return &m_pFrameActions[iFrameIndex];
	}

	return NULL;
}

// -1 means not found
int CMD5Anim::GetJointsIndex(const char* pcName) const
{
	for (int i=0;i<m_iJointsNum;i++)
	{
		if (strcmp(m_pJoints[i].pchName,pcName)==0)
			return i;
	}

	return -1; // not found
}