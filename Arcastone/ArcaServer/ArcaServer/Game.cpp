#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "Pawn.h"
#include "ArcaStone.h"

Game::Game(GameNumber gameNum) : m_GameNum(gameNum)
{
}

Game::~Game()
{
	for (auto gameObject : m_ObjectList)
	{
		delete gameObject;
	}
	m_PlayerList.clear();
	m_ObjectList.clear();
}

void Game::InitGame(PlayerNumber player1, PlayerNumber player2)
{
	m_PlayerList.reserve(2);
	m_PlayerList.push_back(player1);
	m_PlayerList.push_back(player2);

	// 필드 초기화
	m_GameField->InitField(7, 8);

	// 유닛 생성 및 초기화(임의로 넣음)
	ObjectNumber objectNum = 0;
	m_ObjectList.reserve(PLAYER_UNIT_MAX * 2 + 1);

	for (auto player : m_PlayerList)
	{
		for (int i = 0; i < PLAYER_UNIT_MAX; ++i)
		{
			objectNum++;
			Unit* newObj = new Pawn();
			newObj->InitObject(objectNum, player);
			m_ObjectList.push_back(newObj);
		}
	}

	// 아르카스톤(NPC) 생성
	objectNum++;
	ArcaStone* arcaStone = new ArcaStone();
	arcaStone->InitObject(objectNum);
	m_ObjectList.push_back(arcaStone);

	// 맵에 오브젝트 배치
	SetObjectOnField();

}

void Game::SetObjectOnField()
{
	// 하나하나 배치해주세요~ 슈퍼 하드코딩~!
}

