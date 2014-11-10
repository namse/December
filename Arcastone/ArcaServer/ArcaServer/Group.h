#pragma once

#include "UnitData.h"

class Group
{
public:
	Group(){}
	~Group(){}

	std::vector<UnitData>* GetUnitList(){ return &m_UnitList; }
	void SetUnitList(std::vector<UnitData> unitList){ m_UnitList = unitList; }

private:
	std::vector<UnitData> m_UnitList;
};

