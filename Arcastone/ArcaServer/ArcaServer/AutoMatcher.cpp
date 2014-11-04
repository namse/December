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
	// 단순 2인 매칭
	if (0 < m_WaitPlayerList.size())
	{
		PlayerNumber matchPlayer = m_WaitPlayerList.front();
		m_WaitPlayerList.pop();
		auto gameID = GGameManager->CreateGame(playerId, matchPlayer);

		// make and send packet
		auto game = GGameManager->GetGame(gameID);
		auto playerSession1 = GClientManager->GetClient(playerId);
		auto playerSession2 = GClientManager->GetClient(matchPlayer);
		Packet::GameStartResult player1Packet;
		Packet::GameStartResult player2Packet;
		
		// get unit data
		auto unitList = game->GetUnitList();
		
		// fill unit data packet
		for (unsigned int i = 0; i < unitList.size(); ++i)
		{
			auto unit = unitList[i];
			assert(unit);
			
			auto position = unit->GetPos();

			player1Packet.mUnit[i].unitType = player2Packet.mUnit[i].unitType = unit->GetUnitType();
			player1Packet.mUnit[i].unitMoveType = player2Packet.mUnit[i].unitMoveType = unit->GetUnitMoveType();
			player1Packet.mUnit[i].hp = player2Packet.mUnit[i].hp = unit->GetHP();
			player1Packet.mUnit[i].weight = player2Packet.mUnit[i].weight = unit->GetWeight();
			player1Packet.mUnit[i].attack = player2Packet.mUnit[i].attack = unit->GetAttack();
			player1Packet.mUnit[i].moveRange = player2Packet.mUnit[i].moveRange = unit->GetMoveRange();
			player1Packet.mUnit[i].x = player2Packet.mUnit[i].x = position.x;
			player1Packet.mUnit[i].y = player2Packet.mUnit[i].y = position.x;
			player1Packet.mUnit[i].id = player2Packet.mUnit[i].id = unit->GetID();

			auto unitOwner = unit->GetOwner();
			if (unitOwner == playerId) // player1's id
			{
				player1Packet.mUnit[i].unitOwner = UO_ME;
				player2Packet.mUnit[i].unitOwner = UO_ENEMY;
			}
			else if (unitOwner == matchPlayer) // player2's id 
			{
				player1Packet.mUnit[i].unitOwner = UO_ENEMY;
				player2Packet.mUnit[i].unitOwner = UO_ME;
			}
			else if (unitOwner == PLAYER_NUMBER_NPC)
			{
				player1Packet.mUnit[i].unitOwner = UO_NPC;
				player2Packet.mUnit[i].unitOwner = UO_NPC;
			}
			else
			{
				assert(false);
			}
		}
		player1Packet.mField.fieldWidth = player2Packet.mField.fieldWidth = MAP_FIELD_WIDTH;
		player1Packet.mField.fieldHeight = player2Packet.mField.fieldHeight = MAP_FIELD_HEIGHT;
		player1Packet.mLength = player2Packet.mLength = unitList.size();

		playerSession1->SendRequest(&player1Packet);
		playerSession2->SendRequest(&player2Packet);
		

		game->StartGame();
	}
	else
	{
		m_WaitPlayerList.push(playerId);
	}

}