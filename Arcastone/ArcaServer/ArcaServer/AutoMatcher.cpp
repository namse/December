#include "stdafx.h"
#include "AutoMatcher.h"

AutoMatcher* GAutoMatcher = nullptr;

AutoMatcher::AutoMatcher()
{
}

AutoMatcher::~AutoMatcher()
{
}

void AutoMatcher::AddWaitPlayer(PlayerNumber playerId)
{
	m_WaitPlayerList.push_back(playerId);

	// TODO : 애들 짝 지워서 버스 태워 보내세요 선생님
}