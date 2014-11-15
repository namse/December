#pragma once

#include "UnitData.h"

class JsonManager
{
public:
	JsonManager();
	~JsonManager();
	
	UnitData				GetUnitData(UnitType unitType);
	int						GetUnitDataCount(){ return m_UnitDataCount; }

private:

	UnitData				m_UnitData[100];
	int						m_UnitDataCount;

};

extern JsonManager* GJsonManager;