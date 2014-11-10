#include "stdafx.h"
#include "JsonManager.h"

JsonManager* GJsonManager = nullptr;

JsonManager::JsonManager()
{
	Json::Value unit;
	// 각 유닛 데이터를 배열에 담는다
	Json::Value unitList(Json::arrayValue);

	// 유닛 종류의 수 세기
	m_UnitDataCount = 0;

#pragma region data
	{
		// json 파일이 없으므로 일단 우겨넣음 .
		unit["name"] = UT_ARCASTONE;
		unit["MoveType"] = UMT_NONE;
		unit["MoveRange"] = 0;
		unit["Atk"] = 6;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unitList.append(unit);
		m_UnitDataCount++;

		unit["name"] = UT_SOLDIER;
		unit["MoveType"] = UMT_STRAIGHT;
		unit["MoveRange"] = 2;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unitList.append(unit);
		m_UnitDataCount++;

		unit["name"] = UT_RIDER;
		unit["MoveType"] = UMT_JUMP;
		unit["MoveRange"] = 4;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unitList.append(unit);
		m_UnitDataCount++;

		unit["name"] = UT_KNIGHT;
		unit["MoveType"] = UMT_DASH;
		unit["MoveRange"] = 3;
		unit["Atk"] = 3;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unitList.append(unit);
		m_UnitDataCount++;

		unit["name"] = UT_BISHOP;
		unit["MoveType"] = UMT_TELEPORT;
		unit["MoveRange"] = 4;
		unit["Atk"] = 6;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unitList.append(unit);
		m_UnitDataCount++;
	}
#pragma endregion

	Json::Reader reader;
	Json::Value parsingList;

	reader.parse(unitList.toStyledString(), parsingList);

	for (int i = 0; i < m_UnitDataCount; ++i)
	{
		m_UnitData[i].m_UnitType = (UnitType)parsingList[i]["name"].asInt();
		m_UnitData[i].m_UnitMoveType = (UnitMoveType)parsingList[i]["MoveType"].asInt();
		m_UnitData[i].m_MoveRange = (int)parsingList[i]["MoveRange"].asInt();
		m_UnitData[i].m_Attack = (int)parsingList[i]["Atk"].asInt();
		m_UnitData[i].m_HP = (int)parsingList[i]["Hp"].asInt();
		m_UnitData[i].m_Weight = (int)parsingList[i]["weight"].asInt();
	}
}


JsonManager::~JsonManager()
{
}

UnitData JsonManager::GetUnitData(UnitType unitType)
{
	UnitData retUnitData = UnitData();
	
	for (int i = 0; i < m_UnitDataCount; ++i)
	{
		if (m_UnitData[i].m_UnitType == unitType)
		{
			retUnitData = m_UnitData[i];
			return retUnitData;
		}
	}
	return retUnitData;
}