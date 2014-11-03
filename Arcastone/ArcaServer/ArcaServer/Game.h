#pragma once
#include "Field.h"
#include "Unit.h"


class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void			InitGame(PlayerNumber player1, PlayerNumber player2);
	int				GetUnit(OUT Unit* unitArr[]);

private:
	GameNumber		m_GameNum;
	Field*			m_GameField;
	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
};