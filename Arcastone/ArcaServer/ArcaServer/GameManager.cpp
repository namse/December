#include "stdafx.h"
#include "GameManager.h"

GameManager* GGameManager = nullptr;

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

GameNumber GameManager::CreateGame(PlayerNumber player1, PlayerNumber player2)
{
	static GameNumber GameId = 0;
	GameId++;

	// start new game
	auto newGame = new Game(GameId);
	m_GameList.insert(GameList::value_type(GameId, newGame));
	newGame->InitGame(player1, player2);
	
	return GameId;
}