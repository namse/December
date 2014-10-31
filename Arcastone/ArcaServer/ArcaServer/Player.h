#pragma once
#include "Unit.h"
#include "Group.h"

class Player
{
public:
	Player(PlayerNumber playerId);
	~Player();
private:
	PlayerNumber	m_PlayerID;
	std::vector<Unit*>	m_UnitList;
	std::vector<Group*>	m_GroupList;
};

