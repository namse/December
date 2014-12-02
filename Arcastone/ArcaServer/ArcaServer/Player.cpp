#include "stdafx.h"
#include "Player.h"
#include "ArcaStone.h"
#include "Game.h"


Player::Player()
{
	m_MaxCost = MAX_TURN;

	m_CurrentCost = MAX_TURN;
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

	// 아르카스톤 찾고
	for (int i = 0; i < PLAYER_COUNT_ALL; ++i)
	{
		std::vector<Unit>* unitList = game->GetPlayerList()[i].GetUnitList();
		for (auto unit : *unitList)
		{
			if (unit.GetUnitType() == type)
			{
				// TODO : 각각의 유닛들에 대해 적용 가능하도록 수정
				arcaStone = dynamic_cast<ArcaStone*>(&unit);
				break;
			}
		}
	}

	// 아르카스톤 없는데여?
	if (arcaStone == nullptr)
		return;


	// 전 턴에 내 유닛이 아르카 스톤 옆에 있었습니까?
	for (auto unit : m_UnitList)
	{
		if (unit.GetIsNearArca())
		{
			isNearUnitBefore = true;
		}
	}

	// 지금 내 유닛이 아르카 스톤 옆에 있습니까?
	for (auto unit : m_UnitList)
	{
		Coord positionGap = arcaStone->GetPos() - unit.GetPos();
		for (int i = 1; i <= 6; i++) // Itor for HexaDirection
		{
			if (positionGap == GetUnitVector((HexaDirection)i))
			{
				isNearUnitNow = true;
				unit.SetIsNearArca(true);
				break;
			}
			unit.SetIsNearArca(false);
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
			m_CurrentCost--;
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

void Player::SetCurrentCost(int cost)
{
	if (cost > m_MaxCost)
		cost = m_MaxCost;
	m_CurrentCost = cost;
}