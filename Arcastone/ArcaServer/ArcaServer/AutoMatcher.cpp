#include "stdafx.h"
#include "AutoMatcher.h"
#include "GameManager.h"

AutoMatcher* GAutoMatcher = nullptr;

AutoMatcher::AutoMatcher()
{
}

AutoMatcher::~AutoMatcher()
{
}

void AutoMatcher::AddWaitPlayer(PlayerNumber playerId)
{
	// 단순 2인 매칭
	if (0 < m_WaitPlayerList.size())
	{
		PlayerNumber matchPlayer = m_WaitPlayerList.front();
		m_WaitPlayerList.pop();
		GGameManager->CreateGame(playerId, matchPlayer);
	}
	else
	{
		m_WaitPlayerList.push(playerId);
	}

}