#include "stdafx.h"
#include "Game.h"

#include "ClientManager.h"
#include "ClientSession.h"
#include "UserManager.h"

#include "../../CommonDefinitions.h"

/*  Skills  */
#include "FireBall.h"
#include "Stamp.h"

#define START_POINT_USER1 Coord(MAP_FIELD_WIDTH/2,MAP_FIELD_HEIGHT - 2)
#define START_POINT_USER2 Coord(MAP_FIELD_WIDTH/2,1)

Game::Game(GameNumber gameNum) : m_GameNum(gameNum)
{
	m_UnitIdentityNumberCounter = 0;

	m_Winner = WW_NONE;
}

Game::~Game()
{

}

void Game::InitGame(UserNumber user1, UserNumber user2)
{
	// 유저에게서 어떤 유닛 그룹을 사용할 지 결정된 상태.
	// 임시로 첫번째 그룹을 선택했다고 가정.
	int groupNumber = 0;
	
	m_User[PLAYER_ONE] = user1;
	m_User[PLAYER_TWO] = user2;
	m_Player[PLAYER_ONE].SetUserNumber(user1);
	m_Player[PLAYER_ONE].SetPlayerNumber(PLAYER_ONE);
	m_Player[PLAYER_TWO].SetUserNumber(user2);
	m_Player[PLAYER_TWO].SetPlayerNumber(PLAYER_TWO);
	m_Player[PLAYER_NPC].SetUserNumber(USER_NUMBER_NPC);
	m_Player[PLAYER_NPC].SetPlayerNumber(PLAYER_NPC);

	// initialize field
	m_Field.InitField(MAP_FIELD_WIDTH, MAP_FIELD_HEIGHT); // 7 by 10's rectangle field, like ipad prototype

	// create and initialize unit
	// get group data from each user
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		User* user = GUserManager->GetUser(m_Player[i].GetUserNumber());
		Group group = user->GetGroupList()[groupNumber];

		// get unit data from group
		for (auto it = group.m_UnitDataList.begin(); it != group.m_UnitDataList.end(); it++)
		{
			UnitType unitType = it->second;
			Coord originPosition = it->first;

			Unit* unit = new Unit(unitType);
			assert(unit != nullptr);

			Coord center;
			if (i == PLAYER_ONE)
			{
				center = Coord(MAP_CENTER_WIDTH - 3, 0) + originPosition; // locate unit by group data
			}
			else
			{
				center = Coord(MAP_FIELD_WIDTH - 1 - MAP_CENTER_WIDTH + 3, MAP_FIELD_HEIGHT - 1) - originPosition; // 대칭으로
			}

			unit->SetPosition(center);
			unit->SetOwner((PlayerNumber)i);
			unit->SetId(GenerateUnitIdentityNumber());

			m_Player[i].SetUnit(unit);
		};
	}

	// NPC 설치
	if (USE_ARCA) SetUpNPC(UT_ARCASTONE, Coord(MAP_CENTER_WIDTH, MAP_CENTER_HEIGHT));

	// 너네 마나 이만큼 남았어~
	SendCurrendtCost();

	// play turn and first attacker setting
//	m_Attacker = m_UserList.at(rand() % m_UserList.size());
}

void Game::HandleAction(UserNumber user, ActionData* actionData)
{
	// 게임이 끝나는 상황인지 확인
	IsGameOver();

	// 정당한 패킷이 아니면 무시
	if (!IsCorrectAction(user, actionData))
		return;

	// 유닛 액션 실행!
	OperatingUnitAction(user, actionData);

	// 유닛이 어떻게 어떻게 이동했는지 통째로 알려준다
	SendActionQueue();

	// 변화된 상황에 대해 이벤트 호출
	AfterActionEvent();

	// 남은 턴 횟수가 없다면 턴넘김
	if (*(GetAttacker()->GetCurrentCost()) <= 0)
		TurnEnd();

	// 너네 마나 이만큼 남았어~
	SendCurrendtCost();

	// 게임이 끝나는 상황인지 확인
	IsGameOver();
}

void Game::OperatingUnitAction(UserNumber user, ActionData* actionData)
{
	m_UnitActionQueue.clear();

	switch (actionData->type)
	{
	case UAS_ATTACK:{
						Unit* attacker = GetUnit(actionData->id);
						if (attacker == nullptr) return;
						int attackCost = attacker->UnitMove(this, actionData);

						*(GetAttacker()->GetCurrentCost()) -= attackCost;
	}return;

	case UAS_SKILL:{
					   Skill* skill = InitSkill(actionData->skillType);

					   // 스킬데이터가 조건에 부합하면 false 를 리턴하여 무시해버린다
					   skill->ActSkill(this, actionData);
	}return;

	case UAS_NONE:
		return;
	}
}


void Game::StartGame()
{
	Packet::YourTurnResult outPacket;
	for (auto userNumber : m_User)
	{
		if (m_Player[m_Turnmanager.GetWhosTurn()].GetUserNumber() == userNumber)
			outPacket.mIsReallyMyTurn = true;
		else
			outPacket.mIsReallyMyTurn = false;
		auto session = GClientManager->GetClient(userNumber);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}
}

void Game::IsGameOver()
{
	// 유닛 카운팅!
	UnitCounting();

	// m_IsGameOver = false; 게임이 종료되지 않은 상태에서 이 함수가 실행榮鳴?가정 .

	// 플레이어1 전멸이요~
	if (m_LivingUnitCount[PLAYER_ONE] == 0)
	{
		// 그럼 게임을 종료시켜야지!
		m_IsGameOver = true;
		m_Winner = WW_USER1;
		GameOver();
	}

	// 플레이어2 전멸이요~
	if (m_LivingUnitCount[PLAYER_TWO] == 0)
	{
		m_Winner = WW_USER2;
		// 플레이어1도 전멸인디요? 그럼 누가 이긴거야?
		if (m_IsGameOver)
		{
			// 무승부가 있는 게임이라면 무승부지!
			if (USE_DRAW)
			{
				m_Winner = WW_DRAW;
			}
			// 아니면 마지막에 공격한 사람이 이긴거지!
			else
			{
				// 플레이어1이 공격자래매요?
				if (m_Player[PLAYER_ONE].GetPlayerNumber() == m_Player[m_Turnmanager.GetWhosTurn()].GetPlayerNumber())
				{
					m_Winner = WW_USER1;
				}
				// 아니라면 플레이어2가 이긴거군
				else
				{
					m_Winner = WW_USER2;
				}
			}
		}
		m_IsGameOver = true;
		GameOver();
	}
}


void Game::GameOverForSurrender(UserNumber srrender)
{
	if (m_User[PLAYER_ONE] == srrender)
	{
		m_Winner = WW_USER2;
	}
	else if (m_User[PLAYER_TWO] == srrender)
	{
		m_Winner = WW_USER1;
	}
	else
	{
		return;
	}
	m_IsGameOver = true;
	GameOver();
}

void Game::GameOver()
{
	if (m_IsGameOver)
	{
		// 게임이 끝났으면 승자 알려줌
		auto session1 = GClientManager->GetClient(m_User[0]);
		auto session2 = GClientManager->GetClient(m_User[1]);

		if (session1 == nullptr && session2 == nullptr) return;

		Packet::GameOverResult outPacket1;
		Packet::GameOverResult outPacket2;


		switch (m_Winner)
		{
		case WW_USER1:
		{
			outPacket1.mIsMyWin = true;
			outPacket2.mIsMyWin = false;
		}break;
		case WW_USER2:
		{
			outPacket1.mIsMyWin = false;
			outPacket2.mIsMyWin = true;
		}break;
		case WW_DRAW:
		{
			outPacket1.mIsDraw = true;
			outPacket2.mIsDraw = true;
		}break;
		default:
			assert(m_Winner != WW_NONE && "invalid winner user number");
		}

		session1->SendRequest(&outPacket1);
		session2->SendRequest(&outPacket2);
		return;
	}
}


void Game::UnitCounting()
{
	for (int i = 0; i < PLAYER_COUNT_ALL; ++i)
	{
		// 0으로 초기화
		m_UnitCount[i] = 0;
		m_LivingUnitCount[i] = 0;
	}

	for (auto player : m_Player)
	{
		std::vector<Unit>* unitList = player.GetUnitList();
		for (int i = 0; i < unitList->size(); ++i)
		{
			PlayerNumber playerNumber = player.GetPlayerNumber();
			m_UnitCount[playerNumber]++;

			if (unitList->at(i).GetUnitStatus() != UST_DEAD)
			{
				m_LivingUnitCount[playerNumber]++;
			}
		}
	}
}

void Game::SetUpNPC(UnitType unitType, Coord unitPos)
{
	// create arcastone(npc)
	Unit* npc = new Unit(unitType);

	npc->SetOwner((PlayerNumber)PLAYER_NUMBER_NPC);
	npc->SetId(GenerateUnitIdentityNumber());
	npc->SetPosition(unitPos); // Center of Map
	m_Player[PLAYER_NUMBER_NPC].SetUnit(npc);

	// 유닛 수 초기화
	UnitCounting();
}

bool Game::IsCorrectAction(UserNumber user, ActionData* actionData)
{
	// 클라야.. 니 턴 아니란다
	if (user != m_Player[m_Turnmanager.GetWhosTurn()].GetUserNumber())
		return false;

	// 게임이 끝났는데 무슨 공격이야!
	if (m_IsGameOver)
	{
		return false; // 무시!
	}

	// 잘못된 유닛으로 공격하려고 하면 무시
	{
		// 유닛이 공격불가상태인지 확인
	}

	// 스킬을 발동하는 경우
	if (actionData->type == UAS_SKILL)
	{
		// TODO : 스킬사용에 필요한 조건 확인

		if (!USE_SKILL)
			return false;

	}
	// 공격하는 경우
	if (actionData->type == UAS_ATTACK)
	{
		// TODO : 유닛의 스탯에 따라 공격 범위 확인

		switch (actionData->attackType)
		{
		case UMT_STRAIGHT:
			if (actionData->range == 0)
				return false;
			break;
		case UMT_JUMP:
			if (actionData->range == 0)
				return false;
			break;
		case UMT_DASH:
			if (actionData->range == 0)
				return false;
			break;
		case UMT_TELEPORT:
			// 텔포하려는 위치에 유닛이 있는 경우 -> 합!체!
			if (GetUnitInPosition(actionData->position[0]) != nullptr)
			{
				// 하면 안되지!
				return false;
			}
			break;
		}
	}

	// 올바른 공격이군! 통과!
	return true;
}

void Game::PrintUnitActionQueue(UnitAction actionData)
{
#ifdef _DEBUG
	printf("Add Unit Action Queue\n");
	switch (actionData.mActionType)
	{
	case UAT_MOVE:
		printf("MoveType : UAT_MOVE");
		printf("\n");
		goto UnitMove;
	case UAT_STRAIGHT:
		printf("MoveType : UAT_STRAIGHT");
		printf("\n");
		goto UnitMove;
	case UAT_JUMP:
		printf("MoveType : UAT_JUMP");
		printf("\n");
		goto UnitMove;
	case UAT_DASH:
		printf("MoveType : UAT_DASH");
		printf("\n");
		goto UnitMove;
	case UAT_TELEPORT:
		printf("MoveType : UAT_TELEPORT");
		printf("\n");
		goto UnitMove;

	case UAT_COLLISION:
		printf("Action Type : Unit Collision");
		printf("\n");
		goto UnitCollision;

	case UAT_DIE:
		printf("Action Type : Unit Die");
		printf("\n");
		goto UnitDie;
	}

	return;

UnitMove:
	printf("Unit ID : %d\n Move Range : %d\n Move Direction : %d\n Move Point : %d, %d\n",
		(int)actionData.mUnitId,
		actionData.mMoveData.mRange,
		(int)actionData.mMoveData.mDirection,
		(int)actionData.mMoveData.mFinalX,
		(int)actionData.mMoveData.mFinalY);
	printf("\n");

	return;

UnitCollision:
	printf("Attacker ID : %d\n", (int)actionData.mUnitId);
	printf("Target ID : %d\n", (int)actionData.mCollisionData.mTarget);
	printf("Attacker HP : %d\n", actionData.mCollisionData.mMyHP);
	printf("Target HP : %d\n", actionData.mCollisionData.mTargetHP);

	return;

UnitDie:
	printf("Die Unit ID : %d\n", (int)actionData.mUnitId);

	return;
#endif
}


Unit* Game::GetUnit(UnitIdentityNumber id)
{
	for (int i = 0; i < PLAYER_COUNT_ALL; ++i)
	{
		if (m_Player[i].GetUnit(id) != nullptr)
			return m_Player[i].GetUnit(id);
	}
	return nullptr;
}

bool Game::IsUserInThisGame(UserNumber userNumber)
{
	for (auto userNumber_ : m_User)
		if (userNumber == userNumber_)
			return true;
	return false;
}

Unit* Game::GetUnitInPosition(Coord position)
{
	Unit* retUnit = nullptr;

	for (int i = 0; i < PLAYER_COUNT_ALL; ++i)
	{
		std::vector<Unit>* unitList = m_Player[i].GetUnitList();
		for (int j = 0; j < unitList->size(); ++j)
		{
			if (unitList->at(j).GetPos() == position)
			{
				retUnit = &unitList->at(j);
				goto BREAK;
			}
		}
	}
BREAK:;
	return retUnit;
}

UnitIdentityNumber Game::GenerateUnitIdentityNumber()
{
	return m_UnitIdentityNumberCounter++;
}

// 일정 턴 이후 필드 붕괴 시작
void Game::StartBreakDown()
{
	if (m_Turnmanager.GetBreakDownTurn() > m_Turnmanager.GetFlowTurn()) return;

	// TODO : 어디부터 몇 개의 블록이 추락할지 기획 상의 후 코딩
	for (int i = 0; i < BREAK_BLOCK; ++i)
	{
		Coord fallBlockCoord = m_Field.GetRandomBlock();

		Unit* fallUnit = m_Field.MakeFieldHole(this, fallBlockCoord);

		if (fallUnit != nullptr)
		{
			m_UnitActionQueue.clear();

			fallUnit->UnitKill(this);

			Packet::AttackResult outPacketUnitDie;
			outPacketUnitDie.mQueueLength = m_UnitActionQueue.size();
			for (unsigned int i = 0; i < m_UnitActionQueue.size(); i++)
			{
				memcpy(&outPacketUnitDie.mUnitActionQueue[i], &m_UnitActionQueue[i], sizeof(UnitAction));
			}
			for (auto userNumber : m_User)
			{
				auto session = GClientManager->GetClient(userNumber);
				if (session != nullptr)
					session->SendRequest(&outPacketUnitDie);
			}

			IsGameOver();
		}
	}

	return;
}

Player* Game::GetPlayerByUserName(UserNumber userNumber)
{
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		Player* pPlayer = &m_Player[i];
		if (pPlayer->GetUserNumber() == userNumber)
		{
			return pPlayer;
		}
	}
	return nullptr;
}

Skill* Game::InitSkill(UnitSkillType type)
{
	Skill* skill = nullptr;

	switch (type)
	{
	case USK_FIREBALL:
		skill = new FireBall();
		break;
	case USK_STAMP:
		skill = new Stamp();
		break;
	}

	assert(skill && "Wrong Skill Type");

	return skill;
}

void Game::SendActionQueue()
{
	Packet::AttackResult outPacket;
	outPacket.mQueueLength = m_UnitActionQueue.size();
	for (unsigned int i = 0; i < m_UnitActionQueue.size(); i++)
	{
		memcpy(&outPacket.mUnitActionQueue[i], &m_UnitActionQueue[i], sizeof(UnitAction));
	}
	for (auto userNumber : m_User)
	{
		auto session = GClientManager->GetClient(userNumber);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}
}

Player* Game::AttackerSwap()
{
	m_Turnmanager.TurnSwap();

	return &m_Player[m_Turnmanager.GetWhosTurn()];
}

void Game::SendCurrendtCost()
{
	Packet::CostRenewalResult outPacket;

	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		outPacket.mMaxCost = m_Player[i].GetMaxCost();
		outPacket.mCost = *(m_Player[i].GetCurrentCost());

		auto session = GClientManager->GetClient(m_User[i]);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}
}

void Game::SendWhosTurn()
{
	Packet::YourTurnResult outPacket;
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		if (i == m_Turnmanager.GetWhosTurn())
			outPacket.mIsReallyMyTurn = true;
		else
			outPacket.mIsReallyMyTurn = false;

		auto session = GClientManager->GetClient(m_User[i]);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}
}

void Game::NearUnitCheck(UnitType type)
{
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		m_Player[i].IsNearUnit(this, type);
	}
}

UserNumber Game::GetUserNumberByPlayerNumber(PlayerNumber playerNumber)
{
	if (playerNumber > PLAYER_COUNT)
		return -1;

	if (playerNumber < 0)
		return -1;

	return m_User[playerNumber];
}

void Game::AfterActionEvent()
{
	// 아르카스톤에 대한 턴 처리
	NearUnitCheck(UT_ARCASTONE);
}

void Game::TossTurn()
{
	// 턴 넘긴자에게 코스트1 준다.
	*(GetAttacker()->GetCurrentCost()) += 1;
	TurnEnd();
}

void Game::TurnEnd()
{
	m_Turnmanager.TurnFlow();

	// 공격자 바꾼다.
	AttackerSwap();

	// 공격자에게 코스트1 준다.
	*(GetAttacker()->GetCurrentCost()) += 1;

	// 공격하라는 신호를 보낸다!
	SendWhosTurn();
	
	// 너네 마나 이만큼 남았어~
	SendCurrendtCost();

	// 턴이 끝날 때 일어나는 이벤트들 호출
	TurnEndEvent();
}

void Game::TurnEndEvent()
{
	// 땅이 무너진다아~
	StartBreakDown();

	// 5 턴 마다
	if (m_Turnmanager.GetFlowTurn() % 5 == 0)
	{
		RandomSetUpItem();
	}
}

void Game::RandomSetUpItem()
{
	srand(time(NULL));

	FieldBlock fieldBlock[MAP_FIELD_WIDTH*MAP_FIELD_HEIGHT];
	m_Field.GetFieldBlockList(fieldBlock);

	Coord target = { -1 , -1 };

	int count = 0;
	int maxTry = 100;

	while (count < maxTry)
	{
		count++;
		int randNum = rand() % m_Field.GetFieldBlockListSize();
		int i = 0;
		for (auto field : fieldBlock)
		{
			if (i == randNum)
			{
				if (field.status == FBS_HOLE)
					break;

				if (GetUnitInPosition(field.position) != nullptr)
					break;

				target = field.position;

				goto BREAK;
			}
			++i;
		}
	}

BREAK:;

	if (target.x == -1)
		return;

	int npc = rand() % 2;

	switch (npc)
	{
	case 0:
		SetUpNPCWithClient(UT_PEBBLE, target);
		break;
	case 1:
		SetUpNPCWithClient(UT_POTION, target);
		break;
	default:
		break;
	}
}

void Game::SetUpNPCWithClient(UnitType unitType, Coord unitPos)
{
	/*
	// create npc
	Unit* npc = new Unit(unitType);

	npc->SetOwner((PlayerNumber)PLAYER_NUMBER_NPC);
	npc->SetId(GenerateUnitIdentityNumber());
	npc->SetPosition(unitPos); // Center of Map
	m_Player[PLAYER_NUMBER_NPC].SetUnit(npc);

	// 유닛 수 초기화
	UnitCounting();

	Packet::GameStartResult outPacket[PLAYER_COUNT];
	ClientSession* userSession[2];

	for (int p = 0; p < PLAYER_COUNT; ++p)
	{
		outPacket[p].mUnit[0].unitType = npc->GetUnitType();
		outPacket[p].mUnit[0].unitMoveType = npc->GetUnitMoveType();
		outPacket[p].mUnit[0].hp = npc->GetHP();
		outPacket[p].mUnit[0].weight = npc->GetWeight();
		outPacket[p].mUnit[0].attack = npc->GetAttack();
		outPacket[p].mUnit[0].moveRange = npc->GetMoveRange();
		outPacket[p].mUnit[0].point.x = npc->GetPos().x;
		outPacket[p].mUnit[0].point.y = npc->GetPos().y;
		outPacket[p].mUnit[0].id = npc->GetID();

		PlayerNumber owner = npc->GetOwner();

		if (owner == p)
			outPacket[p].mUnit[0].unitOwner = UO_ME;
		else if (owner < PLAYER_COUNT)
			outPacket[p].mUnit[0].unitOwner = UO_ENEMY;
		else
			outPacket[p].mUnit[0].unitOwner = UO_NPC;

		outPacket[p].mUnitLength = 1;

		userSession[p] = GClientManager->GetClient(m_Player[p].GetUserNumber());
		userSession[p]->SendRequest(&outPacket[p]);
	}
	*/
}