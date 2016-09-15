#ifndef CRIMSON_TECHNIQUE_H
#define CRIMSON_TECHNIQUE_H

#pragma once

#define TECHNIQUE_PASSES_MAX 8

#include "Pass.h"
#include "RTTProxy.h"

class  CTechnique
{
friend class CMaterial;
public:	
	~CTechnique();

	// change into addpass that return a new allocated pass	
	//bool   AddPass(CPass* pPass);
	CPass* AddPass(void);
	/////////////////////////////////////////////////////////////

	CPass* GetPass(int iIndex) const; 

	int    GetPassesNum(void) const;

	bool   BindPass(int iPassNum) const;
	
	bool	IsOpaque();
	

protected:
	CTechnique();


private:

	

	int   m_iPassesNum;
	CPass* m_Passes[TECHNIQUE_PASSES_MAX];
};

#endif
