#include "stdafx.h"
#include "Player.h"
#include "JsonManager.h"

Player::Player(PlayerNumber playerNum) : m_PlayerNum(playerNum)
{
	// 임시로 유닛 데이터 넣는 곳 임다

	/*
	UnitData data[6];

	data[0] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[0].m_Position = Coord(2, 3);

	data[1] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[1].m_Position = Coord(4, 2);

	data[2] = GJsonManager->GetUnitData(UT_RIDER);
	data[2].m_Position = Coord(2, 4);

	data[3] = GJsonManager->GetUnitData(UT_RIDER);
	data[3].m_Position = Coord(4, 3);

	data[4] = GJsonManager->GetUnitData(UT_KNIGHT);
	data[4].m_Position = Coord(3, 2);

	data[5] = GJsonManager->GetUnitData(UT_MAGICIAN);
	data[5].m_Position = Coord(3, 3);

	for (int i = 0; i < 6; ++i)
	{
	group.AddUnitData(data[i], data[i].m_Position);
	}
	*/

	Group group;
	group.AddUnitData(UT_SOLDIER, Coord(2, 3));
	group.AddUnitData(UT_SOLDIER, Coord(4, 2));
	group.AddUnitData(UT_RIDER, Coord(2, 4));
	group.AddUnitData(UT_RIDER, Coord(4, 3));
	group.AddUnitData(UT_KNIGHT, Coord(3, 2));
	group.AddUnitData(UT_MAGICIAN, Coord(3, 3));
	m_GroupList.push_back(group);
}


Player::~Player()
{
}