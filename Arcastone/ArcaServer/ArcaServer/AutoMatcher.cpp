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
		Packet::GameStartResult playerPacket;
		
		// get unit data
		Unit* unitList[MAX_UNIT_ON_GAME];
		memset(unitList, 0, sizeof(unitList));

		int unitNum = game->GetUnit(unitList);
		assert(!unitList[0]);
		
		// fill unit data packet
		for (int i = 0; i < unitNum; ++i)
		{
			auto unit = unitList[i];
			if (!unit) break;
			auto unitData = playerPacket.mUnit[i];
			auto position = unit->GetPos();

			unitData.unitType = unit->GetUnitType();
			unitData.unitMoveType = unit->GetUnitMoveType();
			unitData.ownerPlayer = unit->GetOwner();
			unitData.hp = unit->GetHP();
			unitData.weight = unit->GetWeight();
			unitData.attack = unit->GetAttack();
			unitData.moveRange = unit->GetMoveRange();
			unitData.x = position.x;
			unitData.y = position.x;

			++i;
		}
		playerPacket.mField.fieldWidth = MAP_FIELD_WIDTH;
		playerPacket.mField.fieldHeight = MAP_FIELD_HEIGHT;
		playerPacket.mGameData.attacker = game->GetAttacker();
		playerPacket.mLength = unitNum;

		playerSession1->SendRequest(&playerPacket);
		playerSession2->SendRequest(&playerPacket);
		
	}
	else
	{
		m_WaitPlayerList.push(playerId);
	}

}