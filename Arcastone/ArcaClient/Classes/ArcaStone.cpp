#include "ArcaStone.h"


ArcaStone::ArcaStone()
{
}


ArcaStone::~ArcaStone()
{
}

void ArcaStone::initSprite()
{
	Unit::initSprite();
	assert(m_Owner == UO_NPC);

	Sprite* unitSprite;
	unitSprite = Sprite::create("npc_arcastone.png");

	unitSprite->setScale(HEXAGON_LENGTH*1.3 / unitSprite->getContentSize().width);
	unitSprite->setAnchorPoint(Vec2(0.5f, 0.3f));

	m_Sprite->addChild(unitSprite);
}
