#pragma once
#include "GameObject.h"

class Unit : public GameObject
{
public:
	Unit();
	virtual ~Unit();

	void InitObject(ObjectNumber objectNum, PlayerNumber playerNum);

protected:
	PlayerNumber	m_PlayerNum;
	UnitType		m_UnitType;

};

