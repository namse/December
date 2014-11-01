#pragma once
#include "Field.h"
#include "Unit.h"


class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void		InitGame(PlayerNumber player1, PlayerNumber player2);


private:
	GameNumber	m_GameNum;
	Field*		m_GameField;
	std::vector<Unit*>	m_ObjectList;
	std::vector<PlayerNumber>	m_PlayerList;
};