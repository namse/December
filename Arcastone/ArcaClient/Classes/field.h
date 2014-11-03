#pragma once

#include "Header.h"
#include "Unit.h"

class Field
{
public:
	Field();
	~Field();

	void						setUnitData(UnitData unitData){ m_UnitData.push_back(unitData); }

	//map<Position, UnitData*>*	getUnit(){ return &m_Unit; }
	//UnitData*					getUnit(Position key);

private:
	vector<UnitData>			m_UnitData;

};

