#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "Pawn.h"
#include "ArcaStone.h"

#define START_POINT_PLAYER1 Coord(3,5)
#define START_POINT_PLAYER2 Coord(3,1)


Game::Game(GameNumber gameNum) : m_GameNum(gameNum)
{
}

Game::~Game()
{
	for (auto unit : m_UnitList)
	{
		delete unit;
	}
	m_PlayerList.clear();
	m_UnitList.clear();
}

void Game::InitGame(PlayerNumber player1, PlayerNumber player2)
{
	m_PlayerList.reserve(2);
	m_PlayerList.push_back(player1);
	m_PlayerList.push_back(player2);

	// initialize field
	m_GameField->InitField(MAP_FIELD_WIDTH, MAP_FIELD_HEIGHT); // 7 by 10's rectangle field, like ipad prototype

	// create and initialize unit
	// get group data from each player
	for (auto playerNumber : m_PlayerList)
	{
		auto player = GPlayerManager->GetPlayer(playerNumber);
		auto group = player->GetGroupList()[0];

		// get unit data from group
		for (auto it = group.m_UnitDataList.begin(); it != group.m_UnitDataList.end(); it++)
		{
			auto unitData = it->second;
			auto originPosition = it->first;
			Unit* unit = nullptr;

			switch (unitData.m_UnitType)
			{
			case UT_PAWN:
			{
				unit = new Pawn();

			}break;
			}
			assert(unit == nullptr);

			// init by copying unitData
			unit->InitUnit(unitData, playerNumber); 
			Coord position;

			if (playerNumber == player1)
			{
				position = START_POINT_PLAYER1 + originPosition; // locate unit by group data
			}
			else if (playerNumber == player2)
			{
				position = START_POINT_PLAYER2 - originPosition; // ´ëÄªÀ¸·Î
			}
			unit->SetPosition(position);
			m_UnitList.push_back(unit);
		};
	}

	// create arcastone(npc)
	ArcaStone* arcaStone = new ArcaStone();

	arcaStone->SetOwner(PLAYER_NUMBER_NPC);
	arcaStone->SetPosition(Coord(3, 3)); // Center of Map
	m_UnitList.push_back(arcaStone);

	// play turn and first attacker setting
	m_Attacker = m_PlayerList.at( rand() % m_PlayerList.size() );
	m_PlayTurn = 0;
}

void Game::StartGame()
{
	// TODO::game logic
}


int Game::GetUnit(OUT Unit* unitArr[])
{
	int num = 0;
	for (auto unit : m_UnitList)
	{
		unitArr[num] = unit;
		++num;
	}

	return num;	// return all unit num
}