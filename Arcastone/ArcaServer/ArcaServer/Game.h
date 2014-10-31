#pragma once
#include "Field.h"

class Game
{
public:
	Game(GameNumber gameId);
	~Game();

	void		InitGame(PlayerNumber player1, PlayerNumber player2);

private:
	GameNumber	m_GameID;
	Field*		m_GameField;
	std::vector<UnitNumber>		m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
};

