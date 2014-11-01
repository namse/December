#pragma once
#include "UnitData.h"
class Unit
{
public:
	Unit();
	virtual ~Unit();

	void InitObject(UnitData unitData, PlayerNumber ownerPlayerNum);
	void SetOwner(PlayerNumber playerNumber) { m_OwnerPlayerNum = playerNumber; }
	void SetPosition(Coord position) { m_Position = position; }
protected:
	PlayerNumber	m_OwnerPlayerNum;

	UnitType				m_UnitType;
	UnitMoveType		m_UnitMoveType;
	int					m_HP;
	int					m_Attack;
	int					m_Weight;
	int					m_MoveRange;

	Coord				m_Position;
};

