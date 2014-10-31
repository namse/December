#pragma once
#include "GameObject.h"

class NPC : public GameObject
{
public:
	NPC();
	virtual ~NPC();

protected:
	NPCType	m_NPCType;
};

