#include "stdafx.h"
#include "JsonManager.h"


JsonManager::JsonManager()
{
}


JsonManager::~JsonManager()
{
}

void JsonManager::WriteJson()
{
	Json::Value unit;
	// 각 유닛 데이터를 배열에 담는다
	Json::Value unitList(Json::arrayValue);

	// 유닛 종류의 수 세기
	m_UnitTypeCount = 0;

	{
		// json 파일이 없으므로 일단 우겨넣음 .
		unit["UnitType"] = UT_ARCASTONE;
		unit["Owner"] = UO_NONE;
		unit["MoveType"] = UMT_NONE;
		unit["MoveRange"] = 0;
		unit["Atk"] = 6;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unit["Status"] = UST_NONE;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["UnitType"] = UT_SOLDIER;
		unit["Owner"] = UO_NONE;
		unit["MoveType"] = UMT_STRAIGHT;
		unit["MoveRange"] = 2;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unit["Status"] = UST_NONE;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["name"] = UT_RIDER;
		unit["Owner"] = UO_NONE;
		unit["MoveType"] = UMT_JUMP;
		unit["MoveRange"] = 4;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unit["Status"] = UST_NONE;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["name"] = UT_KNIGHT;
		unit["Owner"] = UO_NONE;
		unit["MoveType"] = UMT_DASH;
		unit["MoveRange"] = 3;
		unit["Atk"] = 3;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unit["Status"] = UST_NONE;
		unitList.append(unit);
		m_UnitTypeCount++;
	}

	Json::Reader reader;
	Json::Value parsingList;

	reader.parse(unitList.toStyledString(), parsingList);

	for (int i = 0; i < m_UnitTypeCount; ++i)
	{
		m_UnitData[i].unitType = (UnitType)parsingList[i]["name"].asInt();
		m_UnitData[i].unitOwner = (UnitOwner)parsingList[i]["Owner"].asInt();
		m_UnitData[i].unitMoveType = (UnitMoveType)parsingList[i]["MoveType"].asInt();
		m_UnitData[i].moveRange = (int)parsingList[i]["MoveRange"].asInt();
		m_UnitData[i].attack = (int)parsingList[i]["Atk"].asInt();
		m_UnitData[i].hp = (int)parsingList[i]["Hp"].asInt();
		m_UnitData[i].weight = (int)parsingList[i]["weight"].asInt();
		m_UnitData[i].unitStatusType = (UnitStatusType)parsingList[i]["Status"].asInt();
	}
}