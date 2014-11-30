#include "stdafx.h"
#include "GameManager.h"

GameManager* GGameManager = nullptr;

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

GameNumber GameManager::CreateGame(UserNumber user1, UserNumber user2)
{
	static GameNumber GameId = 0;
	GameId++;

	// start new game
	auto newGame = new Game(GameId);
	m_GameList.insert(GameList::value_type(GameId, newGame));
	newGame->InitGame(user1, user2);
	
	return GameId;
}

Game* GameManager::GetGameWithUserNumber(UserNumber userNumber) {
	for (auto gameIt : m_GameList)
	if (gameIt.second->IsUserInThisGame(userNumber) == true)
		return gameIt.second;
	return nullptr;
}

bool GameManager::CloseGameByUserNumber(UserNumber userNumber)
{
	Game* game = GetGameWithUserNumber(userNumber);
	if (game == nullptr) return false;

	game->GameOverForSurrender(userNumber);
	m_GameList.erase(game->GetGameNumber());
}