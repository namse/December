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

void AutoMatcher::AddWaitPlayer(PlayerNumber playerId)
{
	// ´Ü¼ø 2ÀÎ ¸ÅÄª
	if (0 < m_WaitPlayerList.size())
	{
		PlayerNumber matchPlayer = m_WaitPlayerList.front();
		m_WaitPlayerList.pop();
		auto gameID = GGameManager->CreateGame(playerId, matchPlayer);

		// make and send packet
		auto game = GGameManager->GetGame(gameID);
		auto playerSession1 = GClientManager->GetClient(playerId);
		auto playerSession2 = GClientManager->GetClient(matchPlayer);
		Packet::GameStartResult outPacket[2];
		
		/*
		// get field data
		std::map<Coord, FieldBlock> fieldMap = game->GetField()->GetFieldBlockList();
		outPacket[0].mField = fieldMap;
		outPacket[1].mField = fieldMap;
		*/

		// get unit data
		auto unitList = game->GetUnitList();
		
		// fill unit data packet
		for (unsigned int i = 0; i < unitList.size(); ++i)
		{
			Unit* unit = unitList[i];
			assert(unit);
			
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
			if (unitOwner == playerId) // player1's id
			{
				outPacket[0].mUnit[i].unitOwner = UO_ME;
				outPacket[1].mUnit[i].unitOwner = UO_ENEMY;
			}
			else if (unitOwner == matchPlayer) // player2's id 
			{
				outPacket[0].mUnit[i].unitOwner = UO_ENEMY;
				outPacket[1].mUnit[i].unitOwner = UO_ME;
			}
			else if (unitOwner == PLAYER_NUMBER_NPC)
			{
				outPacket[0].mUnit[i].unitOwner = UO_NPC;
				outPacket[1].mUnit[i].unitOwner = UO_NPC;
			}
			else
			{
				assert(false);
			}
		}

		outPacket[0].mLength = outPacket[1].mLength = unitList.size();

		playerSession1->SendRequest(&outPacket[0]);
		playerSession2->SendRequest(&outPacket[1]);
		

		game->StartGame();
	}
	else
	{
		m_WaitPlayerList.push(playerId);
	}

}