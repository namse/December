#pragma once
#include "Player.h"

class PlayerManager
{
public:
	PlayerManager();
	~PlayerManager();

	PlayerNumber	CreatePlayer();
	void			DeletePlayer(PlayerNumber playerNum);
	Player*			GetPlayer(PlayerNumber playerNum){
		auto playerIt = m_PlayerList.find(playerNum);
		if (playerIt == m_PlayerList.end())
			return nullptr;
		return playerIt->second;
	}

private:
	typedef std::map<PlayerNumber, Player*> PlayerList;
	PlayerList		m_PlayerList;
};

extern PlayerManager* GPlayerManager;