#include "stdafx.h"
#include "PlayerManager.h"


PlayerManager* GPlayerManager = nullptr;

PlayerManager::PlayerManager()
{
}


PlayerManager::~PlayerManager()
{
	
}

PlayerNumber PlayerManager::CreatePlayer()
{
	static PlayerNumber playerNum = 0;
	playerNum++;
	auto newPlayer = new Player(playerNum);
	m_PlayerList.insert(PlayerList::value_type(playerNum, newPlayer));
	
	return playerNum;
}