#include "stdafx.h"
#include "Player.h"

Player::Player(PlayerNumber playerNum) : m_PlayerNum(playerNum)
{
	UnitData data;

	data.m_HP = 30;
	data.m_Attack = 2;
	data.m_Weight = 0;
	data.m_MoveRange = 2;
	data.m_UnitType = UT_PAWN;
	data.m_UnitMoveType = UMT_STRAIGHT;

	m_UnitList.push_back(data);

	SetGroupForTest();
}


Player::~Player()
{
}

void Player::SetGroupForTest()
{
	Group group;

	for (int col = 0; col < 3; col++)
	{
		for (int row = 0; row < 2; row++)
		{
			UnitData pawn = m_UnitList[0];
			auto originPosition = Coord(col - 1, row - (int)(col / 2));
			group.AddUnitData(pawn, originPosition);
		}
	}
}
