#include "stdafx.h"
#include "Unit.h"


Unit::Unit()
{
	m_UnitType = UT_NONE;
	m_UnitMoveType = UMT_NONE;
	m_HP = m_Attack = m_Weight = m_MoveRange = -1;
	m_UnitStatusType = UST_NONE;
}

Unit::~Unit()
{
}

void Unit::InitUnit(UnitData unitData, PlayerNumber ownerPlayerNum, UnitIdentityNumber id)
{
	m_UnitStatusType = UST_NONE;

	m_UnitType = unitData.m_UnitType;
	m_UnitMoveType = unitData.m_UnitMoveType;
	m_MoveRange = unitData.m_MoveRange;

	m_HP = unitData.m_HP;
	m_Attack = unitData.m_Attack;
	m_Weight = unitData.m_Weight;

	SetOwner(ownerPlayerNum);

	m_ID = id;
}
