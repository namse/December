#pragma once
#include "Game.h"
#include "User.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	GameNumber		CreateGame(UserNumber user1, UserNumber user2);
	Game*			GetGame(GameNumber GameID){ return m_GameList.find(GameID)->second;  }
	Game*			GetGameWithUserNumber(UserNumber userNumber);
	bool			CloseGameByUserNumber(UserNumber userNumber);

private:
	typedef std::map<GameNumber, Game*> GameList;
	GameList		m_GameList;
};

extern GameManager* GGameManager;