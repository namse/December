#pragma once
#include "UnitData.h"

struct Group
{

	void AddUnitData(UnitData unitData, Coord originPosition) { m_UnitDataList.insert(GroupUnitList::value_type(originPosition, unitData)); }

	typedef Coord OriginPosition;
	typedef std::map<OriginPosition, UnitData> GroupUnitList;
	GroupUnitList m_UnitDataList;
};

