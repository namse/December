#include "Soldier.h"

Soldier::Soldier()
{
}

Soldier::~Soldier()
{
}

void Soldier::initSprite()
{
	Unit::initSprite();
	assert(m_Owner != UO_NONE && "unknown unit owner");

	Sprite* unitSprite;
	unitSprite = Sprite::create("character.png");
	
	unitSprite->setScale(HEXAGON_LENGTH*1.5 / unitSprite->getContentSize().width);
	unitSprite->setAnchorPoint(Vec2(0.5f, 0.3f));

	m_Sprite->addChild(unitSprite);
}