#pragma once

#include "Header.h"
#include "Unit.h"

class Field
{
public:
	Field();
	~Field();

	void						setUnit();

	//map<Position, UnitData*>*	getUnit(){ return &m_Unit; }
	//UnitData*					getUnit(Position key);

private:
	//map<Position, UnitData*>	m_Unit;

};

