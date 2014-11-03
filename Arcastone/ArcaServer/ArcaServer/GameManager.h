#pragma once
#include "Game.h"
#include "Player.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	GameNumber		CreateGame(PlayerNumber player1, PlayerNumber player2);
	Game*			GetGame(GameNumber GameID){ return m_GameList.find(GameID)->second;  }

private:
	typedef std::map<GameNumber, Game*> GameList;
	GameList		m_GameList;
};

extern GameManager* GGameManager;