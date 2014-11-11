#include "Game.h"


Game::Game()
{
	m_Field = new Field();
	m_Player[0] = new Player();
	m_Player[1] = new Player();
}


Game::~Game()
{
	m_Field = nullptr; ///# 헐? null 먼저 만들고 delete?
	delete m_Field;
	m_Player[0] = nullptr;
	delete m_Player[0];
	m_Player[1] = nullptr;
	delete m_Player[1];
}