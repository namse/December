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
	auto player = new Player(playerId);
	m_PlayerList.insert(PlayerList::value_type(playerId, player));
	
	return playerId;
}