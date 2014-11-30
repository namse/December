#pragma once

#include "Group.h"

class User
{
public:
	User(UserNumber userNum);
	~User();

	std::vector<Group>				GetGroupList(){ return m_GroupList; }

private:
	UserNumber						m_UserNum;
	std::vector<Group>				m_GroupList;
	std::vector<UnitType>			m_UnitList;
};

