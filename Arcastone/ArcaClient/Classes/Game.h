#pragma once

#include "Header.h"
#include "Field.h"
#include "Player.h"

class Game
{
public:
	static Game& getInstance(){
		static Game instance;
		return instance;
	}

	Field*							getField(){ return m_Field; }
	Player*							getPlayer(PlayerWho pw){ return m_Player[pw]; }


private:
	Game();
	~Game();

	Field*							m_Field;
	Player*							m_Player[2];

};

