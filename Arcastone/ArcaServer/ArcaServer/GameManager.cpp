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

Game* GameManager::GetGameWithPlayerNumber(PlayerNumber playerNumber) {
	for (auto gameIt : m_GameList)
	if (gameIt.second->IsPlayerInThisGame(playerNumber) == true)
		return gameIt.second;
	return nullptr;
}

bool GameManager::CloseGameByPlayerNumber(PlayerNumber playerNumber)
{
	Game* game = GetGameWithPlayerNumber(playerNumber);
	if (game == nullptr) return false;

	game->GameOverForSurrender(playerNumber);
	m_GameList.erase(game->GetGameNumber());
}