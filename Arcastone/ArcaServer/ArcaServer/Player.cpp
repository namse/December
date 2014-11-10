#include "stdafx.h"
#include "Player.h"
#include "ParsingData.h"

Player::Player(PlayerNumber playerNum) : m_PlayerNum(playerNum)
{
	// TODO : 
	UnitData data;
	std::vector<UnitData> dataList;
	Group group;

	{
		// TODO : 플레이어가 자신의 유닛 배치하는 씬 만들 때..
		data = GParsingData->GetUnitData(UT_SOLDIER);
		data.m_Position = Coord(2, 4);
		dataList.push_back(UnitData(data));
		data.m_Position = Coord(3, 3);
		dataList.push_back(UnitData(data));
		data.m_Position = Coord(4, 3);
		dataList.push_back(UnitData(data));

		data = GParsingData->GetUnitData(UT_KNIGHT);

		data.m_Position = Coord(2, 3);
		dataList.push_back(UnitData(data));
		data.m_Position = Coord(4, 2);
		dataList.push_back(UnitData(data));

		data = GParsingData->GetUnitData(UT_RIDER);

		data.m_Position = Coord(3, 2);
		dataList.push_back(UnitData(data));
	}

	group.SetUnitList(dataList);

	SetGroup(group);
}


Player::~Player()
{
}