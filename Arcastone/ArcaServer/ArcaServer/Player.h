#pragma once

#include "Group.h"

class Player
{
public:
	Player(PlayerNumber playerNum);
	~Player();

	Group* GetGroup(int groupIndex){ return &m_GroupList.at(groupIndex); }

	void SetGroup(int groupIndex, Group group){ m_GroupList.at(groupIndex) = group; }
	void SetGroup(Group group){ m_GroupList.push_back(group); }

private:
	PlayerNumber			m_PlayerNum;

	std::vector<Group>		m_GroupList;
};

