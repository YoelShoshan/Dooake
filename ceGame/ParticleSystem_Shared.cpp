#include "stdafx.h"
#include "ParticleSystem_Shared.h"

#include "RendererFrontEnd.h"
extern CRendererFrontEnd* g_pRendererFront;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "ParticleSystemManager.h"


#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

char CParticleSystem_Shared::currentWord[500];

CParticleSystem_Shared::CParticleSystem_Shared()
{
	m_fParticleSize = 1.5f;
	m_vStartVelocityRange_Min.Set(-10,-10,-10);
	m_vStartVelocityRange_Max.Set(10,10,10);

	m_vStartPositionOffsetRange_Min.Set(-6,-6,-6);
	m_vStartPositionOffsetRange_Min.Set(6,6,6);

	// life time
	m_fLifeTimeRange_Min = 0.15f;
	m_fLifeTimeRange_Max = 0.3f;

	// max particles
	m_uiParticlesMax = 1200;
	//m_uiParticlesMax = 4;

	m_bRotateAroundOrigin = false;

	m_vStartRotateDistance.Set(10.f,10.f,10.f);
	m_fRotateSpeed = 10.f;


	// transform particles with origin
	// translates each particle along with the origin
	m_bTrasnformParticlesWithOrigin = false;

	

	m_vAcceleration.Set(0.f,-30.0f,0.f);
	//m_vAcceleration.Set(0.f,0.f,0.f);

	m_pIB = NULL;
	

	m_bRotateAround[0] = true;
	m_bRotateAround[1] = true;
	m_bRotateAround[2] = true;

	m_pcMaterialName = NULL;

	m_dTotalLifeTime = -1;

	m_bBloom = true;

	m_dTimeTillFullFirstAppearance = 500.0;
}

CParticleSystem_Shared::~CParticleSystem_Shared()
{

	st_ParticleSystemManager->Unregister(m_pcName); // unregister this static model from static-model-manager

}

bool CParticleSystem_Shared::Load(const char* pcFileName)
{
	BREAKPOINT(!pcFileName);

	char temp[750];

	sprintf(temp,"CParticleSystem_Shared::Attempting to load Particle System script [%s].",pcFileName);
	g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_INFORMATIVE);


	m_pcName = new char[strlen(pcFileName)+1];
	strcpy(m_pcName,pcFileName);

	//Init();


	FILE* pFile;
	if ((pFile=fopen (pcFileName, "rb"))== NULL)
	{
		
		sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
		return false;
	}

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iFileLength+1];
	fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength] = NULL;

	char currentWord[500];
	char errorMessage[600];
	
	// create a parser containing it
	CParser par(pText,iFileLength+1);

	//TODO: consider the case that the first word is PS

	par.NextWord_IC(); 
	par.FillCurrentWord(currentWord);

	if(strcmp(currentWord,"PS")==0)
	{
		ParseParticleSystem(par,pcFileName,0,this);
	}
	else
	{
		
		sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
		sprintf(temp,"no particle system defined! (PS)",pcFileName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
		return false;
	}


	InitAll();

	
	sprintf(temp,"CParticleSystem_Shared::Succesfully loaded Particle System script [%s].",pcFileName);
	g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_INFORMATIVE);
	

	return true;
}

void CParticleSystem_Shared::InitAll(void)
{
	Init();
	//for (std::vector<CParticleSystem_Shared*>::iterator it=m_Children.begin();it!=m_Children.end();++it)
	for (std::vector<CParticleSystem_Shared*>::iterator it=m_Children.begin();it!=m_Children.end();++it)
	{
		(*it)->InitAll();
	}

}

bool CParticleSystem_Shared::ParseParticleSystem(CParser& par,const char* pcFileName,int iDepth,CParticleSystem_Shared* pMe)
{
	
	

	while (1) // all particle system sons
	{
		/*par.NextWord_IC(); 
		par.FillCurrentWord(currentWord);*/

		if (strcmp(currentWord,"}")==0) // all sons ended
			return true;
			

		int iInstancesNum = 1; // default to 1 instance

		par.NextWord_IC(); 
		par.FillCurrentWord(currentWord);

		if (strcmp(currentWord,":")==0) // a request for few instance of a particle system
		{
			par.NextWord_IC(); 
			par.FillCurrentWord(currentWord);

			iInstancesNum = atoi(currentWord);

			if (!iInstancesNum)
			{
				char temp[600];
				sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
				g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
				sprintf(temp,"proper use is PS : [instances num]. Either no num was entered or a illegal zero val num was entered.");
				g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
				return false;
			}

			par.NextWord_IC(); 
			par.FillCurrentWord(currentWord);

			
		}

		

		if (strcmp(currentWord,"{")!=0)
		{
			char temp[600];
			sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
			sprintf(temp,"{ expected immidiately after particle system defenition");
			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
			return false;
		}

		
		bool bSkippedLine = false;

		while (1) // as long as this particle system defenition didn't end
		{
			
			if (!bSkippedLine)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
			}

			bSkippedLine=false;

			if (par.ReachedEnd())
			{
				char temp[600];
				sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
				g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
				sprintf(temp,"unexpected end of file! did you miss a } ?");
				g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
				return false;
			}

			if (strcmp(currentWord,"}")==0) // this particle system ended
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				if (par.ReachedEnd())
					return true;

				// debug
				return true;
			}


			if (strcmp(currentWord,"PS")==0) // new particle system
			{

				CParticleSystem_Shared* pPS = new CParticleSystem_Shared();
				pPS->Init();

				pMe->m_Children.push_back(pPS);

				bool bRes = ParseParticleSystem(par,pcFileName,iDepth+1,pPS);
				
				if (!bRes)
					return false;

				if (par.ReachedEnd())
					return true;


				// debug
				bSkippedLine=true;

				continue;
			}
			/*else if (strcmp(currentWord,"}")==0) // particle system ended
				break;*/
		
			// if we're here then it's not an inner particle system defenition and it's a quality of a particles system
		

			// NOTICE:REMEMBER TO ALWAYS USE pMe->blah!!!!!

			if (strcmp(currentWord,"no_bloom")==0)
			{						
				pMe->m_bBloom = false;
			} else 
			if (strcmp(currentWord,"total_life_time")==0)
			{		
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_dTotalLifeTime = atof(currentWord)*1000.f;				

			} else if (strcmp(currentWord,"time_till_full_appear")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_dTimeTillFullFirstAppearance = atof(currentWord)*1000.f;												
			}
			else if	(strcmp(currentWord,"acceleration")==0)
			{
				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vAcceleration.x = atof(currentWord);

				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vAcceleration.y = atof(currentWord);

				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vAcceleration.z = atof(currentWord);

				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

			} else if (strcmp(currentWord,"particle_size")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				pMe->m_fParticleSize = atof(currentWord);


			} else if (strcmp(currentWord,"life_time_range")==0)
			{
				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_fLifeTimeRange_Min = atof(currentWord);

				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_fLifeTimeRange_Max = atof(currentWord);

				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

			} else if (strcmp(currentWord,"start_dir_range")==0)
			{
				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Min.x = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Max.x = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Min.y = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Max.y = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Min.z = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartVelocityRange_Max.z = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);


			} else if (strcmp(currentWord,"start_pos_offset_range")==0)
			{
				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Min.x = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Max.x = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Min.y = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Max.y = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				// read "("
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Min.z = atof(currentWord);
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				pMe->m_vStartPositionOffsetRange_Max.z = atof(currentWord);
				// read ")"
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

			}
			else if (strcmp(currentWord,"max_particles")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				int iParticlesMax = atoi(currentWord);

				if (!iParticlesMax)
				{
					char temp[600];
					sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
					g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
					sprintf(temp,"unexpected end of file! did you miss a } ?");
					g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
					return false;
				}

				pMe->m_uiParticlesMax = iParticlesMax;

			}
			else if (strcmp(currentWord,"transform_origin")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);

				if (strcmp(currentWord,"rotate_around")==0)
				{
					pMe->m_bRotateAroundOrigin = true;

					// read "("
					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);
															
					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);
					pMe->m_vStartRotateDistance.x = atof(currentWord);

					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);
					pMe->m_vStartRotateDistance.y = atof(currentWord);

					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);
					pMe->m_vStartRotateDistance.z = atof(currentWord);

					// read ")"
					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);

					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);
					pMe->m_fRotateSpeed = atof(currentWord);


					// read "("
					par.NextWord_IC(); 
					par.FillCurrentWord(currentWord);

					pMe->m_bRotateAround[0] = false;
					pMe->m_bRotateAround[1] = false;
					pMe->m_bRotateAround[2] = false;

					while (strcmp(currentWord,")")!=0)
					{
						if (par.ReachedEnd())
						{
							char temp[600];
							sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
							sprintf(temp,"error with transform_origin syntax. can't find ) for rotation axis defenition.");
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
							return false;
						}

						par.NextWord_IC(); 
						par.FillCurrentWord(currentWord);

						if (strcmp(currentWord,"x")==0)
							pMe->m_bRotateAround[0] = true;
						else if (strcmp(currentWord,"y")==0)
							pMe->m_bRotateAround[1] = true;
						else if (strcmp(currentWord,"z")==0)
							pMe->m_bRotateAround[2] = true;
						else if (strcmp(currentWord,")")!=0)
						{
							char temp[600];
							sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
							sprintf(temp,"error with transform_origin syntax. unfamilier axis: [%s]",currentWord);
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
							return false;
						}
					}

				} else // other type of transformation
				{
					char temp[600];
					sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
					g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
					sprintf(temp,"unfamilier transform_origin operation.",currentWord);
					g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
					return false;
				}
				
			}
				
			else if (strcmp(currentWord,"material")==0)
			{
					par.NextWord_IC(); 
					par.FillCurrentQuetuePhrase(currentWord);

					int iLength = strlen(currentWord);
					if (!iLength)
					{
							char temp[600];
							sprintf(temp,"CParticleSystem_Shared::Load opening of Particle System script [%s] failed!",pcFileName);
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
							sprintf(temp,"error with material syntax. must provide material name like ""Materials\lava.mat"" ");
							g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);		
							return false;
					}

					pMe->m_pcMaterialName = new char[iLength+1];
					strcpy(pMe->m_pcMaterialName,currentWord);
						
				

			} else // unrecognized command
			{
				// ignore unkown commands for now
				//FIXME: if first word on next line is not after spaces, this might be problematic
				par.NextLine();
				par.EatWhiteSpace();
				par.FillCurrentWord(currentWord);
				
				bSkippedLine=true;
				continue;
			}

		} // end of reading particle system defenitions

		


	} // end of looping on all particle system sons


}

void CParticleSystem_Shared::Init(void)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create the INDEX BUFFER

	// init index buffer
	m_pIB = g_pRendererBack->GenerateIndexBuffer();

	// 6 indices per particle.
	int iIndicesNum = m_uiParticlesMax * 6;
	
	m_pIB->Allocate(iIndicesNum,IB_MEM_GPU);

	UINT* pIndices = new UINT[iIndicesNum];

	for (int i=0;i<m_uiParticlesMax;i++)
	{
		pIndices[6*i]   = (i*4);
		pIndices[6*i+1] = (i*4)+1;
		pIndices[6*i+2] = (i*4)+2;
		pIndices[6*i+3] = (i*4);
		pIndices[6*i+4] = (i*4)+2;
		pIndices[6*i+5] = (i*4)+3;
	}

	m_pIB->Lock(CR_LOCK_WRITE_ONLY,0,0);
	UINT* pIndicesData = (UINT*) m_pIB->GetData();
	memcpy(pIndicesData,pIndices,sizeof(UINT)*iIndicesNum);
	m_pIB->Unlock();

	SafeDeleteArray(pIndices);


	
}