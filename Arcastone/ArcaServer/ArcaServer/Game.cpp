#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"


Game::Game(GameNumber gameNumber) : m_GameID(gameNumber)
{
}

Game::~Game()
{
}

void Game::InitGame(PlayerNumber player1, PlayerNumber player2)
{
	m_PlayerList.reserve(2);
	m_PlayerList.push_back(player1);
	m_PlayerList.push_back(player2);

	// 필드 초기화
	m_GameField->InitField(7, 8);

	// 유닛 초기화



}