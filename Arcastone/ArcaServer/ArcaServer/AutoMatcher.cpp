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
		auto gameID = GGameManager->CreateGame(playerId, matchPlayer);

		// TODO : Packet 만들고 보내자 아자아자
	}
	else
	{
		m_WaitPlayerList.push(playerId);
	}

}