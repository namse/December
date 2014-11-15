﻿#include "Unit.h"
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

void Unit::initSprite()
{
	m_Sprite = Sprite::create();
 
	// 체력/공격력 프레임 표시
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

	// 체력과 공격력 숫자 표시
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

void Unit::SetHP(int hp)
{
	m_HP = hp;

	// 스프라이트의 체력 수정
	char buff[5];
	itoa(m_HP, buff, 10);

	auto labelHP = LabelTTF::create(buff, "Helvetica", 12);
	labelHP->setString(buff);
}

