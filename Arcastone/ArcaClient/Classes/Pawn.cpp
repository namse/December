#include "Pawn.h"

Pawn::Pawn()
{
}

Pawn::~Pawn()
{
}

void Pawn::initSprite()
{
	Unit::initSprite();
	assert(m_Owner != UO_NONE);

	Sprite* unitSprite;
	if (m_Owner == UO_ME) unitSprite = Sprite::create("character.png");
	else if (m_Owner == UO_ENEMY) unitSprite = Sprite::create("character2.png");
	else if (m_Owner == UO_NPC) unitSprite = Sprite::create("character.png");

	unitSprite->setScale(HEXAGON_LENGTH*1.5 / unitSprite->getContentSize().width);
	unitSprite->setAnchorPoint(Vec2(0.5f, 0.3f));

	m_Sprite->addChild(unitSprite);
}