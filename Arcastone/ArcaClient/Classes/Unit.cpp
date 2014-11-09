#include "Unit.h"
#include "Pawn.h"
#include "NPC.h"

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
	case UT_ARCASTONE:
	{
						 newUnit = new NPC();
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
	// TODO : ���� �׸��� ���(hexa ���� rect ����)�� ���� ���ǹ� �ٸ��� �� ��
	if ((setPos.x < 0 || setPos.x > MAP_SIZEX) || (setPos.y < 0 || setPos.y > MAP_SIZEY))
		return false;

	m_Position = setPos;
	return true;
}

void Unit::initSprite()
{
	m_Sprite = Sprite::create();
 
	// ü��/���ݷ� ������ ǥ��
	auto frameHP = Sprite::create("frame_hp.png");
	auto frameAtk = Sprite::create("frame_atk.png");

	float scale = (HEXAGON_LENGTH * 3 / 4) / frameHP->getContentSize().width;
	float position = HEXAGON_LENGTH * 3 / 4;

	int zIdx = 5;

	frameHP->setScale(scale);
	frameAtk->setScale(scale);
	
	frameHP->setPosition(position, 0);
	frameAtk->setPosition(-position, 0);

	m_Sprite->addChild(frameHP, zIdx, "frame_hp");
	m_Sprite->addChild(frameAtk, zIdx, "frame_atk");

	// ü�°� ���ݷ� ���� ǥ��
	char buff[5];
	itoa(m_HP, buff, 10);
	auto labelHP = LabelTTF::create(buff, "Helvetica", 15);

	itoa(m_Attack, buff, 10);
	auto labelAtk = LabelTTF::create(buff, "Helvetica", 15);

	labelHP->setPosition(position, 0);
	labelAtk->setPosition(-position, 0);

	labelHP->setColor(Color3B(255, 255, 255));
	labelAtk->setColor(Color3B(255, 255, 255));
	
	zIdx = 10;
	m_Sprite->addChild(labelHP, zIdx, "hp");
	m_Sprite->addChild(labelAtk, zIdx, "atk");
}

void Unit::setHP(int hp)
{
	m_HP = hp;

	// ��������Ʈ�� ü�� ����
	char buff[5];
	itoa(m_HP, buff, 10);

	auto labelHP = LabelTTF::create(buff, "Helvetica", 12);
	labelHP->setString(buff);
}

