#pragma once

#include "Header.h"
#include "Unit.h"

class Player
{
public:
	Player();
	~Player();

	Unit*						getUnit(int idx);
	int							getUnitCounter(){ return m_UnitCounter; }

	void						setUnit(Unit unit){ m_Unit[m_UnitCounter++] = unit; }
	void						setUnit(UnitData unitData);
	
private:
	Unit						m_Unit[MAX_UNIT_ON_GAME];
	int							m_UnitCounter;

};

