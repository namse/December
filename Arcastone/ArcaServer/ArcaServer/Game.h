#pragma once
#include "Field.h"
#include "Unit.h"


class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void			InitGame(PlayerNumber player1, PlayerNumber player2);
	void			StartGame();

	int				GetUnit(OUT Unit* unitArr[]);
	PlayerNumber	GetAttacker(){ return m_Attacker; }

private:
	GameNumber		m_GameNum;
	Field			m_GameField;
	PlayerNumber	m_Attacker;
	int				m_PlayTurn;
	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
};