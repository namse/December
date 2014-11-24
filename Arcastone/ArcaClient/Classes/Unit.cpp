#include "Unit.h"
#include "Soldier.h"
#include "Rider.h"
#include "Knight.h"
#include "Magician.h"
#include "Prince.h"
#include "Princess.h"
#include "ArcaStone.h"
#include "Rock.h"
#include "NPC.h"

Unit::Unit()
{
}

Unit::~Unit()
{
}

Unit* Unit::create(UnitData unitData)
{
	Unit* newUnit = nullptr;
	switch (unitData.unitType)
	{
		case UT_SOLDIER:
		{
							newUnit = new Soldier();
		}break;
		case UT_RIDER:
		{
						   newUnit = new Rider();
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
		case UT_ROCK:
		{
							newUnit = new Rock();
		}break;
		case UT_MAGICIAN:
		{
						   newUnit = new Magician();
		}break;
		case UT_ARCASTONE:
		{
							 newUnit = new ArcaStone();
		}break;
		default:
			assert(false && "Unit::create - Non defined new unitType");
	}

	if (newUnit != nullptr) newUnit->init(unitData);

	return newUnit;
}

void Unit::init(UnitData unitData)
{
	m_UnitType = unitData.unitType;
	m_MoveType = unitData.unitMoveType;
	//m_MoveType = UMT_TELEPORT;
	m_Owner = unitData.unitOwner;
	m_StatusType = UST_NONE;

	m_HP = unitData.hp;
	m_ID = unitData.id;
	m_Attack = unitData.attack;
	m_MoveRange = unitData.moveRange;
	m_Weight = unitData.weight;

	m_Skill = Skill();
	// TODO : Json 에서 스킬 데이터 읽어오도록

	if (m_UnitType == UT_MAGICIAN) // 비숍
	{
		m_Skill = Skill(USK_FIREBALL);
		// 랭크1 파이어볼!
	}

	m_Sprite = nullptr;

	initSprite();
	setPosition(HexaPoint(unitData.point));
}

void Unit::initSprite()
{
	m_Sprite = Sprite::create();

	// 체력/공격력 프레임 표시
	auto frameHP = Sprite::create("frame_hp.png");
	auto frameAtk = Sprite::create("frame_atk.png");

	float scale = (HEXAGON_LENGTH * 3 / 4) / frameHP->getContentSize().width;
	float position = HEXAGON_LENGTH * 3 / 4;

	frameHP->setScale(scale);
	frameAtk->setScale(scale);
	
	frameHP->setPosition(position, 0);
	frameAtk->setPosition(-position, 0);

	m_Sprite->addChild(frameHP, 10, "frame_hp");
	m_Sprite->addChild(frameAtk, 10, "frame_atk");

	// 체력과 공격력 숫자 표시
	char buff[5];
	itoa(m_HP, buff, 10);
	m_HpLabel = LabelTTF::create(buff, "Helvetica", 15);

	itoa(m_Attack, buff, 10);
	m_AtkLabel = LabelTTF::create(buff, "Helvetica", 15);

	m_HpLabel->setPosition(position, 0);
	m_AtkLabel->setPosition(-position, 0);

	m_HpLabel->setColor(Color3B(255, 255, 255));
	m_AtkLabel->setColor(Color3B(255, 255, 255));
	
	m_Sprite->addChild(m_HpLabel, 20, "hp");
	m_Sprite->addChild(m_AtkLabel, 20, "atk");

	// 자신 유닛인지, 상대 유닛인지 표시 
	assert(m_Owner != UO_NONE && "undefined unit owner");
	if (m_Owner != UO_NPC)
	{
		Sprite* ownerMark;
		if (m_Owner == UO_ME)
		{
			ownerMark = Sprite::create("myunit.png");
		}
		else if (m_Owner == UO_ENEMY)
		{
			ownerMark = Sprite::create("enemyunit.png");
		}
		float scale = (HEXAGON_LENGTH) / ownerMark->getContentSize().width;
		ownerMark->setScale(scale);
		ownerMark->setPositionY(ownerMark->getContentSize().height*scale*1.5);
		m_Sprite->addChild(ownerMark, 30 + ZORDER_STAT, "ownerMark");
	}
}

void Unit::SetHP(int hp)
{
	m_HP = hp;

	// 스프라이트의 체력 수정
	char buff[5];
	itoa(m_HP, buff, 10);

	m_HpLabel->setString(buff);
}

void Unit::setScreenPos(ScreenPoint setPos)
{
	assert(m_Sprite != nullptr && "unit sprite not created!");
	int zOder = ZORDER_CHARACTER + ((DISPLAY_SIZEY - setPos.y) / 5) + ((DISPLAY_SIZEX - setPos.x) / 10);
	m_Sprite->setZOrder(zOder);
	m_Sprite->setPosition(setPos);
}