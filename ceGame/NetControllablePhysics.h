#ifndef NETWORKED_CONTROLLABLE_PHYSICS_H
#define NETWORKED_CONTROLLABLE_PHYSICS_H

#pragma once

#include "BasicPhysics.h"

#define LAST_RECIEVED_POS_ANG_SIZE 2


class CNetControlalblePhysics : public CBasicPhysics
{
public:
	CNetControlalblePhysics();
	~CNetControlalblePhysics();

	void SetLastRecievedPositionAngles(const CVec3f& vPos,const CVec3f& vAngles);
	

	//?????
	//bool IsSlider(void) { return true; };
	

	// DEAD RECKONING!
	// instead of finding the real physics one, use dead reckoning
	void FindPhysicsStepDesiredPosition(float fStep);

	//void BlendPhysicsState(float fAlpha);

private:
	CVec3f m_vLastRecievedPosition[LAST_RECIEVED_POS_ANG_SIZE]; // 0 is last, 1 is one before last, etc.	
	CVec3f m_vLastRecievedAngles[LAST_RECIEVED_POS_ANG_SIZE]; // 0 is last, 1 is one before last, etc.

	double m_vLastRecievedPosAng_Times[LAST_RECIEVED_POS_ANG_SIZE];
	


};






#endif