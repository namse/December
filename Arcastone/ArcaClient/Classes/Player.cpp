#include "Player.h"


Player::Player()
{
	m_UnitCounter = 0;
}


Player::~Player()
{
}

Unit* Player::getUnit(int idx)
{
	if (idx >= 0 && idx < m_UnitCounter)
		return &m_Unit[idx];
	return nullptr;
}

void Player::setUnit(UnitData unitData)
{
	Unit unit;
	unit.setUnitStatus(unitData);

	m_Unit[m_UnitCounter++] = unit;
}