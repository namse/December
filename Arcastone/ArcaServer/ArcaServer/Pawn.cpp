#include "stdafx.h"
#include "Pawn.h"


Pawn::Pawn()
{
	m_Name = "Pawn";
	m_HP = 30;
	m_Attack = 2;
	m_Weight = 0;
	m_MoveRange = 2;
	m_UnitType = UC_PAWN;
	m_UnitMoveType = MT_STRAIGHT;
}

Pawn::~Pawn()
{
}

