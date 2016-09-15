#include "stdafx.h"
#include "PhysicsModel.h"

#include "ce.h"

CEntity* IPhysicsModel::GetParentEntity(void)
{ 
	BREAKPOINT(!m_pParentEntity);
	return m_pParentEntity; 
}

void IPhysicsModel::SetCurrentTimeStepPos(const CVec3f& vCurrentTimeStepPos)
{
	BREAKPOINT(   *((unsigned long*)&vCurrentTimeStepPos) == 0xffc00000 );

		m_vCurrentTimeStepPos = vCurrentTimeStepPos;
}