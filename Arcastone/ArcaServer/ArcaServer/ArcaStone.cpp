#include "stdafx.h"
#include "ArcaStone.h"


ArcaStone::ArcaStone()
{
	m_HP = 30;
	m_Attack = 2;
	m_Weight = 1;
	m_MoveRange = 0;
	m_UnitMoveType = UMT_NONE;
	m_UnitType = UT_ARCASTONE;
}


ArcaStone::~ArcaStone()
{
}
