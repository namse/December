#include "Rock.h"


Rock::Rock()
{
}


Rock::~Rock()
{
}

void Rock::initSprite()
{
	Unit::initSprite();
	assert(m_Owner == UO_NPC);

	Sprite* unitSprite;
	unitSprite = Sprite::create("frame_atk.png");

	unitSprite->setScale(HEXAGON_LENGTH*1.5 / unitSprite->getContentSize().width);
	unitSprite->setAnchorPoint(Vec2(0.5f, 0.3f));

	m_Sprite->addChild(unitSprite);
}
