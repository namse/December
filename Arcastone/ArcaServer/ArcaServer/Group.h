#pragma once
#include "Unit.h"

class Group
{
public:
	Group();
	~Group();

private:
	GroupNumber			m_GroupId;
	std::vector<Unit*>	m_UnitList;
};

