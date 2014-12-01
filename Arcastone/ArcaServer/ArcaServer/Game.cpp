#include "stdafx.h"
#include "Game.h"

#include "ClientManager.h"
#include "ClientSession.h"
#include "UserManager.h"

#include "../../CommonDefinitions.h"

/*  Skills  */
#include "FireBall.h"
#include "Stamp.h"

#define START_POINT_USER1 Coord(3,5)
#define START_POINT_USER2 Coord(3,1)

Game::Game(GameNumber gameNum) : m_GameNum(gameNum)
{
	m_UnitIdentityNumberCounter = 0;

	m_CurrentCost = MAX_TURN;

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

			Unit* unit = Unit::CreateUnit(unitType);
			assert(unit != nullptr);

			Coord center;
			if (i == 0)
			{
				center = Coord(0, 0) + originPosition; // locate unit by group data
			}
			else
			{
				center = Coord(6, 10) - originPosition; // 대칭으로
			}

			unit->SetPosition(center);
			unit->SetOwner((PlayerNumber)i);
			unit->SetId(GenerateUnitIdentityNumber());

			m_Player[i].SetUnit(unit);
			m_AllUnit.push_back(unit);
		};
	}

	// 아르카스톤 설치
	if (USE_ARCA) SetUpNPC(UT_ARCASTONE, Coord(3, 5));

	// 장애물 설치
	if (USE_ROCK)
	{
		SetUpNPC(UT_ROCK, Coord(5, 8));
		SetUpNPC(UT_ROCK, Coord(1, 2));
		SetUpNPC(UT_TREE, Coord(0, 9));
		SetUpNPC(UT_TREE, Coord(0, 10));
		SetUpNPC(UT_TREE, Coord(1, 10));
		SetUpNPC(UT_TREE, Coord(5, 0));
		SetUpNPC(UT_TREE, Coord(6, 0));
		SetUpNPC(UT_TREE, Coord(6, 1));
	}

	// play turn and first attacker setting
//	m_Attacker = m_UserList.at(rand() % m_UserList.size());
}

void Game::HandleAction(UserNumber user, ActionData* actionData)
{
	// 정당한 패킷이 아니면 무시
	if (!IsCorrectAction(user, actionData))
		return;

	// 유닛 액션 실행!
	OperatingUnitAction(user, actionData);

	// 유닛이 어떻게 어떻게 이동했는지 통째로 알려준다
	SendActionQueue();

	// 아르카스톤에 대한 턴 처리 해주고..
	NearArcaCheck();

	// 너네 마나 이만큼 남았어~
	SendCurrendtCost();

	// 게임이 끝나는 상황인지 확인
	IsGameOver();

	// 남은 턴 횟수가 없다면
	if (m_CurrentCost <= 0)
	{
		m_Turnmanager.TurnFlow();
		StartBreakDown();

		// 플레이어의 마나량을 재밍하는 스킬을 넣고싶다면 이 부분을 수정하면 된다.
		// 공격자를 바꾸고, 현재 코스트를 공격자의 최대 코스트까지 채운다.
		m_CurrentCost = AttackerSwap()->GetMaxCost();

		// 공격하라는 신호를 보낸다!
		SendWhosTurn();
	}
}

void Game::OperatingUnitAction(UserNumber user, ActionData* actionData)
{
	m_UnitActionQueue.clear();

	Player* attacker = GetPlayerByUserName(user);
	UnitSkillType type = actionData->skillType;
	if (type != USK_NONE)
	{
		// 유닛 이동
		Unit* attacker = GetUnit(actionData->id);
		attacker->UnitMove(this, actionData);

		// 이동 가능 횟수 - 1
		m_CurrentCost--;
	}
	else
	{
		// 스킬 사용
		Skill* skill = InitSkill(type);

		// 스킬데이터가 조건에 부합하면 false 를 리턴하여 무시해버린다
		if (!skill->ActSkill(this, actionData))
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

	for (Unit* unit : m_AllUnit)
	{
		PlayerNumber playerNumber = unit->GetOwner();

		// 살아있는 유닛
		if (unit->GetUnitStatus() != UST_DEAD)
		{
			m_UnitCount[playerNumber]++;
			m_LivingUnitCount[playerNumber]++;
		}
		// 죽은 유닛
		else
		{
			m_UnitCount[playerNumber]++;
		}
	}
}

void Game::SetUpNPC(UnitType unitType, Coord unitPos)
{
	// create arcastone(npc)
	Unit* npc = Unit::CreateUnit(unitType);

	npc->SetOwner((PlayerNumber)PLAYER_NUMBER_NPC);
	npc->SetId(GenerateUnitIdentityNumber());
	npc->SetPosition(unitPos); // Center of Map
	m_AllUnit.push_back(npc);

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
		// 유닛의 Owner 확인
		Unit* pUnit = GetUnit(actionData->id);
		if (pUnit->GetOwner() != m_Turnmanager.GetWhosTurn())
		{
			return false;
		}

		// 유닛이 공격불가상태인지 확인
	}

	// 스킬을 발동하는 경우
	if (actionData->skillType != USK_NONE)
	{
		// TODO : 스킬사용에 필요한 조건 확인


	}
	// 공격하는 경우
	else
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


Unit* Game::GetUnit(UnitIdentityNumber id) {
	for (auto unit : m_AllUnit)
		if (unit->GetID() == id)
			return unit;
	return nullptr;
}

bool Game::IsUserInThisGame(UserNumber userNumber) {
	for (auto userNumber_ : m_User)
		if (userNumber == userNumber_)
			return true;
	return false;
}

Unit* Game::GetUnitInPosition(Coord position){
	for (auto unit : m_AllUnit)
		if (unit->GetPos() == position)
			return unit;
	return nullptr;
}

UnitIdentityNumber Game::GenerateUnitIdentityNumber() {
	return m_UnitIdentityNumberCounter++;
}

// 일정 턴 이후 필드 붕괴 시작
void Game::StartBreakDown()
{
	if (m_Turnmanager.GetBreakDownTurn() > m_Turnmanager.GetFlowTurn()) return;

	// TODO : 어디부터 몇 개의 블록이 추락할지 기획 상의 후 코딩
	// 임시로 순서대로 블록 두개씩 사라지게
	for (int i = 0; i < BREAK_BLOCK; ++i)
	{
		Coord fallBlockCoord = m_Field.GetRandomBlock();

		Unit* fallUnit = m_Field.MakeFieldHole(&fallBlockCoord, &m_AllUnit, m_User);

		if (fallUnit != nullptr)
		{
			m_UnitActionQueue.clear();

			fallUnit->KillThisUnit(this);

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

		if (i == m_Turnmanager.GetWhosTurn())
			outPacket.mCost = m_CurrentCost;
		else
			outPacket.mCost = m_Player[i].GetMaxCost();

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

void Game::NearArcaCheck()
{
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		m_Player[i].IsNearArca(&m_AllUnit, &m_Turnmanager, &m_CurrentCost);
	}
}

UserNumber Game::GetUserNumberByPlayerNumber(PlayerNumber playerNumber)
{
	if (playerNumber > PLAYER_COUNT)
		return -1;

	if (playerNumber < 0)
		return -1;

	return playerNumber;
}