#include "stdafx.h"
#include "ArcaStone.h"


ArcaStone::ArcaStone()
{
	m_Name = "Arcastone";
	m_HP = 30;
	m_Attack = 2;
	m_Weight = 1;
	m_MoveRange = 0;
	m_NPCType = NT_ARCASTONE;
	m_UnitMoveType = MT_NONE;
}


ArcaStone::~ArcaStone()
{
}
