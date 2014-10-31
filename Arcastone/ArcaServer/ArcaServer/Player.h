#pragma once
#include "GameObject.h"
#include "Group.h"

class Player
{
public:
	Player(PlayerNumber playerNum);
	~Player();

private:
	PlayerNumber		m_PlayerNum;
	std::vector<Group*>			m_GroupList;
	std::vector<GameObject*>	m_UnitList;
};

