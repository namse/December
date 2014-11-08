#include "Unit.h"
#include "Pawn.h"

Unit::~Unit()
{
}

Unit* Unit::create(UnitData unitData)
{
	Unit* newUnit = nullptr;
	switch (unitData.unitType)
	{
	case UT_PAWN:
	{
					newUnit = new Pawn();
	}break;
	default:
	{
			   newUnit = new Unit();
	}break;
	}

	if (newUnit != nullptr) newUnit->init(unitData);
	return newUnit;
}

void Unit::init(UnitData unitData)
{
	m_UnitType = unitData.unitType;
	m_MoveType = unitData.unitMoveType;
	m_Owner = unitData.unitOwner;
	m_State = US_NORMAL;

	m_HP = unitData.hp;
	m_Attack = unitData.attack;
	m_MoveRange = unitData.moveRange;
	m_Weight = unitData.weight;

	m_Sprite = nullptr;
}

bool Unit::setPosition(HexaPoint setPos)
{
	// TODO : ���� �׸��� ���(hexa ���� rect ����)�� ���� ���ǹ� �ٸ��� �� ��
	if ((setPos.x < 0 || setPos.x > MAP_SIZEX) || (setPos.y < 0 || setPos.y > MAP_SIZEY))
		return false;

	m_Position = setPos;
	return true;
}
