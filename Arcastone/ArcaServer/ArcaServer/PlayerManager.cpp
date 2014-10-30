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
	static PlayerNumber playerId = 0;
	playerId++;
	auto newPlayer = new Player(playerId);
	m_PlayerList.insert(PlayerList::value_type(playerId, newPlayer));
	
	return playerId;
}