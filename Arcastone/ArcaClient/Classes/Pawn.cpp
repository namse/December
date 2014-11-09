#include "Pawn.h"

Pawn::Pawn()
{
}

Pawn::~Pawn()
{
}

void Pawn::initSprite()
{
	assert(m_Owner != UO_NONE);
	if (m_Owner == UO_ME) m_Sprite = Sprite::create("Me.png");
	else if (m_Owner == UO_ENEMY) m_Sprite = Sprite::create("Enemy.png");
	else if (m_Owner == UO_NPC) m_Sprite = Sprite::create("Me.png");
}