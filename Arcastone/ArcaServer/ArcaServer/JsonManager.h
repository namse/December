#pragma once

class JsonManager
{


public:
	JsonManager();
	~JsonManager();

private:
	void WriteJson();

	Packet::GameStartResult::UnitData m_UnitData[MAX_UNIT_TYPE];
	int			m_UnitTypeCount;

};

extern JsonManager* GJsonManager;