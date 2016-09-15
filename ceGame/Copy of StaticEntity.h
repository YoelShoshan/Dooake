#ifndef CRIMSON_STATIC_ENTITY_H
#define CRIMSON_STATIC_ENTITY_H

#pragma once

#include "Entity.h"

class CStaticEntity : public CEntity
{
public:
	virtual bool IsStatic(void) { return true;};

};







#endif