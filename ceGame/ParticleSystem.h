#ifndef CRIMSON_PARTICLE_SYSTEM_H
#define CRIMSON_PARTICLE_SYSTEM_H

#pragma once

#include "RenderModel.h"
#include "Math3d.h"
#include "IndexBuffer.h"
#include "VertexBuffer_IE.h"
#include "Renderable.h"
#include "ParticleSystem_Shared.h"

#include <vector>




class CParticleSystem : public IRenderModel
{
public:
	CParticleSystem();	
	~CParticleSystem();


	// can be used by creator of the particle system to know how to update it
	// (for example, character skeleton joint index)
	unsigned int GetBindIndex(void) const { return m_uiBindIndex;};
	void SetBindIndex(unsigned int uiBindIndex) { m_uiBindIndex = uiBindIndex; };

	bool GetFinishedDying(void) const { return m_bFinishedDying;};
	//void SetFinishedDying(bool bVal) { m_bFinishedDying = bVal;};

	//void SetForceStartDying() { m_bForceStartDying = true; };
	void Die(void) { m_bDying=true;};
	
	// seconds
	//void SetTimeTillFullFirstAppear(double dTime);

	// triggers INIT as well right now.
	void SetShared(CSmartPointer<CParticleSystem_Shared> pShared);



	void SetOrigin(const CVec3f& vOrigin);

	CVec3f GetOrigin(void) const { return m_vOrigin;};

	//// IRenderModel interface requirements

	void ClearEffectingLights(void) {};
	void AddEffectingLight(ILight_Export* pLight) {};

	void SetPosition(const CVec3f& vPos) {};
	
	void   SetAngles(const CVec3f& vAngles) {};
	CVec3f GetAngles(void) const {return CVec3f(0.f,0.f,0.f);};

	
	void BlendAnglesState(float fAlpha) {};
	CVec3f GetCurrentAngles(void) const {return CVec3f(0.f,0.f,0.f);};
	CVec3f GetPreviousAngles(void) const {return CVec3f(0.f,0.f,0.f);};

	void Update(float fDeltaTime,bool bVisible);
	
	void Render(int iTech);

	void SetCleanMe(void) { m_bCleanMe = true;};
	bool GetCleanMe(void) const { return m_bCleanMe; };

	void InformParentDied(void) { m_bParentEntityDied = true;};
	bool GetParentDied(void) const { return m_bParentEntityDied;};



	const char* GetName(void) const { return m_pcName; };


private:

	bool m_bCleanMe;

	//void SetParentOrigin(const CVec3f& vParentOrigin);

	void SetShared_Helper(const CParticleSystem_Shared* pShared);

	float GetRandFloatInRange(float fMin,float fMax);

	void Init(void);

	CSmartPointer<CParticleSystem_Shared> m_pShared;
	
	tParticle* m_pParticles;

	unsigned int m_uiBindIndex;

	static CVec3f* s_pVertices_Temp;
	static unsigned int s_uiVertices_Temp_Size;
	static DWORD* s_pColors_Temp;
	//hack for now
	static CVec2f* s_pTexcoords_Temp;


	CVec3f m_vPosition;

	// particle system origin
	CVec3f  m_vOrigin;

	//CVec3f m_vParentOrigin;
	
	// in order to prevent that all particles will suddenly show at the same moment
	// this sets on how long you can stretch the appreance of them
	//double m_dTimeTillFullFirstAppearance; 

	double m_dCreationTime;
	//double m_dTotalLifeTime;

	bool m_bDying;
	bool m_bFinishedDying;
	//bool m_bForceStartDying;


	bool m_bParentEntityDied;
	

	

	// in case you want the particle system to wait a bit before showing up
	// miliseconds
	float m_fStartTimeDelay; // not implemented yet.
		

	unsigned int  m_uiParticlesAppeared;

	CRenderable*  m_pRend;	
	IVertexBuffer_Export* m_pVB;
	
	std::vector<CParticleSystem*> m_Children;

	int m_iVertsNum;

	char* m_pcName;

};






#endif