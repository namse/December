#pragma once

#include "Header.h"
#include "field.h"
#include "Player.h"

class Game
{
public:
	static Game& getInstance(){
		static Game instance;
		return instance;
	}



private:
	Game();
	~Game();

	field* m_Field;
	Player* m_Player[2];

};

