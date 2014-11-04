#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "Pawn.h"
#include "ArcaStone.h"
#include "ClientManager.h"
#include "ClientSession.h"

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
	m_UnitIdentityNumberCounter = 0;
	m_PlayerList.reserve(2);
	m_PlayerList.push_back(player1);
	m_PlayerList.push_back(player2);

	// initialize field
	m_GameField.InitField(MAP_FIELD_WIDTH, MAP_FIELD_HEIGHT); // 7 by 10's rectangle field, like ipad prototype

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
			assert(unit != nullptr);

			// init by copying unitData
			unit->InitUnit(unitData, playerNumber, GenerateUnitIdentityNumber());
			Coord position;

			if (playerNumber == player1)
			{
				position = START_POINT_PLAYER1 + originPosition; // locate unit by group data
			}
			else if (playerNumber == player2)
			{
				position = START_POINT_PLAYER2 - originPosition; // 대칭으로
			}
			unit->SetPosition(position);
			m_UnitList.push_back(unit);
		};
	}

	// create arcastone(npc)
	ArcaStone* arcaStone = new ArcaStone();

	arcaStone->SetOwner(PLAYER_NUMBER_NPC);
	arcaStone->SetPosition(Coord(3, 5)); // Center of Map
	m_UnitList.push_back(arcaStone);

	// play turn and first attacker setting
	m_Attacker = m_PlayerList.at(rand() % m_PlayerList.size());
	m_PlayTurn = 0;
}

void Game::HandleAttack(PlayerNumber attacker, AttackData attackData)
{
	if (m_Attacker != attacker) // 아직 턴이 아닌데 공격을 시도하면
	{
		//무시
		return;
	}

	Unit* attackUnit = GetUnit(attackData.id);
	if (attackUnit == nullptr)// 잘못된 유닛으로 공격하려고 햇으니까
	{
		//무시해!
		return;
	}

	//공격 데이터에 문제가 있으면
	if (attackData.Range == 0)
	{
		//무시
		return;
	}
	else
	{


		m_UnitActionQueue.clear();


		UnitMove(attackData.direction, attackData.Range, attackUnit, true);


		// Send Unit Actions
		{
			Packet::AttackResult outPacket;
			outPacket.mQueueLength = m_UnitActionQueue.size();
			for (unsigned int i = 0; i < m_UnitActionQueue.size(); i++)
			{
				memcpy(&outPacket.mUnitActionQueue[i], &m_UnitActionQueue[i], sizeof(UnitAction));
			}
			for (auto playerNumber : m_PlayerList)
			{
				auto session = GClientManager->GetClient(playerNumber);
				if (session != nullptr)
					session->SendRequest(&outPacket);
			}
		}


		m_PlayTurn++;

		bool isNearAlkaStone = false;
		ArcaStone* arcaStone = nullptr;

		// 아르카스톤 찾고
		for (auto unit : m_UnitList)
		{
			if (unit->GetUnitType() == UT_ARCASTONE)
			{
				arcaStone = dynamic_cast<ArcaStone*>(unit);
				break;
			}
		}


		for (Unit* unit : m_UnitList)
		{
			//내 유닛이 아르카 스톤 옆에 있습니까?
			if (unit->GetOwner() == m_Attacker)
			{
				for (int i = 1; i <= 6; i++) // Itor for HexaDirection
				{
					if (arcaStone->GetPos() - unit->GetPos() == GetUnitVector((HexaDirection)i))
					{
						isNearAlkaStone = true;
						goto finishFindUnitNearArcastone;
					}
				}
			}
		}

	finishFindUnitNearArcastone:

		int maxturn = MAX_TURN;
		if (isNearAlkaStone == true)
		{
			maxturn++;
		}
		if (m_IsFirstTurn == true)
		{
			maxturn = MAX_TURN;
		}
		if (m_PlayTurn < maxturn)
		{
			;
		}
		else
		{
			m_IsFirstTurn = false;
			m_PlayTurn = 0;
			if (m_Attacker == m_PlayerList[0])
				m_Attacker = m_PlayerList[1];
			else m_Attacker = m_PlayerList[0];

			// Let Them Know Is My Turn!
			{
				Packet::YourTurnResult outPacket;
				for (auto playerNumber : m_PlayerList)
				{
					if (m_Attacker == playerNumber)
						outPacket.mIsReallyMyTurn = true;
					else
						outPacket.mIsReallyMyTurn = false;

					auto session = GClientManager->GetClient(playerNumber);
					if (session != nullptr)
						session->SendRequest(&outPacket);
				}
			}

		}
	}
}
void Game::UnitMove(HexaDirection direction, int range, Unit* unit, bool isFirstMove)
{
	if (range <= 0)
	{
		// finish Move
		return;
	}

	// 처음 부딪힌 유닛과, 그 라인에서 마지막으로 연계되어 부딪힐 유닛을 찾는다.
	Unit* firstGuy = nullptr;
	Unit* lastGuy = nullptr;
	int costLength = -1;
	for (int l = 1; l <= range; l++)
	{
		// unit의 direction 방향으로 l만큼 거리에 서있는 유닛
		Unit* standGuy = GetUnitInPosition(unit->GetPos() + GetUnitVector(direction) * l);
		if (standGuy == nullptr) // 게 서있는가? - 아뇨
		{
			if (firstGuy == nullptr) // 처음으로 만난 놈이 아직 없는가?
				continue;
			else // 이제껏 충돌은 하긴 했는가? 그럼 충돌을 멈춰야겠구려
				break;
		}
		else{ // 네놈, 거기 서있었구나!
			if (firstGuy == nullptr) // 아직 아무도 만나지 못했는가?
			{
				costLength = l;
				firstGuy = standGuy;
			}
			lastGuy = standGuy;
		}
	}

	if (firstGuy != nullptr) // 누군가과 (입술)박치기를 했습니까?
	{
		// 가장 가까이에서 만난 애의 한칸 뒤에 유닛을 배치하고
		unit->SetPosition(unit->GetPos() + GetUnitVector(direction) * (costLength - 1));

		UnitAction action;
		action.mActionType = UAT_MOVE;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = costLength - 1;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = unit->GetPos().x;
		action.mMoveData.mFinalY = unit->GetPos().y;
		m_UnitActionQueue.push_back(action);


		if (m_GameField.isInsideOfField(unit->GetPos()) == false) // 거 유닛 바깥에 나갔슴까?
			KillThisUnit(unit); //그럼 젠뷰쌰쓰!

		UnitPush(unit, lastGuy, range - costLength, isFirstMove); // 맨 마지막에 있는 놈만 밀어버리세요.
	}
	else // 당신 앞에 아무도 없다면.. 크큭 솔로군
	{
		unit->SetPosition(unit->GetPos() + GetUnitVector(direction) * range); // 쭉 앞으로 가시어요

		UnitAction action;
		action.mActionType = UAT_MOVE;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = range;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = unit->GetPos().x;
		action.mMoveData.mFinalY = unit->GetPos().y;
		m_UnitActionQueue.push_back(action);

		if (m_GameField.isInsideOfField(unit->GetPos()) == false) // 거 유닛 바깥에 나갔슴까?
			KillThisUnit(unit); //그럼 젠뷰쌰쓰!.. 안그래도 혼잔데 낙사라니 ㅠㅠ
	}
}

void Game::UnitPush(Unit* pusher, Unit* target, int power, bool isFirstPush)
{
	if (isFirstPush) // 첫 충돌이면 공격력에 비례해서 밀고
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), pusher->GetAttack() - target->GetWeight(), target, false);
	else // 두번째 이상의 충돌이면 이제까지의 밀린 정도를 감안해서 밀고
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), power - target->GetWeight(), target, false);

	//적이면 데미지 먹이고
	if (pusher->GetOwner() != target->GetOwner())
		UnitApplyDamageWithCollision(target, pusher);
	else
	{
		UnitAction action;
		action.mActionType = UAT_COLLISION;
		action.mUnitId = pusher->GetID();
		action.mCollisionData.mTarget = target->GetID();
		action.mCollisionData.mMyHP = pusher->GetHP();
		action.mCollisionData.mTargetHP = target->GetHP();
		m_UnitActionQueue.push_back(action);
	}
}

void Game::UnitApplyDamageWithCollision(Unit* thisGuy, Unit* thatGuy)
{
	thisGuy->SetHP(thisGuy->GetHP() - thatGuy->GetAttack());
	thatGuy->SetHP(thatGuy->GetHP() - thisGuy->GetAttack());

	UnitAction action;
	action.mActionType = UAT_COLLISION;
	action.mUnitId = thisGuy->GetID();
	action.mCollisionData.mTarget = thatGuy->GetID();
	action.mCollisionData.mMyHP = thisGuy->GetHP();
	action.mCollisionData.mTargetHP = thatGuy->GetHP();
	m_UnitActionQueue.push_back(action);

	if (thisGuy->GetHP() <= 0)
		KillThisUnit(thisGuy);
	if (thatGuy->GetHP() <= 0)
		KillThisUnit(thatGuy);
}

void Game::KillThisUnit(Unit* unit)
{
	// TODO

	// Temp Code
	unit->SetPosition(Coord(INT_MAX, INT_MAX));



	UnitAction action;
	action.mActionType = UAT_DIE;
	action.mUnitId = unit->GetID();
	m_UnitActionQueue.push_back(action);
}

void Game::StartGame()
{
	int random = rand() % m_PlayerList.size();
	m_Attacker = m_PlayerList[random];
	
	Packet::YourTurnResult outPacket;
	for (auto playerNumber : m_PlayerList)
	{
		if (playerNumber == m_Attacker)
			outPacket.mIsReallyMyTurn = true;
		else
			outPacket.mIsReallyMyTurn = false;
		auto session = GClientManager->GetClient(playerNumber);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}
}
