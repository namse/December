#include "Game.h"


Game::Game()
{
	m_Field = new Field();
	m_Player[0] = new Player();
	m_Player[1] = new Player();
}


Game::~Game()
{
	delete m_Field;
	delete m_Player[0];
	delete m_Player[1];
}