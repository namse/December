#include "stdafx.h"
#include "ParsingData.h"

ParsingData* GParsingData = nullptr;

ParsingData::ParsingData()
{
	Json::Value unit;
	// 각 유닛 데이터를 배열에 담는다
	Json::Value unitList(Json::arrayValue);

	// 유닛 종류의 수 세기
	int m_UnitTypeCount = 0;

#pragma region ParseData
	{
		// json 파일이 없으므로 일단 우겨넣음 .
		unit["name"] = UT_ARCASTONE;
		unit["MoveType"] = UMT_NONE;
		unit["MoveRange"] = 0;
		unit["Atk"] = 6;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["name"] = UT_SOLDIER;
		unit["MoveType"] = UMT_STRAIGHT;
		unit["MoveRange"] = 2;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 0;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["name"] = UT_RIDER;
		unit["MoveType"] = UMT_JUMP;
		unit["MoveRange"] = 4;
		unit["Atk"] = 2;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unitList.append(unit);
		m_UnitTypeCount++;

		unit["name"] = UT_KNIGHT;
		unit["MoveType"] = UMT_DASH;
		unit["MoveRange"] = 3;
		unit["Atk"] = 3;
		unit["Hp"] = 30;
		unit["Weight"] = 1;
		unitList.append(unit);
		m_UnitTypeCount++;
	}
#pragma endregion

	Json::Reader reader;
	Json::Value parsingList;

	reader.parse(unitList.toStyledString(), parsingList);

	for (int i = 0; i < m_UnitTypeCount; ++i)
	{
		UnitData unitData;
		unitData.m_UnitType = (UnitType)parsingList[i]["name"].asInt();
		unitData.m_UnitMoveType = (UnitMoveType)parsingList[i]["MoveType"].asInt();
		unitData.m_MoveRange = (int)parsingList[i]["MoveRange"].asInt();
		unitData.m_Attack = (int)parsingList[i]["Atk"].asInt();
		unitData.m_HP = (int)parsingList[i]["Hp"].asInt();
		unitData.m_Weight = (int)parsingList[i]["weight"].asInt();

		m_UnitList.insert(std::map<UnitType, UnitData>::value_type(unitData.m_UnitType, unitData));
	}
}


ParsingData::~ParsingData()
{
}

UnitData ParsingData::GetUnitData(UnitType unitType)
{
	UnitData retUnitData = UnitData();

	std::map<UnitType, UnitData >::iterator findIter = m_UnitList.find(unitType);

	// 찾았다면 리턴
	if (findIter != m_UnitList.end())
	{
		retUnitData = findIter->second;
	}
	return retUnitData;
}