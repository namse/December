#pragma once

#include "Group.h"

class Player
{
public:
	Player(PlayerNumber playerNum);
	~Player();

	void SetGroupForTest();
	std::vector<Group> GetGroupList(){ return m_GroupList; }

private:
	PlayerNumber		m_PlayerNum;
	std::vector<Group>		m_GroupList;
	std::vector<UnitData>	m_UnitList;
};

