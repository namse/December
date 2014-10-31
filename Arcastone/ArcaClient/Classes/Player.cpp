#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}

Unit* Player::getUnit(int idx)
{
	if (!(idx >= 0 && idx <= m_Unit.size()))
		return nullptr;

	return m_Unit.at(idx);
}