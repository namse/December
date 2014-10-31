#pragma once
#include "Player.h"

class PlayerManager
{
public:
	PlayerManager();
	~PlayerManager();

	PlayerNumber	CreatePlayer();
	void			DeletePlayer(PlayerNumber playerNum);
	Player*			GetPlayer(PlayerNumber playerNum);

private:
	typedef std::map<PlayerNumber, Player*> PlayerList;
	PlayerList		m_PlayerList;
};

extern PlayerManager* GPlayerManager;