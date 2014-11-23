﻿#pragma once
#include "UnitData.h"
class Unit
{
public:
	Unit();
	virtual ~Unit();

	static Unit*			CreateUnit(UnitType unitType);
	void					InitUnit(UnitType unitType);

	void					SetId(UnitIdentityNumber unitId){ m_ID = unitId; }
	void					SetOwner(PlayerNumber playerNumber) { m_OwnerPlayerNum = playerNumber; }
	void					SetPosition(Coord position) { m_Position = position; }
	void					SetHP(int hp) { m_HP = hp; }
	void					setStatus(UnitStatusType status){ m_UnitStatus = status; }

	UnitType				GetUnitType(){ return m_UnitType; }
	UnitStatusType			GetUnitStatus(){ return m_UnitStatus; }
	UnitMoveType			GetUnitMoveType(){ return m_UnitMoveType; }
	PlayerNumber			GetOwner(){ return m_OwnerPlayerNum; }
	int						GetHP(){ return m_HP; }
	int						GetAttack(){ return m_Attack; }
	int						GetWeight(){ return m_Weight; }
	int						GetMoveRange(){ return m_MoveRange; }
	Coord					GetPos(){ return m_Position; }
	UnitIdentityNumber		GetID(){ return m_ID; }

	

protected:
	PlayerNumber			m_OwnerPlayerNum;

	UnitType				m_UnitType;
	UnitStatusType			m_UnitStatus;	// 한번에 하나의 상태만 가질 수 있다
	UnitMoveType			m_UnitMoveType;
	int						m_HP;
	int						m_Attack;
	int						m_Weight;
	int						m_MoveRange;
	UnitIdentityNumber		m_ID;


	Coord					m_Position;
};

