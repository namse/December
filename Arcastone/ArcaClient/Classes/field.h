#pragma once

#include "Header.h"
#include "Unit.h"

class Field
{
public:
	Field();
	~Field();

	void						fieldInit();

	void						setUnitData(UnitData unitData, int idx){ m_UnitData[idx] = unitData; }
	void						setUnitData(UnitData unitData[], int length);

	UnitData					getUnitData(int idx){ return m_UnitData[idx]; }
	UnitData*					getUnitData(){ return m_UnitData; }
	int							getUnitDataLength(){ return m_UnitDataLength; }

private:
	UnitData					m_UnitData[MAX_UNIT_ON_GAME];
	int							m_UnitDataLength;

};

