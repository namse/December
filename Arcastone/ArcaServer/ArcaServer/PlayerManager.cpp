#include "stdafx.h"
#include "PlayerManager.h"
#include "AutoMatcher.h"
#include "GameManager.h"


PlayerManager* GPlayerManager = nullptr;

PlayerManager::PlayerManager()
{
}


PlayerManager::~PlayerManager()
{
	for (auto playerIt : m_PlayerList)
	{
		delete playerIt.second;
	}

	m_PlayerList.clear();
}

PlayerNumber PlayerManager::CreatePlayer()
{
	static PlayerNumber playerNum = 0;
	playerNum++;
	auto newPlayer = new Player(playerNum);
	m_PlayerList.insert(PlayerList::value_type(playerNum, newPlayer));
	
	return playerNum;
}

Player* PlayerManager::GetPlayer(PlayerNumber playerNum){
	auto playerIt = m_PlayerList.find(playerNum);
	if (playerIt == m_PlayerList.end())
		return nullptr;
	return playerIt->second;
}


void PlayerManager::DeletePlayer(PlayerNumber playerNum)
{
	auto playerIt = m_PlayerList.find(playerNum);
	if (playerIt == m_PlayerList.end())
		return;

	GAutoMatcher->DeleteWaitPlayer(playerNum);
	GGameManager->CloseGameByPlayerNumber(playerNum);

	delete playerIt->second;
	m_PlayerList.erase(playerIt);
}