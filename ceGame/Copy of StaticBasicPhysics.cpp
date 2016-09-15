#include "stdafx.h"
#include "StaticBasicPhysics.h"

CBasicPhysics_Static::CBasicPhysics_Static()
{
	SetActive(false);

}

CBasicPhysics_Static::~CBasicPhysics_Static()
{


}

bool CBasicPhysics_Static::CheckHitBy(IPhysicsModel* pOtherPM,const CVec3f& vStartPos, const CVec3f& vEndPos,float& fMoveTillHit, CVec3f& vHitNormal )
{

	fMoveTillHit = 1.f;
	vHitNormal=CVec3f(0.f,1.f,0.f);


	float fMoveTillHit_temp;
	CVec3f vHitNormal_temp;


	/*CVec3f vPos  = pOtherPM->GetPreviousTimeStepPos();
	CVec3f vMove = pOtherPM->GetDesiredPos() - vPos;*/

	/*CVec3f vPos  = vStartPos;
	CVec3f vMove = vEndPos - vStartPos;*/


	// the following is used just for a boolean test, either hit or not
	// actual T until hit is ignored and 1.f is always restored
	// this is used in collectables for example, where you want to know if you've hit them, but you don't want cd&r with them.

	bool bInter  = 	AABBSweep( (m_BBox.max - m_BBox.min)*0.5f,
				                m_vCurrentTimeStepPos,
								m_vCurrentTimeStepPos,
 			                    //CVec3f(0.f,0.f,0.f),
				               (pOtherPM->m_BBox.max - pOtherPM->m_BBox.min)*0.5f,
				               vStartPos, // start
				               vEndPos,
							   //pOtherPM->GetPreviousTimeStepPos() ,  pOtherPM->GetDesiredPos() - pOtherPM->GetPreviousTimeStepPos(),

							   //pOtherPM->GetPreviousTimeStepPos() ,  pOtherPM->GetDesiredPos(),

				               fMoveTillHit_temp,vHitNormal_temp); // restore! - we DON'T wanna change anything in the movement.
							   //fMoveTillHit,vHitNormal); // debug


	return bInter;

}



