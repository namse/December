#pragma once
#include "Game.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	void CreateGame();


private:
	std::map<Game*, GameNumber> m_GameList;

};

extern GameManager* GGameManager;