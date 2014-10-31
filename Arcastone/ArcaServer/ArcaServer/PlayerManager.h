#pragma once
#include "Player.h"

class PlayerManager
{
public:
	PlayerManager();
	~PlayerManager();

	PlayerNumber	CreatePlayer();
	void			DeletePlayer(PlayerNumber playerId);
	Player*			GetPlayer(PlayerNumber playerId);

private:
	typedef std::map<PlayerNumber, Player*> PlayerList;
	PlayerList		m_PlayerList;
};

extern PlayerManager* GPlayerManager;