#pragma once

#include "Header.h"
#include "Field.h"
#include "Player.h"

class Game
{
public:
	Game();
	~Game();

	Field*						getField(){ return m_Field; }
	Player*						getPlayer(PlayerWho pw){ return m_Player[pw]; }


private:
	Field*						m_Field;
	Player*						m_Player[2];

};

