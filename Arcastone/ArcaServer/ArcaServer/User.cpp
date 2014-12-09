#include "stdafx.h"
#include "User.h"
#include "JsonManager.h"

User::User(UserNumber userNum) : m_UserNum(userNum)
{
	// 임시로 유닛 데이터 넣는 곳 임다

	Group group;
//	group.AddUnitData(UT_SOLDIER, Coord(2, 4));
	group.AddUnitData(UT_SOLDIER, Coord(4, 2));
	group.AddUnitData(UT_SOLDIER, Coord(2, 3));
//	group.AddUnitData(UT_SOLDIER, Coord(4, 3));
	group.AddUnitData(UT_KNIGHT, Coord(3, 2));
//	group.AddUnitData(UT_SOLDIER, Coord(3, 3));

	m_GroupList.push_back(group);
}


User::~User()
{
}