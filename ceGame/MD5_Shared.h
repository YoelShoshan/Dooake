#ifndef CRIMSON_MD5_SHARED_H
#define CRIMSON_MD5_SHARED_H

#pragma once

#include "MD5Mesh.h"
#include "MD5Anim.h"

#include "ReferenceCount.h"

#define MD5_INSTANCE_ANIMATIONS_MAX 50

struct CMD5_Shared : public CReferenceCount
{
friend class CMD5Manager;
public:
	
	~CMD5_Shared();

	// REMEMBER:
	// assumes that -*joint name always appears AFTER the joints it should delete!
	bool LoadCharacterFile(const char* pchFileName);	

	const CMD5Mesh* GetMesh(void) const { return m_pMesh; };
	CMD5Anim*const* GetAnims(void) const { return m_ppAnimations;}
	int GetAnimsNum(void) const { return m_iAnimationsNum;};

	const IIndexBuffer*  GetIndexBuffer(int iIndex) const { return m_ppIndexBuffers[iIndex];};
	
	bool GetFlipUpDown(void) const { return m_bFlipLookUpDown;};

	bool GetIsAutoFire_Attack1(void) const { return m_bAutoFire_Attack1; };
	bool GetIsAutoFire_Attack2(void) const { return m_bAutoFire_Attack2; };

	const char* GetName(void) const { return m_pcName;};

protected:
	CMD5_Shared();

	void SetMesh(CMD5Mesh* pSet);
	bool AddAnim(CMD5Anim* pAnim);

	char* m_pcCharacterName;

	bool GenerateIndexBuffers(void);

	IIndexBuffer**  m_ppIndexBuffers;

	CMD5Mesh* m_pMesh; 
	int      m_iAnimationsNum;
	CMD5Anim** m_ppAnimations; 	
	bool m_bFlipLookUpDown;

	bool m_bAutoFire_Attack1;
	bool m_bAutoFire_Attack2;

	char* m_pcName;

};


#endif