#pragma once

#include "Header.h"
#include "Unit.h"

class NPC : public Unit
{
public:
	NPC();
	virtual ~NPC();

	void initSprite();
};

