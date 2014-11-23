#include "stdafx.h"
#include "Unit.h"
#include "Soldier.h"
#include "Magician.h"
#include "Knight.h"
#include "Prince.h"
#include "Princess.h"
#include "ArcaStone.h"
#include "Rider.h"
#include "Rock.h"
#include "JsonManager.h"


Unit::Unit()
{
	m_UnitType = UT_NONE;
	m_UnitMoveType = UMT_NONE;
	m_HP = m_Attack = m_Weight = m_MoveRange = -1;
	m_UnitStatus = UST_NONE;
}

Unit::~Unit()
{
}


Unit* Unit::CreateUnit(UnitType unitType)
{
	Unit* newUnit;

	switch (unitType)
	{
	case UT_SOLDIER:
	{
		newUnit = new Soldier();
	}break;
	case UT_MAGICIAN:
	{
		newUnit = new Magician();
	}break;
	case UT_KNIGHT:
	{
		newUnit = new Knight();
	}break;
	case UT_PRINCE:
	{
		newUnit = new Prince();
	}break;
	case UT_PRINCESS:
	{
		newUnit = new Princess();
	}break;
	case UT_ARCASTONE:
	{
		newUnit = new ArcaStone();
	}break;
	case UT_RIDER:
	{
		newUnit = new Rider();
	}break;
	case UT_ROCK:
	{
		newUnit = new Rock();
	}break;
	default:
		assert(false && "Unit::create - Non defined new unitType");
	}

	if(newUnit != nullptr) newUnit->InitUnit(unitType);

	return newUnit;
}


void Unit::InitUnit(UnitType unitType)
{
	UnitData unitData = GJsonManager->GetUnitData(unitType);
	
	m_UnitType = unitType;
	m_UnitMoveType = unitData.m_UnitMoveType;
	m_HP = unitData.m_HP;
	m_Attack = unitData.m_Attack;
	m_Weight = unitData.m_Weight;
	m_MoveRange = unitData.m_MoveRange;
}

