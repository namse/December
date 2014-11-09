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
	if (m_Owner == UO_ME) m_Sprite->addChild(Sprite::create("Me.png"));
	else if (m_Owner == UO_ENEMY) m_Sprite->addChild(Sprite::create("Enemy.png"));
	else if (m_Owner == UO_NPC) m_Sprite->addChild(Sprite::create("Me.png"));
}