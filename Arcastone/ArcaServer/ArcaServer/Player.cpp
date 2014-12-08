#include "stdafx.h"
#include "Player.h"
#include "ArcaStone.h"
#include "Game.h"


Player::Player()
{
	m_MaxCost = MAX_TURN;

	m_CurrentCost = 1;
}


Player::~Player()
{
}


void Player::IsNearUnit(Game* game, UnitType type)
{
	// 옆에 유닛이 있는지 보고, 전에 없었는데 이번에 있으면 이동가능횟수++
	// 반대면 --

	// 첫 턴이면 알카스톤적용X
	if (game->GetTurnManager()->GetIsFirstTurn())
		return;

	if (!USE_ARCA)
		return;

	bool isNearUnitNow = false;
	bool isNearUnitBefore = false;
	ArcaStone* arcaStone = nullptr;

	Unit* unit = new Unit(UT_NONE);
	// 아르카스톤 찾고
	for (int i = 0; i < PLAYER_COUNT_ALL; ++i)
	{
		std::vector<Unit>* unitList = game->GetPlayerList()[i].GetUnitList();
		for (int i = 0; i < unitList->size(); ++i)
		{
			unit = &unitList->at(i);
			if (unit->GetUnitType() == type)
			{
				arcaStone = static_cast<ArcaStone*>(unit);
				break;
			}
		}
	}

	// 아르카스톤 없는데여?
	if (arcaStone == nullptr)
		return;


	// 전 턴에 내 유닛이 아르카 스톤 옆에 있었습니까?
	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		if (m_UnitList.at(i).GetIsNearArca())
			isNearUnitBefore = true;
	}

	// 지금 내 유닛이 아르카 스톤 옆에 있습니까?
	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Coord positionGap(arcaStone->GetPos().x - m_UnitList.at(i).GetPos().x, arcaStone->GetPos().y - m_UnitList.at(i).GetPos().y);
		for (int d = 1; d <= 6; ++d)
		{
			if (positionGap == GetUnitVector((HexaDirection)d))
			{
				isNearUnitNow = true;
				m_UnitList.at(i).SetIsNearArca(true);
				break;
			}
			m_UnitList.at(i).SetIsNearArca(false);
		}
	}

	// 이번 턴에 아르카스톤이 옆에 있고, 전 턴에는 옆에 없었다면
	if (isNearUnitNow && !isNearUnitBefore)
	{
		// 최대 이동가능 횟수를 1 증가시킨다.
		m_MaxCost++;
		// 내턴이면 현재 이동가능 횟수도 늘린다.
		if (game->GetTurnManager()->GetWhosTurn() == m_PlayerNumber)
			m_CurrentCost++;
	}

	// 이번 턴에 아르카스톤이 옆에 없고, 전 턴에는 옆에 있었다면
	if (!isNearUnitNow && isNearUnitBefore)
	{
		m_MaxCost--;
		if (game->GetTurnManager()->GetWhosTurn() == m_PlayerNumber)
			m_CurrentCost = (m_CurrentCost > 0) ? m_CurrentCost - 1 : 0;
	}
}

Unit* Player::GetUnit(int id)
{
	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		if (m_UnitList.at(i).GetID() == id)
			return &m_UnitList.at(i);
	}
	return nullptr;
}