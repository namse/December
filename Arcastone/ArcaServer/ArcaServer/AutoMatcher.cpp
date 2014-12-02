#include "stdafx.h"
#include "AutoMatcher.h"
#include "GameManager.h"
#include "ClientManager.h"
#include "ClientSession.h"

AutoMatcher* GAutoMatcher = nullptr;

AutoMatcher::AutoMatcher()
{
}

AutoMatcher::~AutoMatcher()
{
}

void AutoMatcher::AddWaitUser(UserNumber userId)
{
	// 단순 2인 매칭
	if (0 < m_WaitUserList.size())
	{
		UserNumber matchUser = m_WaitUserList.front();
		m_WaitUserList.erase(m_WaitUserList.begin());

		auto gameID = GGameManager->CreateGame(userId, matchUser);

		// make and send packet
		Game* game = GGameManager->GetGame(gameID);

		Packet::GameStartResult outPacket[PLAYER_COUNT];

		{
			// get field data
			Field* pField = game->GetField();
			int blockCount = pField->GetFieldBlockListSize();

			for (int i = 0; i < PLAYER_COUNT; ++i)
			{
				pField->GetFieldBlockList(outPacket[i].mFieldList);
				outPacket[i].mFieldLength = blockCount;
				outPacket[i].mFieldSizeY = pField->GetFieldSizeY();
				outPacket[i].mFieldSizeX = pField->GetFieldSizeX();
			}
			outPacket[0].mReverseMap = true;
			outPacket[1].mReverseMap = false;
		}

		for (int p = 0; p < PLAYER_COUNT; ++p)
		{
			int incIndex = 0;
			for (int player_idx = 0; player_idx < PLAYER_COUNT_ALL; ++player_idx)
			{
				std::vector<Unit>* unitList = game->GetPlayerList()[player_idx].GetUnitList();
				int listSize = unitList->size();
				for (int i = 0; i < listSize; ++i)
				{
					Unit* unit = &unitList->at(i);
					outPacket[p].mUnit[incIndex].unitType = unit->GetUnitType();
					outPacket[p].mUnit[incIndex].unitMoveType = unit->GetUnitMoveType();
					outPacket[p].mUnit[incIndex].hp = unit->GetHP();
					outPacket[p].mUnit[incIndex].weight = unit->GetWeight();
					outPacket[p].mUnit[incIndex].attack = unit->GetAttack();
					outPacket[p].mUnit[incIndex].moveRange = unit->GetMoveRange();
					outPacket[p].mUnit[incIndex].point.x = unit->GetPos().x;
					outPacket[p].mUnit[incIndex].point.y = unit->GetPos().y;
					outPacket[p].mUnit[incIndex].id = unit->GetID();

					PlayerNumber owner = unit->GetOwner();

					if (owner == p)
						outPacket[p].mUnit[incIndex].unitOwner = UO_ME;
					else if (owner < PLAYER_COUNT)
						outPacket[p].mUnit[incIndex].unitOwner = UO_ENEMY;
					else
						outPacket[p].mUnit[incIndex].unitOwner = UO_NPC;

					incIndex++;
				}
			}
		}

		// TODO 유닛수에 따라 증가하게 할것
		outPacket[0].mUnitLength = 13;
		outPacket[1].mUnitLength = 13;

		ClientSession* userSession[2];
		userSession[PLAYER_ONE] = GClientManager->GetClient(userId);
		userSession[PLAYER_TWO] = GClientManager->GetClient(matchUser);

		for (int i = 0; i < PLAYER_COUNT; ++i)
		{
			userSession[i]->SendRequest(&outPacket[i]);
		}

		game->StartGame();
	}
	else
	{
		m_WaitUserList.push_back(userId);
	}

}

bool AutoMatcher::DeleteWaitUser(UserNumber plyaerId)
{
	std::list <UserNumber>::iterator it;
	for (it = m_WaitUserList.begin(); it != m_WaitUserList.end(); ++it)
	{
		if (plyaerId == *it)
		{
			m_WaitUserList.erase(it);
			return true;
		}
	}
	return false;
}