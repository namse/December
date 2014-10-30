#include "stdafx.h"
#include "Game.h"


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



}