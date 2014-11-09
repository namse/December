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
	m_ID = unitData.id;
	m_Attack = unitData.attack;
	m_MoveRange = unitData.moveRange;
	m_Weight = unitData.weight;

	m_Sprite = nullptr;

	initSprite();
	setPosition(HexaPoint(unitData.point));
}

bool Unit::setPosition(HexaPoint setPos)
{
	// TODO : 맵을 그리는 방식(hexa 인지 rect 인지)에 따라 조건문 다르게 할 것
	if ((setPos.x < 0 || setPos.x > MAP_SIZEX) || (setPos.y < 0 || setPos.y > MAP_SIZEY))
		return false;

	m_Position = setPos;
	return true;
}

void Unit::initSprite()
{
	m_Sprite = Sprite::create();

	// 체력과 공격력 표시 
	char buff[5];
	itoa(m_HP, buff, 10);
	auto labelHP = LabelTTF::create(buff, "Helvetica", 12);

	itoa(m_Attack, buff, 10);
	auto labelAtk = LabelTTF::create(buff, "Helvetica", 12);

	labelHP->setPosition(20, 0);
	labelAtk->setPosition(-20, 0);

	labelHP->setColor(Color3B(255, 0, 0));
	labelAtk->setColor(Color3B(255, 255, 255));

	m_Sprite->addChild(labelHP,10,"hp");
	m_Sprite->addChild(labelAtk,10,"atk");
}

void Unit::setHP(int hp)
{
	m_HP = hp;

	// 스프라이트의 체력 수정
	char buff[5];
	itoa(m_HP, buff, 10);

	auto labelHP = LabelTTF::create(buff, "Helvetica", 12);
	labelHP->setString(buff);
}

