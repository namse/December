#pragma once
#include "UnitData.h"
class Unit
{
public:
	Unit();
	virtual ~Unit();

	void			InitUnit(UnitData unitData, PlayerNumber ownerPlayerNum);
	void			SetOwner(PlayerNumber playerNumber) { m_OwnerPlayerNum = playerNumber; }
	void			SetPosition(Coord position) { m_Position = position; }
	void			SetHP(int hp) { m_HP = hp; }

	UnitType		GetUnitType(){ return m_UnitType; }
	UnitMoveType	GetUnitMoveType(){ return m_UnitMoveType; }
	PlayerNumber	GetOwner(){ return m_OwnerPlayerNum; }
	int				GetHP(){ return m_HP; }
	int				GetAttack(){ return m_Attack; }
	int				GetWeight(){ return m_Weight; }
	int				GetMoveRange(){ return m_MoveRange; }
	Coord			GetPos(){ return m_Position; }

protected:
	PlayerNumber	m_OwnerPlayerNum;

	UnitType		m_UnitType;
	UnitMoveType	m_UnitMoveType;
	int				m_HP;
	int				m_Attack;
	int				m_Weight;
	int				m_MoveRange;

	Coord			m_Position;
};

