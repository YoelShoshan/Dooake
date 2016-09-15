#ifndef CRIMSON_CONTROLLABLE_PHYSICS_H
#define CRIMSON_CONTROLLABLE_PHYSICS_H

#pragma once

#include "BasicPhysics.h"

#include "Entity.h"

class CControllablePhysics : public CBasicPhysics
{
public:

	bool IsSlider(void) { return true; };

	CControllablePhysics();
	~CControllablePhysics();


	void FindPhysicsStepDesiredPosition(float fStep);

	
	

	

	//bool GetRenderRenderModel(void) { return m_bRenderRenderModel; };
	/*void ProcessInputRequests(void);

	void ClearInputBooleans(void);*/

	
private:
	//bool m_bInAir;

	

	

};



#endif