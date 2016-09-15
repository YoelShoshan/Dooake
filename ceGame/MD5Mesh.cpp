#include "stdafx.h"
#include "MD5Mesh.h"

#include "ceParser.h"

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "math.h"



//#include "crtdbg.h"

#include "assert.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;


//#include "MaterialManager.h"


CMD5Mesh::CMD5Mesh()
{
	m_pJoints = NULL;
	m_iJointsNum = 0;
	m_pSubMeshes=NULL;
	m_iSubMeshesNum = 0;

	for (int i=0;i<MD5_MESH_CHANNELS_NUM;i++)
	{
		m_ppChannels[i] = NULL;
		m_ChannelsIndicesNum[i] = 0;
	}

}
CMD5Mesh::~CMD5Mesh()
{
	SafeDeleteArray(m_pJoints);
	SafeDeleteArray(m_pSubMeshes);



}

bool CMD5Mesh::Load(const char* pchFileName/*,const CVec3f& vScale*/)
{
	char message[300];
	sprintf(message,"CMD5Mesh::Load loading %s md5mesh file...",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","000099",true,LOG_MESSAGE_INFORMATIVE);

	FILE* pFile;
	if ((pFile=fopen (pchFileName, "rb"))== NULL)
	{		
		g_pLogFile->OutPutPlainText("CMD5Mesh::Create opening of MD5Mesh file failed!",LOG_MESSAGE_ERROR);
		return false;
	}

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
		if (strcmp(currentWord,"numJoints")==0)
			break;

		par.NextWord_IC();	
	}

	par.NextWord_IC();	
	par.FillCurrentWord(currentWord);

	m_iJointsNum = atoi(currentWord);

	m_pJoints = new tMD5Joint[m_iJointsNum];

	par.NextWord_IC();	
	par.NextWord_IC();	
	par.FillCurrentWord(currentWord);

	m_iSubMeshesNum = atoi(currentWord);

	//tMD5SubMesh** m_ppSubMeshes;
	m_pSubMeshes = new tMD5SubMesh[m_iSubMeshesNum];
		
	par.NextWord_IC(); // joints
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
			//m_pJoints[iJointsRead].iParentIndex = 17;
		}
		else
		{
			par.FillCurrentWord(currentWord);

			m_pJoints[iJointsRead].pParent = &m_pJoints[atoi(currentWord)];
			m_pJoints[iJointsRead].iParentIndex = atoi(currentWord);
		}

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
/*glMatrixMode (GL_MODELVIEW);										
	glLoadIdentity ();	*/
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


		// Read in the position
		par.NextWord_IC();  // move to ')'


		par.NextWord_IC();
		
		iJointsRead++;
	}

	int iSubMeshesRead=0;

while (iSubMeshesRead<m_iSubMeshesNum)
{

	


	par.NextWord_IC(); // "mesh"

	par.NextWord_IC(); // {

	par.NextWord_IC(); // "shader"

	par.NextWord_IC(); // shader name

	//par.FillCurrentWord(currentWord); // i need to use this in the future
	par.FillCurrentQuetuePhrase(currentWord);


	
	/*m_pSubMeshes[iSubMeshesRead].pMaterial = new CMaterial();
	m_pSubMeshes[iSubMeshesRead].pMaterial->Load(currentWord);*/

	m_pSubMeshes[iSubMeshesRead].spMaterial = g_pRendererBack->LoadMaterial(currentWord);
	

	par.NextWord_IC(); // "numverts"

	par.NextWord_IC();
	par.FillCurrentWord(currentWord); // i need to use this in the future
	m_pSubMeshes[iSubMeshesRead].iVertsNum = atoi(currentWord);

	tMD5Vertex* pVerts = new tMD5Vertex[m_pSubMeshes[iSubMeshesRead].iVertsNum];
	CVec2f*     pTexCoords = new CVec2f[m_pSubMeshes[iSubMeshesRead].iVertsNum];

		

	int iVertsRead=0;

	while (iVertsRead<m_pSubMeshes[iSubMeshesRead].iVertsNum)
	{
		par.NextWord_IC(); // skip "vert"
		par.NextWord_IC(); // skip vert index
		par.NextWord_IC(); // skip '('

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		pVerts[iVertsRead].tc.v[0] = atof(currentWord);
		pTexCoords[iVertsRead].v[0]=atof(currentWord);

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);

		pVerts[iVertsRead].tc.v[1] =atof(currentWord);
		pTexCoords[iVertsRead].v[1]=atof(currentWord);
		
		// for some reason i have flipped s/t coordinate space for gl and d3d...
/*			if (g_pGraphicsAPI)
				if (g_pGraphicsAPI->GetHandness()==API_RIGHT_HANDED)
				{
					//pVerts[iVertsRead].tc.v[1] = 1.f - pVerts[iVertsRead].tc.v[1];
					pTexCoords[iVertsRead].v[1] = 1.f - pTexCoords[iVertsRead].v[1];
				}*/

		par.NextWord_IC(); // skip ')'

		par.NextWord_IC(); // read weights start index
		par.FillCurrentWord(currentWord);
		pVerts[iVertsRead].iWeightsIndex = atoi(currentWord);

		par.NextWord_IC(); // read weights index count
		par.FillCurrentWord(currentWord);
		pVerts[iVertsRead].iWeightsCount = atoi(currentWord);
		
		iVertsRead++;
	}

	m_pSubMeshes[iSubMeshesRead].pVerts = pVerts;
	m_pSubMeshes[iSubMeshesRead].pTexCoords = pTexCoords;


	/////////////////
	// read indices
	/////////////////

	par.NextWord_IC(); // skip "numTris"
	par.NextWord_IC();
	par.FillCurrentWord(currentWord);
	m_pSubMeshes[iSubMeshesRead].iIndicesNum= atoi(currentWord)*3;

	m_pSubMeshes[iSubMeshesRead].puiIndices = new unsigned int[m_pSubMeshes[iSubMeshesRead].iIndicesNum];


	int iTrisRead=0;
	int iIndexPosition;

	while (iTrisRead<m_pSubMeshes[iSubMeshesRead].iIndicesNum /3)
	{
		par.NextWord_IC(); // skip "tri"
		par.NextWord_IC(); // skip tri index

		// i'm changing the order for TBN to come out 

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].puiIndices[iTrisRead*3+2] = atoi(currentWord);

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].puiIndices[iTrisRead*3+1] = atoi(currentWord);

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].puiIndices[iTrisRead*3+0] = atoi(currentWord);

		iTrisRead++;
	}

	///////////////////////////
	// read weights
	///////////////////////////

	par.NextWord_IC(); // skip "numweights"

	par.NextWord_IC();
	par.FillCurrentWord(currentWord);
	m_pSubMeshes[iSubMeshesRead].iWeightsNum = atoi(currentWord);
	m_pSubMeshes[iSubMeshesRead].pWeights = new tMD5Weight[m_pSubMeshes[iSubMeshesRead].iWeightsNum];


	int iWeightsRead=0;

	while (iWeightsRead<m_pSubMeshes[iSubMeshesRead].iWeightsNum)
	{
		par.NextWord_IC(); // skip "weight"
		par.NextWord_IC(); // skip weight index

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].pWeights[iWeightsRead].iJointIndex = atoi(currentWord);


		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].pWeights[iWeightsRead].fBias = atof(currentWord);

		par.NextWord_IC(); // skip '('

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].pWeights[iWeightsRead].vPosOffest.v[0] = atof(currentWord);

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].pWeights[iWeightsRead].vPosOffest.v[1] = atof(currentWord);

		par.NextWord_IC();
		par.FillCurrentWord(currentWord);
		m_pSubMeshes[iSubMeshesRead].pWeights[iWeightsRead].vPosOffest.v[2] = atof(currentWord);

		par.NextWord_IC(); // skip ')'

		iWeightsRead++;
	}

	par.NextWord_IC(); // skip '}'

	iSubMeshesRead++;
}


	sprintf(message,"////CMD5Mesh::Load loading of %s md5mesh file Finished.",pchFileName);
	g_pLogFile->OutPutPlainText(message,"4","000099",true,LOG_MESSAGE_INFORMATIVE);

	return true;
}

// should only be used in per-processing,
// if i want it to work in realtime, i need to prepare some set or sth to reduce search time


void  CMD5Mesh::ConvertTempChannelsSetsIntoIndexArrays(void)
{
	for (int c=0;c<MD5_MESH_CHANNELS_NUM;c++)
	{
		m_ChannelsIndicesNum[c] = m_TempChannels[c].size();

		m_ppChannels[c] = new int [ m_ChannelsIndicesNum[c]];

		int i=0;
		//for (int i=0;i<m_TempChannels[c].size();i++)
		for (std::set<int>::iterator it = m_TempChannels[c].begin() ; 
			it!= m_TempChannels[c].end() ; ++it)
		{

			m_ppChannels[c][i] = (*it);
			i++;
		}


		m_TempChannels[c].clear();
	}

}

bool CMD5Mesh::RemoveJointsFromChannel(const char* pchName,int iChannelID,bool bRecurseDescendents)
{
		int iInsertJointNum = FindJointIndex(pchName);

	if (iInsertJointNum < 0)
	{
		char message[400];
		sprintf(message,"////CMD5Mesh::RemoveJointsFromChannel - joint:%s not found! ignored.",pchName);
		g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);

		return false;
	}

	m_TempChannels[iChannelID].erase(iInsertJointNum);


	if (!bRecurseDescendents)	
	{
		return true;
	}
	

	std::set<int> descendents = GetJointDescendents(iInsertJointNum);

	for (std::set<int>::iterator it=descendents.begin(); it!=descendents.end(); ++it)
	{
		m_TempChannels[iChannelID].erase((*it));
	}

	int iTemp = m_TempChannels[iChannelID].size();
	
	return true;

}

const int*  CMD5Mesh::GetChannelJointsIndices(int iChannelNum) const
{
	return m_ppChannels[iChannelNum];
}

int        CMD5Mesh::GetChannelJointsIndicesNum(int iChannelNum) const
{
	return m_ChannelsIndicesNum[iChannelNum];

}

bool CMD5Mesh::AddJointsToChannel(const char* pchName,int iChannelID,bool bRecurseDescendents)
{
	int iInsertJointNum = FindJointIndex(pchName);

	if (iInsertJointNum < 0)
	{
		char message[400];
		sprintf(message,"////CMD5Mesh::AddJointsToChannel - joint:%s not found! ignored.",pchName);
		g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);

		return false;
	}

	m_TempChannels[iChannelID].insert(iInsertJointNum);


	if (!bRecurseDescendents)	
	{
		return true;
	}
	

	std::set<int> descendents = GetJointDescendents(iInsertJointNum);

	for (std::set<int>::iterator it=descendents.begin(); it!=descendents.end(); ++it)
	{
		m_TempChannels[iChannelID].insert((*it));
	}

	int iTemp = m_TempChannels[iChannelID].size();
	
	return true;
	


}


int   CMD5Mesh::FindJointIndex(const char* pchName)
{
	for (int i=0;i<m_iJointsNum;i++)
	{
		if (strcmp(m_pJoints[i].pchName,pchName)==0)
			return i;
	}

	return -1;
}

std::set<int> CMD5Mesh::GetJointDescendents(int iIndexNum)
{
	std::set<int> ans;
    // no need to recurse - comment from d3 sdk:

	// all children of the joint should follow it in the list.
	// once we reach a joint without a parent or with a parent
	// who is earlier in the list than the specified joint, then
	// we've gone through all it's children.
	
	 for(int i = iIndexNum + 1; i < m_iJointsNum; i++ )
	 {
		 if (m_pJoints[i].iParentIndex < iIndexNum)
			 break;

		 ans.insert(i);
	 }


	return ans;
}

/*
void CMD5Mesh::DebugGLRenderJoints(void)
{
	glBegin(GL_LINES);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_TEXTURE);
	
	
	for (int i=0;i<m_iJointsNum;i++)
	{
		if (m_pJoints[i].pParent)
		{
			glColor3f(1.f,0.f,0.f);
			glVertex3fv(m_pJoints[i].pos.v);
			glVertex3fv(m_pJoints[i].pParent->pos.v);
		}

	}
	glEnd();
}

void CMD5Mesh::DebugGLRenderMesh(void)
{
	static CVec3f** ppVerts = NULL;
	
	if (ppVerts==NULL)
	{
		ppVerts = new CVec3f*[m_iSubMeshesNum];
		for (int i=0;i<m_iSubMeshesNum;i++)
			ppVerts[i] = new CVec3f[m_pSubMeshes[i].iVertsNum];
	}


	int iJointIndex;

	float fMatrix[16];
	CMatrix rotTemp;
	CMatrix rot;
	
	CVec3f vTempPos;

	const tMD5Joint* pJoint;	
	const tMD5Weight* pWeight;

for (int iSubMeshNum=0;iSubMeshNum<m_iSubMeshesNum;iSubMeshNum++)
{
	
	for (int i=0;i<m_pSubMeshes[iSubMeshNum].iVertsNum;i++)
	{				
		ppVerts[iSubMeshNum][i].Set(0.f,0.f,0.f);

		for (int j=m_pSubMeshes[iSubMeshNum].pVerts[i].iWeightsIndex;j<m_pSubMeshes[iSubMeshNum].pVerts[i].iWeightsIndex+m_pSubMeshes[iSubMeshNum].pVerts[i].iWeightsCount;j++)
		{
			pWeight = &m_pSubMeshes[iSubMeshNum].pWeights[j];
			iJointIndex = pWeight->iJointIndex;		
			pJoint = &m_pJoints[iJointIndex];


			pJoint->rot.ReturnMatrix(fMatrix);
			rot.Set(fMatrix);	

			vTempPos = rot * pWeight->vPosOffest;
			
			ppVerts[iSubMeshNum][i]+=( pJoint->pos+	vTempPos ) * pWeight->fBias;
		}
	}


	glBegin(GL_TRIANGLES);
	glDisableClientState(GL_COLOR_ARRAY);
	glColor3f(1.f,1.f,1.f);

for (int j=0;j<m_iSubMeshesNum;j++)
{
	

	for (int i=0;i<m_pSubMeshes[j].iIndicesNum;i++)
	{
		if (i%3==0)
			glColor3f(1.f,0.f,0.f);
		else
		if (i%3==1)
			glColor3f(0.f,1.f,0.f);
		else
			glColor3f(0.f,0.f,1.f);
		glTexCoord2fv(m_pSubMeshes[j].pVerts[m_pSubMeshes[j].puiIndices[i]].tc.v);
		glVertex3fv(ppVerts[j][m_pSubMeshes[j].puiIndices[i]].v);

	}
}
	
	glEnd();
}

}
*/
