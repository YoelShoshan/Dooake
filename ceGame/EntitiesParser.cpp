#include "stdafx.h"
#include "EntitiesParser.h"

#include "Parser.h"
#include <fstream>

using namespace std;									// set our namespace to standard;

ofstream fout;								

#include <assert.h>
//#include <crtdbg.h>

/*#include "LogFile.h"
extern CLogFile g_LogFile;*/

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////// CEntitiesParser \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CEntitiesParser Constructer
/////
///////////////////////////////// CEntitiesParser \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CEntitiesParser::CEntitiesParser()
{
	return;
}


///////////////////////////////// SetEntitiesString \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Sets our m_cpEntities to the InputString
/////
///////////////////////////////// SetEntitiesString \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CEntitiesParser::SetEntitiesString(char *InputString)
{
	m_cpEntities = InputString;
	return;
}

///////////////////////////////// SetEntitiesString \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Exports our Entities string to a File
/////
///////////////////////////////// SetEntitiesString \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CEntitiesParser::ExportEntitiesToFile(void)
{
	/*fout.open("TempEntities.txt");									
	fout << m_cpEntities;
	fout.close();*/
}

///////////////////////////////// GetPlayerPosition \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	 Puts Entity Origin number [inum] into Vector
/////    obviously a very ugly way to do it, but no time... :(
/////    The correct way is to parse everything from our String and not load
/////    the file over and over again like I did.
/////    
///////////////////////////////// GetPlayerPosition \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVec3f CEntitiesParser::GetPlayerPosition(int iNum)
{
	int iTemp=0;

	for (int i=0;i<m_iEntitiesNum;i++)
	{
		if (m_pEntities[i].uiEntityType == kInfoPlayerDeatchMatch)
		{			
			if (iTemp==iNum)
			{
				return m_pEntities[i].vOrigin;
			}
			iTemp++;
		}

	}
	//Vector.v[0] = Vector.v[1] = Vector.v[2];
}

int CEntitiesParser::FillPlayerPositions(CVec3f*& pStartPositions,float*& pfStartAngles)
{	

	char* cpDebug = new char[400];

	int iPositionsNum=0;

	for (int i=0;i<m_iEntitiesNum;i++)
	{
		if (m_pEntities[i].uiEntityType == kInfoPlayerDeatchMatch)
		{			
			iPositionsNum++;
		}

	}

	if (iPositionsNum == 0)
	{
		pStartPositions = new CVec3f[1];
		pfStartAngles   = new float[1];

		pStartPositions[0].Set(50.f,50.f,-350.f);
		pfStartAngles[0] = 180.f;

		return 1;
	}

	//
	/*sprintf(cpDebug,"Positions Num:%d",iPositionsNum);
	g_LogFile.OutPutPlainText(cpDebug,"4","990000",true);*/
	//

	pStartPositions = new CVec3f[iPositionsNum];
	pfStartAngles   = new float[iPositionsNum];

	int iTemp=0;

	

	for (int i=0;i<m_iEntitiesNum;i++)
	{
		if (m_pEntities[i].uiEntityType == kInfoPlayerDeatchMatch)
		{			
			//pStartPositions[iTemp] = m_pEntities[i].vOrigin+CVec3f(0.f,80.f,0.f);
			pStartPositions[iTemp] = m_pEntities[i].vOrigin+CVec3f(0.f,180.f,0.f);
			//pStartPositions[iTemp] = m_pEntities[i].vOrigin;
			pfStartAngles[iTemp]   = (float) m_pEntities[i].iAngle;
			//
			/*sprintf(cpDebug,"Start Position %d: %f,%f,%f.",iTemp,pStartPositions[iTemp].v[0],pStartPositions[iTemp].v[1],pStartPositions[iTemp].v[2]);
			g_LogFile.OutPutPlainText(cpDebug,"4","990000",true);*/
			//

			iTemp++;
		}

	}

	SafeDeleteArray(cpDebug);

	return iPositionsNum;
}



///////////////////////////////// Load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    Here we load for the first time our Entities Text file
/////    we set m_iEntitiesOriginsNum, and use the first encountered
/////    Entity Origin for the Start position when the app starts
/////
///////////////////////////////// Load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CEntitiesParser::Parse(void)
{

	m_iEntitiesNum = 0;
	m_iStartPositions = 0;

	COldParser parser;
	COldParser parser2;

	char* TempWord;
	TempWord = new char[512];

	char* TempWord2;
	TempWord2 = new char[512];



	assert(_CrtCheckMemory());


	parser.SetAll(m_cpEntities);

	while (1)
	{
		parser.GetString(TempWord);
		if (TempWord[0]=='{')	
			while (TempWord[0]!=NULL)
			{
				parser.NextWord();
				parser.GetString(TempWord);
				if (TempWord[0]=='}') // done with this entity
				{					
					m_iEntitiesNum++;
					parser.NextWord();
					break;
				}
				parser.RemoveParenthesis(TempWord);


				if (strcmp(TempWord,"dmg")==0) // damage
				{
					parser.NextWord();
					parser.GetString(TempWord);
					parser.RemoveParenthesis(TempWord);
					//strcpy(&m_pEntities[m_iEntitiesNum].caTargetName[0],TempWord);

					m_pEntities[m_iEntitiesNum].iDamage = atoi(TempWord);
					

				}
				else
				if (strcmp(TempWord,"targetname")==0 || strcmp(TempWord,"target")==0) // target or target name
				{
					parser.NextWord();
					parser.GetString(TempWord);
					parser.RemoveParenthesis(TempWord);
					strcpy(&m_pEntities[m_iEntitiesNum].caTargetName[0],TempWord);
				}
				else
				if (strcmp(TempWord,"model")==0)  // model
				{
					parser.NextWord();
					parser.GetString(TempWord);
					parser.RemoveParenthesis(TempWord);

					if (TempWord[0]=='*') // not a md3
					{
						char* pTemp = &TempWord[1];
						m_pEntities[m_iEntitiesNum].iModelIndex = atoi(pTemp);

						// here i need to use my model manager :)
					}

				}
				else
				if (strcmp(TempWord,"origin")==0)  // origin
				{
					parser.NextWord();
					parser.GetStringIgnoreSpace(TempWord);
					parser.RemoveParenthesis(TempWord);

					parser2.SetAll(TempWord);
					parser2.GetString(TempWord2);
					m_pEntities[m_iEntitiesNum].vOrigin.v[0] = parser2.GetFloat();


					parser2.NextWord();
					parser2.GetString(TempWord2);
					m_pEntities[m_iEntitiesNum].vOrigin.v[2] = -parser2.GetFloat();

					parser2.NextWord();
					parser2.GetString(TempWord2);
					m_pEntities[m_iEntitiesNum].vOrigin.v[1] = parser2.GetFloat();



					parser.NextLine();

				}
				else
				if (strcmp(TempWord,"angle")==0)  // classname
				{
					parser.NextWord();
					parser.GetStringIgnoreSpace(TempWord);
					parser.RemoveParenthesis(TempWord);

					parser2.SetAll(TempWord);
					parser2.GetString(TempWord2);
					m_pEntities[m_iEntitiesNum].iAngle = parser2.GetFloat();
			

					parser.NextLine();


				}
				else				
				if (strcmp(TempWord,"classname")==0)  // classname
				{
					parser.NextWord();
					parser.GetString(TempWord);
					parser.RemoveParenthesis(TempWord);



					//kHealth
					
					if (strcmp(TempWord,"item_armor_combat")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kCombatArmor;
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"item_armor_shard")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kArmorShard;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"item_health")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kHealth;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"item_armor_body")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kArmorBody;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"ammo_bullets")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kAmmoBullets;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"ammo_rockets")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kAmmoRockets;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"ammo_shells")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kAmmoShells;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"ammo_shells")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kAmmoShells;				
						//m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"info_player_deathmatch")==0)
					{
						m_pEntities[m_iEntitiesNum].uiEntityType = kInfoPlayerDeatchMatch;
						m_iStartPositions++;
					}
					else
					if (strcmp(TempWord,"trigger_push")==0)
						m_pEntities[m_iEntitiesNum].uiEntityType = kTriggerPush;
					else
					if (strcmp(TempWord,"trigger_teleport")==0)
						m_pEntities[m_iEntitiesNum].uiEntityType = kTriggerTeleport;					
					else
					if (  (strcmp(TempWord,"target_position")==0) ||
						  (strcmp(TempWord,"misc_teleporter_dest")==0) ||
						  (strcmp(TempWord,"trigger_multiple")==0) ||
						  (strcmp(TempWord,"info_notnull")==0) ||
						  (strcmp(TempWord,"target_relay")==0))
						m_pEntities[m_iEntitiesNum].uiEntityType = kTargetPosition;	
					else // something i'm not handling

					if (strcmp(TempWord,"trigger_hurt")==0)
						m_pEntities[m_iEntitiesNum].uiEntityType = kTriggerHurt;					
					else
						m_pEntities[m_iEntitiesNum].uiEntityType = 0; // none
						

				}
				else
				if (strcmp(TempWord,"message")==0) // message
				{
					parser.NextWord(); 

					TempWord[0] = '"';
					TempWord[1] = NULL;

					parser.GetStringIgnoreSpace(TempWord);										

					parser.NextLine();
					//parser.GetString(TempWord); // debug
				}
				else
					parser.NextLine(); // something i'm not reading
			}
		else
			break;


	}



	parser.NextWord();


	SafeDeleteArray(TempWord);
	SafeDeleteArray(TempWord2);
}
