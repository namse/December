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

		Packet::GameStartResult outPacket[2];

		{
			// get field data
			Field* pField = game->GetField();
			int blockCount = pField->GetFieldBlockListSize();

			pField->GetFieldBlockList(outPacket[0].mFieldList);
			pField->GetFieldBlockList(outPacket[1].mFieldList);
			outPacket[0].mFieldLength = blockCount;
			outPacket[1].mFieldLength = blockCount;
			outPacket[0].mFieldSizeX = pField->GetFieldSizeX();
			outPacket[0].mFieldSizeY = pField->GetFieldSizeY();
			outPacket[1].mFieldSizeX = pField->GetFieldSizeX();
			outPacket[1].mFieldSizeY = pField->GetFieldSizeY();
			outPacket[0].mReverseMap = true;
			outPacket[1].mReverseMap = false;
		}

		{
			// get unit data
			auto unitList = game->GetUnitList();

			// fill unit data packet
			for (unsigned int i = 0; i < unitList->size(); ++i)
			{
				Unit* unit = unitList->at(i);

				auto position = unit->GetPos();

				for (int j = 0; j < 2; ++j)
				{
					outPacket[j].mUnit[i].unitType = unit->GetUnitType();
					outPacket[j].mUnit[i].unitMoveType = unit->GetUnitMoveType();
					outPacket[j].mUnit[i].hp = unit->GetHP();
					outPacket[j].mUnit[i].weight = unit->GetWeight();
					outPacket[j].mUnit[i].attack = unit->GetAttack();
					outPacket[j].mUnit[i].moveRange = unit->GetMoveRange();
					outPacket[j].mUnit[i].point.x = position.x;
					outPacket[j].mUnit[i].point.y = position.y;
					outPacket[j].mUnit[i].id = unit->GetID();
				}

				
				auto unitOwner = unit->GetOwner();
				if (unitOwner == game->GetPlayerList()[0].GetPlayerNumber()) // user1's id
				{
					outPacket[0].mUnit[i].unitOwner = UO_ME;
					outPacket[1].mUnit[i].unitOwner = UO_ENEMY;
				}
				else if (unitOwner == game->GetPlayerList()[1].GetPlayerNumber()) // user2's id 
				{
					outPacket[0].mUnit[i].unitOwner = UO_ENEMY;
					outPacket[1].mUnit[i].unitOwner = UO_ME;
				}
				else if (unitOwner == game->GetPlayerList()[2].GetPlayerNumber())
				{
					outPacket[0].mUnit[i].unitOwner = UO_NPC;
					outPacket[1].mUnit[i].unitOwner = UO_NPC;
				}
			}
			outPacket[0].mUnitLength = unitList->size();
			outPacket[1].mUnitLength = unitList->size();
		}

		auto userSession1 = GClientManager->GetClient(userId);
		auto userSession2 = GClientManager->GetClient(matchUser);

		userSession1->SendRequest(&outPacket[0]);
		userSession2->SendRequest(&outPacket[1]);
		

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