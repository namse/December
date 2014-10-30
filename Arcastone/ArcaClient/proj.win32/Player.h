#pragma once

#include "Header.h"
#include "Unit.h"

class Player
{
public:
	Player();
	~Player();

	vector<Unit*> getUnit() { return m_Unit; }
	Unit* getUnit(int idx);
	
private:
	vector<Unit*> m_Unit;

};

