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

	// TODO : �ֵ� ¦ ������ ���� �¿� �������� ������
}