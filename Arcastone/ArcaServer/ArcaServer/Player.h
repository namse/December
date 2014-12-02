#pragma once

#include "Unit.h"
#include "TurnManager.h"

class Game;
class Player
{
public:
	Player();
	~Player();

	/*  Player  */
	UserNumber				GetUserNumber(){ return m_UserNumber; }
	void					SetUserNumber(UserNumber number){ m_UserNumber = number; }
	PlayerNumber			GetPlayerNumber(){ return m_PlayerNumber; }
	void					SetPlayerNumber(PlayerNumber number){ m_PlayerNumber = number; }


	/*  Unit  */
	std::vector<Unit>*		GetUnitList(){ return &m_UnitList; }
	Unit*					GetUnit(int id);
	void					SetUnit(Unit* unit){ if (unit != nullptr)m_UnitList.push_back(*unit); }


	/*  Cost  */
	int						GetMaxCost(){ return m_MaxCost; }
	void					SetMaxCost(int cost){ m_MaxCost = cost; }
	int*					GetCurrentCost(){ return &m_CurrentCost; }


	void					IsNearUnit(Game* game, UnitType type);
	
private:
	UserNumber				m_UserNumber;
	PlayerNumber			m_PlayerNumber;

	std::vector<Unit>		m_UnitList;

	int						m_MaxCost;
	int						m_CurrentCost;
};

