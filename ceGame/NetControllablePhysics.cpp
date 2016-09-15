#include "stdafx.h"

#include "NetControllablePhysics.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

extern bool g_bDeadReckoning;

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

CNetControlalblePhysics::CNetControlalblePhysics()
{

};

CNetControlalblePhysics::~CNetControlalblePhysics()
{

};

/*void CNetControlalblePhysics::BlendPhysicsState(float fAlpha)
{
	m_vPos = (m_vCurrentTimeStepPos*fAlpha) + (m_vPreviousTimeStepPos*(1.f-fAlpha));	
}*/

void CNetControlalblePhysics::SetLastRecievedPositionAngles(const CVec3f& vPos,const CVec3f& vAngles)
{
	// update positions
	m_vLastRecievedPosition[1] = m_vLastRecievedPosition[0];
	m_vLastRecievedPosition[0]= vPos;

	// update angles

	m_vLastRecievedAngles[1] = m_vLastRecievedAngles[0];
	m_vLastRecievedAngles[0]= vAngles;



	// update times
	m_vLastRecievedPosAng_Times[1] = m_vLastRecievedPosAng_Times[0];
	m_vLastRecievedPosAng_Times[0] = g_pTimer->GetTime();

	//m_vPos = vPos;
	m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;
	m_vCurrentTimeStepPos = vPos;

	/*char msg[150];

	sprintf(msg,"Got a networked position update:%f,%f,%f",vPos.x,vPos.y,vPos.z);
	g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/
}



void CNetControlalblePhysics::FindPhysicsStepDesiredPosition(float fStep)
{	
	
	char msg[150];
	
	m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;


	if (g_bDeadReckoning)
	{
		CVec3f vPositionDelta = m_vLastRecievedPosition[0] - m_vLastRecievedPosition[1];
		CVec3f vAnglesDelta = m_vLastRecievedAngles[0] - m_vLastRecievedAngles[1];
		double dDeltaTime = m_vLastRecievedPosAng_Times[0] - m_vLastRecievedPosAng_Times[1];
		dDeltaTime*=0.001f;

		
		//////////////////////////////////////////////////////////////////////
		// position

		//m_vDesiredPos = m_vCurrentTimeStepPos + vPositionDelta*fStep;
		m_vDesiredPos = m_vCurrentTimeStepPos + vPositionDelta*dDeltaTime;
		
		m_vCurrentTimeStepPos = m_vDesiredPos;

		//////////////////////////////////////////////////////////////////////
		// angles

		CVec3f vNewAngles = m_pParentEntity->GetAngles() + vAnglesDelta*dDeltaTime;
		m_pParentEntity->SetAngles(vNewAngles);




		/*sprintf(msg,"fstep: %f",fStep);			 
		g_TextMessages.AddMessage(msg,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/
		
		
		/*sprintf(msg,"Delta Time Between incoming Packets: %f",dDeltaTime);			 
		g_TextMessages.AddMessage(msg,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

		/*if (vDelta.x!=0.f || vDelta.y!=0.f || vDelta.z!=0.f)
		{
			sprintf(msg,"Dead Reckoning Delta: %f,%f,%f",vDelta.x,vDelta.y,vDelta.z);			 
			g_TextMessages.AddMessage(msg,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		}*/

		
		/*sprintf(msg,"Dead reckoning found new desiredPosition:%f,%f,%f",m_vDesiredPos.x,m_vDesiredPos.y,m_vDesiredPos.z);
		g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_INFORMATIVE);*/

	}
	else
		m_vDesiredPos = m_vCurrentTimeStepPos;

	/*CVec3f vDelta = m_vCurrentTimeStepPos - m_vPreviousTimeStepPos;


	///// debug info /////////////////////////////////////////////////
	
	
	



	/////////////////////////////////////////////////////////////////

	m_vPreviousTimeStepPos = m_vCurrentTimeStepPos;

	//m_vCurrentTimeStepPos += vDelta;
	m_vDesiredPos = m_vCurrentTimeStepPos + vDelta*fStep;*/


}