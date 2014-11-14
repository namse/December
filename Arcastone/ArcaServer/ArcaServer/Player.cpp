#include "stdafx.h"
#include "Player.h"
#include "JsonManager.h"

Player::Player(PlayerNumber playerNum) : m_PlayerNum(playerNum)
{
	// �ӽ÷� ���� ������ �ִ� �� �Ӵ�

	UnitData data[6];

	data[0] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[0].m_Position = Coord(2, 3);

	data[1] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[1].m_Position = Coord(4, 2);

	/*
	data[2] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[2].m_Position = Coord(2, 4);

	data[3] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[3].m_Position = Coord(3, 2);

	data[4] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[4].m_Position = Coord(3, 3);

	data[5] = GJsonManager->GetUnitData(UT_SOLDIER);
	data[5].m_Position = Coord(4, 3);
	*/
	
	data[2] = GJsonManager->GetUnitData(UT_RIDER);
	data[2].m_Position = Coord(2, 4);

	data[3] = GJsonManager->GetUnitData(UT_RIDER);
	data[3].m_Position = Coord(4, 3);

	data[4] = GJsonManager->GetUnitData(UT_KNIGHT);
	data[4].m_Position = Coord(3, 2);

	data[5] = GJsonManager->GetUnitData(UT_BISHOP);
	data[5].m_Position = Coord(3, 3);


	Group group;
	for (int i = 0; i < 6; ++i)
	{
		group.AddUnitData(data[i], data[i].m_Position);
	}
	m_GroupList.push_back(group);
}


Player::~Player()
{
}