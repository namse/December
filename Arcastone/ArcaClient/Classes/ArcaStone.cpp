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
	Sprite* ora = Sprite::create("arca_ora.png");

	unitSprite->setScale(HEXAGON_LENGTH*1.3 / unitSprite->getContentSize().width);
	unitSprite->setAnchorPoint(Vec2(0.5f, 0.3f));

	ora->setScale(0.5);
	ora->setAnchorPoint(Vec2(0.48f, 0.3f));
	ora->setOpacity(64);

	m_Sprite->addChild(ora);
	m_Sprite->addChild(unitSprite);
}
