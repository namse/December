#include "stdafx.h"
#include "JsonManager.h"

JsonManager* GJsonManager = nullptr;

JsonManager::JsonManager()
{
	// unit.json���Ͽ��� ���� ������ �о�ɴϴ�
	std::ifstream pJsonData;
	pJsonData.open("unit.json");

	if (pJsonData.bad())
	{
		printf_s("failed to open unit.json");
	}

	std::string jsonDataString(
		std::istreambuf_iterator<char>(pJsonData.rdbuf()),
		std::istreambuf_iterator<char>());
	pJsonData.close();

	// ���� ������ Json ��ü�� �Ľ��մϴ�
	Json::Value root;
	Json::Reader reader;
	Json::Value dataList;

	reader.parse(jsonDataString,root);
	dataList = root["data_list"];
	m_UnitDataCount = root["data_num"].asInt();

	for (int i = 0; i < m_UnitDataCount; ++i)
	{
		m_UnitData[i].m_UnitType = (UnitType)atoi(dataList[i]["unit_type"].asCString());
		m_UnitData[i].m_UnitMoveType = (UnitMoveType)atoi(dataList[i]["unit_move_type"].asCString());
		m_UnitData[i].m_MoveRange = atoi(dataList[i]["unit_move_range"].asCString());
		m_UnitData[i].m_Attack = atoi(dataList[i]["unit_attack"].asCString());
		m_UnitData[i].m_HP = atoi(dataList[i]["unit_hp"].asCString());
		m_UnitData[i].m_Weight = atoi(dataList[i]["unit_weight"].asCString());
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
