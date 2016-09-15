#ifndef CRIMSON_PARTICLE_SYSTEM_SHARED_H
#define CRIMSON_PARTICLE_SYSTEM_SHARED_H

#pragma once

#include "ReferenceCount.h"

#include "Math3d.h"

#include "IndexBuffer.h"
#include "VertexBuffer_IE.h"
#include "Renderable.h"

#include "ceParser.h"

#include <vector>

struct tParticle
	{
		tParticle():fLife(0.f) {};

		float fLife;
		CVec3f vPos;
		CVec3f vVelocity;
	};

class CParticleSystem_Shared : public CReferenceCount
{
	friend class CParticleSystemManager;

public:	
	~CParticleSystem_Shared();


	bool Load(const char* pcFileName);

	const std::vector<CParticleSystem_Shared*>* GetChildren(void) const { return &m_Children;};

	IIndexBuffer* GetIB(void) const { return m_pIB;};
	
	CVec3f GetAcceleration(void) const { return m_vAcceleration;};

	CVec3f GetStartVelRangeMin(void) const { return m_vStartVelocityRange_Min;};
	CVec3f GetStartVelRangeMax(void) const { return m_vStartVelocityRange_Max;};

	CVec3f GetStartPosOffsetRangeMin(void) const { return m_vStartPositionOffsetRange_Min;};
	CVec3f GetStartPosOffsetRangeMax(void) const { return m_vStartPositionOffsetRange_Max;};



	// rotate around origin

	bool GetRotateAroundOrigin(void) const { return m_bRotateAroundOrigin;};

	CVec3f GetStartRotationDistance(void) const { return m_vStartRotateDistance;};

	float GetRotationSpeed(void) const { return m_fRotateSpeed;};
	
	bool GetRotateAroundX() const { return m_bRotateAround[0];};
	bool GetRotateAroundY() const { return m_bRotateAround[1];};
	bool GetRotateAroundZ() const { return m_bRotateAround[2];};
	
	

	float GetLifeTimeRangeMin(void) const { return m_fLifeTimeRange_Min;};
	float GetLifeTimeRangeMax(void) const { return m_fLifeTimeRange_Max;};

	unsigned int GetParticlesMax(void) const { return m_uiParticlesMax;};
	
	bool GetTransformParticlesWithOrigin(void) const { return m_bTrasnformParticlesWithOrigin;};

	const char* GetMaterialName(void) const { return m_pcMaterialName;};

	float GetParticleSize(void) const { return m_fParticleSize;};

	double GetTotalLifeTime(void) const { return m_dTotalLifeTime;};

	double GetTimeTillFullAppear(void) const { return m_dTimeTillFullFirstAppearance;};

	bool GetBloom(void) const { return m_bBloom;};


	const char* GetName(void) const { return m_pcName;};

protected:
	CParticleSystem_Shared();

private:

	void InitAll(void);

	void Init(void);

	bool ParseParticleSystem(CParser& par,const char* pcFileName,int iDepth,CParticleSystem_Shared* pMe);


	

	// acceleration (gravity for example)
	CVec3f m_vAcceleration;

	// start velocity range
	CVec3f m_vStartVelocityRange_Min;
	CVec3f m_vStartVelocityRange_Max;

	// start position offset range
	CVec3f m_vStartPositionOffsetRange_Min;
	CVec3f m_vStartPositionOffsetRange_Max;

	// life time in seconds
	float m_fLifeTimeRange_Min;
	float m_fLifeTimeRange_Max;

	double m_dTotalLifeTime;

	// max particles
	unsigned int m_uiParticlesMax;

	// in order to prevent that all particles will suddenly show at the same moment
	// this sets on how long you can stretch the appreance of them
	double m_dTimeTillFullFirstAppearance; 

	// rotate around origin
	bool m_bRotateAroundOrigin;
	CVec3f m_vStartRotateDistance;
	float  m_fRotateSpeed;
	bool   m_bRotateAround[3];

	bool  m_bBloom;
	
	float m_fParticleSize;

	char* m_pcMaterialName;

	// transform particles with origin
	// translates each particle along with the origin
	bool m_bTrasnformParticlesWithOrigin;

	IIndexBuffer* m_pIB;
	
	char* m_pcName;

	bool m_bDummy;

	std::vector<CParticleSystem_Shared*> m_Children;

	static char currentWord[500];

};


#endif