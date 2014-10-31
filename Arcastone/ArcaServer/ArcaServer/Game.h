#pragma once
#include "Field.h"
#include "GameObject.h"

class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void		InitGame(PlayerNumber player1, PlayerNumber player2);

private:
	void		SetObjectOnField();

private:
	GameNumber	m_GameNum;
	Field*		m_GameField;
	std::vector<GameObject*>	m_ObjectList;
	std::vector<PlayerNumber>	m_PlayerList;
};