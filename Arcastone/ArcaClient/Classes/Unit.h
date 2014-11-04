#pragma once

#include "Header.h"
#include "Header.h"

class Unit
{
public:
	Unit();
	~Unit();

	void						setUnitStatus(UnitData args){ m_UnitStatus = args; }

	void						setHp(int args){ m_UnitStatus.hp = args; }
	void						setAtk(int args){ m_UnitStatus.attack = args; }
	void						setMoveRange(int args){ m_UnitStatus.moveRange = args; }
	void						setWeight(int args){ m_UnitStatus.weight = args; }
	void						setUnitType(UnitType args){ m_UnitStatus.unitType = args; }
	void						setMoveType(UnitMoveType args){ m_UnitStatus.unitMoveType = args; }
	void						setUnitOwner(UnitOwner args){ m_UnitStatus.unitOwner = args; }

	bool						setPosition(HexaPoint args);

	UnitData					getUnitStatus(){ return m_UnitStatus; }
	
private:
	UnitData					m_UnitStatus;

};

