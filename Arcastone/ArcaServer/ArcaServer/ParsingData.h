#pragma once

#include "UnitData.h"

class ParsingData
{
public:

	ParsingData();
	~ParsingData();

	UnitData GetUnitData(UnitType unitType);

private:
	std::map<UnitType, UnitData> m_UnitList;
};

extern ParsingData* GParsingData;