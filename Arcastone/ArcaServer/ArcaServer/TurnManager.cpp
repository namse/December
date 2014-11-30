#include "stdafx.h"
#include "TurnManager.h"

TurnManager::TurnManager()
{
	m_BreakDownTurn = BREAK_DOWN_TURN;

	m_IsFirstTurn = true;

	m_PlayTurn = 0;

	if (RANDOM_ATTACKER) m_Turn = (PlayerNumber)(rand() % 2);
	else m_Turn = PLAYER_ONE;
}


TurnManager::~TurnManager()
{
}



void TurnManager::TurnSwap()
{
	m_Turn = (PlayerNumber)((m_Turn == 0) ? 1 : 0);
}