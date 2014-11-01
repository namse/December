#pragma once

#include "Header.h"
#include "Header.h"

class Unit
{
public:
	Unit();
	~Unit();

	void setHp(int args){ m_UnitStatus.Hp = args; }
	void setAtk(int args){ m_UnitStatus.Atk = args; }
	void setMoveRange(int args){ m_UnitStatus.MoveRange = args; }
	void setWeight(int args){ m_UnitStatus.Weight = args; }

	bool setPieceType(PieceType args){ m_UnitStatus.PieceType = args; }
	bool setMoveType(MoveType args){ m_UnitStatus.MoveType = args; }
	bool setAttackType(AttackType args){ m_UnitStatus.AttackType = args; }

	bool setPosition(Position args);

	Position getPosition(){ return m_Position; }
	UnitStatus getUnitStatus(){ return m_UnitStatus; }
	
private:
	UnitStatus m_UnitStatus;
	Position m_Position;

};

