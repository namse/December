#include "Field.h"


Field::Field()
{
}


Field::~Field()
{
}

void Field::fieldInit()
{
}

void Field::setUnitData(UnitData unitData[], int length)
{
	for (int i = 0; i < length; ++i)
		m_UnitData[i] = unitData[i];

	m_UnitDataLength = length;
}



/*
UnitData* Field::getUnit(Position key)
{
	// key 값에 대응하는 UnitData 를 반환
	map<Position, UnitData*>::iterator findIter;
	findIter = m_Unit.find(key);

	if (findIter != m_Unit.end())
	{
		return findIter->second;
	}
}*/