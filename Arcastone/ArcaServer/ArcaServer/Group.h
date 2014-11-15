#pragma once
#include "UnitData.h"

struct Group
{

	void AddUnitData(UnitType unitType, Coord originPosition)
	{
		m_UnitDataList.insert(GroupUnitList::value_type(originPosition, unitType));
	}

	typedef	Coord OriginPosition;
	typedef	std::map<OriginPosition, UnitType> GroupUnitList;
	GroupUnitList m_UnitDataList;
};

