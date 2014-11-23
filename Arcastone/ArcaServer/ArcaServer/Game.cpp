#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "ArcaStone.h"
#include "ClientManager.h"
#include "ClientSession.h"
#include "../../CommonDefinitions.h"

#define START_POINT_PLAYER1 Coord(3,5)
#define START_POINT_PLAYER2 Coord(3,1)
#define DEBUG_PRINT			true

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
			auto unitType = it->second;
			auto originPosition = it->first;

			Unit* unit = Unit::CreateUnit(unitType);
			assert(unit != nullptr);

			unit->SetOwner(playerNumber);
			unit->SetId(GenerateUnitIdentityNumber());

			Coord position;

			if (playerNumber == player1)
			{
				position = Coord(0, 0) + originPosition; // locate unit by group data
			}
			else if (playerNumber == player2)
			{
				position = Coord(6, 10) - originPosition; // 대칭으로
			}
			unit->SetPosition(position);
			m_UnitList.push_back(unit);
		};
	}

	// 아르카스톤 설치
	if (USE_ARCA) SetUpNPC(UT_ARCASTONE, Coord(3, 5));

	// 장애물 설치
	if (USE_ROCK) SetUpNPC(UT_ROCK, Coord(5, 8));

	// 유닛 수 초기화
	UnitCounting();

	// play turn and first attacker setting
	m_Attacker = m_PlayerList.at(rand() % m_PlayerList.size());
	m_CanCommand = MAX_TURN;
	m_MaxTurn[0] = MAX_TURN;
	m_MaxTurn[1] = MAX_TURN;
	m_PlayTurn = 0;
	m_Winner = WW_NONE;
	m_IsFirstTurn = true;
}

void Game::HandleAttack(PlayerNumber attacker, AttackData attackData)
{
	// 공격 조건이 올바르지 않다면!!
	if (!IsCorrectAttack(attacker, attackData))
	{
		// TODO : 원래는 잘못된 공격이 들어오면 무시했는데,
		// 이번에 커밋한 코드 보니까 WrongAttack이란걸 보내시던데요.
		// 둘 중 어떤걸로 할지 정하고 변경해주세요.

		// 무시!
		return;
	}

	Unit* attackUnit = GetUnit(attackData.id);
	UnitMoveType unitMoveType = attackUnit->GetUnitMoveType();

	m_UnitActionQueue.clear();
	// 유닛 공격 시작
	UnitMove(attackUnit, attackData);

	// 이동했으니까 이동 가능 횟수 - 1
	m_CanCommand--;

	// 유닛이 어떻게 어떻게 이동했는지 통째로 알려준다!
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

	IsGameOver();

	IsNearArca();	// 아르카스톤에 대한 턴 처리 해주고..

	m_PlayTurn++;	// 턴 경과요~

	{
		// 너 마나 이만큼 남았어~
		Packet::CostRenewalResult outPacket;
		outPacket.mCost = m_CanCommand;

		int attackerIndex = GetPlayerIndexByPlayerNumber(m_Attacker);
		outPacket.mMaxCost = m_MaxTurn[attackerIndex];
		auto session = GClientManager->GetClient(m_Attacker);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}

	// 남은 턴 횟수가 없다면
	if (m_CanCommand <= 0)
	{
		m_IsFirstTurn = false;

		if (m_Attacker == m_PlayerList[0])
		{
			// 공격자를 바꾸고
			m_Attacker = m_PlayerList[1];
			// 얼마나 움직일 수 있는지 알려준다.
			m_CanCommand = m_MaxTurn[1];
			// 플레이어의 마나량을 재밍하는 스킬을 넣고싶다면 이 값에 일정값을 빼주면 된다.
		}
		else
		{
			m_Attacker = m_PlayerList[0];
			m_CanCommand = m_MaxTurn[0];
		}

		// 공격하라는 신호를 보낸다!
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

void Game::UnitMove(Unit* unit, AttackData attackData)
{
	UnitMoveType	unitMoveType = unit->GetUnitMoveType();
	Coord			unitPos = unit->GetPos();
	int				unitAtk = unit->GetAttack();
	int				range = attackData.range;
	HexaDirection	direction = attackData.direction;
	Unit*			crashGuy = nullptr; // 충돌한 유닛을 찾는다
	Coord			movePos = unitPos;
	int				moveRange = 0;
	UnitActionType	actionType;

	// TODO : 이 함수는 어떤 공격이 들어와도 핸들링 할 수 있게 추상화하고,
	// 각 공격 타입에 맞는 이동들을 따로 함수로 만들어서 처리할 것.
	// 안그러면 새로운 이동방법, 공격방법 나올때마다 난리남

	switch (unitMoveType)
	{
	case UMT_STRAIGHT:
	{

		// TODO : 뭉쳐있을 경우 맨 마지막의 애만 첫번째 공격한 애의 밀려남 계수로 날라가도록.
		// 다른 공격스타일도.
		// 그럴려면 기본적으로 이동, 밀려남 등이 추상화가 되어 있어야 함.

		actionType = UAT_MOVE;
		for (int l = 1; l <= range; l++)
		{
			// unit의 direction 방향으로 l만큼 거리에 서있는 유닛
			Unit* standUnit = GetUnitInPosition(movePos + GetUnitVector(direction));
			if (nullptr != standUnit) // 서있을 시 충돌
			{
				crashGuy = standUnit;
				break;
			}
			movePos = movePos + GetUnitVector(direction);
			moveRange++;
		}
	}break;

	case UMT_JUMP:
	{
		actionType = UAT_JUMP;

		// 공격유닛이 이동하는 위치에 이미 유닛이 잇니?
		movePos = Coord(unit->GetPos() + (GetUnitVector(direction) * range));
		moveRange = range;
		Unit* standUnit = GetUnitInPosition(movePos);
		if (nullptr != standUnit)
		{
			// 그럼 호..혹시 그 전칸에도 유닛이 있니?
			movePos = Coord(movePos - GetUnitVector(direction));
			moveRange = range - 1;
			Unit* standUnitBefore = GetUnitInPosition(movePos);

			if (standUnitBefore == nullptr || range == 1) // 없네요? or 1한칸 이동할거거든요?
			{
				crashGuy = standUnit;
				break;
			}
			else // 있어요!
			{
				// 에잉.. 그럼 못가겠네
				return;
			}
		}

	}break;

	case UMT_DASH:
	{
		// 대쉬방향을 알기 위해 BeforePosition 을 사용해보아요

		// 입력한 range 만큼 '한칸씩' 이동하겠어요~
		for (int move = 0; move < attackData.range; ++move)
		{
			HexaDirection direction = GetHexaDirection(movePos, attackData.position[move]);
			// 아! 물론 이동하려는 위치에 유닛이 있으면
			Unit* standUnit = GetUnitInPosition(attackData.position[move]);
			if (nullptr != standUnit)
			{
				crashGuy = standUnit;
				break;
			}
			// 유닛을 만나지 않으면 계속 질주하세욧!
			else
			{
				unit->SetPosition(attackData.position[move]);

				// TODO : mMoveData는 Move용 데이터임. Dash용이 아님.
				// 현재는 UnitAction에 Move, Colide, Die 모든 데이터가 다 들어있어서
				// 메모리 비효율적인데,

				// 지금 이런식으로 통째로 되어있는 것을
				// 잘게 나뉘어 한 곳에 쌓아서, 하나의 패킷으로 보내보셈.
				// Ex) 무브가 3바이트, die가 2바이트, 충돌이 1바이트면
				// 무브|무브|다이|무브|충돌
				// 이래서 총 12바이트만 가도록. 현재는 무브든 다이든 모두다 6바이트라서 6 * 5바이트가 날라감.

				UnitAction dashAction;
				dashAction.mActionType = UAT_DASH;
				dashAction.mUnitId = unit->GetID();
				dashAction.mMoveData.mRange = 1;
				dashAction.mMoveData.mDirection = direction;
				dashAction.mMoveData.mFinalX = attackData.position[move].x;
				dashAction.mMoveData.mFinalY = attackData.position[move].y;

				m_UnitActionQueue.push_back(dashAction);
#ifdef DEBUG_PRINT
				PrintUnitActionQueue(dashAction);
#endif
			}
			movePos = attackData.position[move];
		}

	}break;

	case UMT_TELEPORT:
	{
						 if (GetUnitInPosition(attackData.position[0]) != nullptr) return;

						 if (m_GameField.IsInsideOfField(attackData.position[0]) == false) return;

		actionType = UAT_TELEPORT;
		movePos = attackData.position[0];
	}break;

	default:
		assert(false && "HandleAttack In UnitMoveType Wrong");
	}


	// 액션큐에 이동 위치를 담아줍니다. 대쉬 유닛의 경우 패스
	if (unitMoveType != UMT_DASH)
	{
		unit->SetPosition(movePos);

		UnitAction action;
		action.mActionType = actionType;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = moveRange;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = movePos.x;
		action.mMoveData.mFinalY = movePos.y;

		m_UnitActionQueue.push_back(action);
#ifdef DEBUG_PRINT
		PrintUnitActionQueue(action);
#endif
	}


	// 충돌유닛이 있으면
	if (nullptr != crashGuy)
	{
		if (unit->GetOwner() != crashGuy->GetOwner())
		{
			// 적이면 서로 데미지 먹임
			crashGuy->SetHP(crashGuy->GetHP() - unit->GetAttack());
			unit->SetHP(unit->GetHP() - crashGuy->GetAttack());
		}

		if (unit->GetHP() <= 0)
			KillThisUnit(crashGuy);

		UnitAction collisionAct;
		collisionAct.mActionType = UAT_COLLISION;
		collisionAct.mUnitId = unit->GetID();
		collisionAct.mCollisionData.mTarget = crashGuy->GetID();
		collisionAct.mCollisionData.mMyHP = crashGuy->GetHP();
		collisionAct.mCollisionData.mTargetHP = crashGuy->GetHP();
		m_UnitActionQueue.push_back(collisionAct);
#ifdef DEBUG_PRINT
		PrintUnitActionQueue(collisionAct);
#endif

		// 얻어맞은 유닛은 죽은 후에도 밀려난 다음에 사망
		if (crashGuy->GetHP() <= 0)
			KillThisUnit(crashGuy);

		// 유닛이 맞아죽지 않았으면 밀려나게 합니다
		if (crashGuy->GetUnitStatus() != UST_DEAD)
		{
			if (unitAtk > 0) UnitPush(crashGuy, unitAtk, direction);
		}
	}
	else if (!m_GameField.IsInsideOfField(movePos))
	{
		KillThisUnit(unit);
	}
	return;

}

void Game::HandleSkill(PlayerNumber attacker, SkillData skillData)
{
	// TODO : 합당한 스킬 사용인지 판별할 것.

	Unit* castUnit = GetUnit(skillData.id);

	m_UnitActionQueue.clear();

	switch (skillData.skillType)
	{
	case USK_FIREBALL:{
						  // 나중에 이런 스킬 스탯 하드코딩들 고쳐야될듯
						  if (skillData.position[0].x == -1) return;

						  Unit* target = GetUnitInPosition(skillData.position[0]);

						  // 캐스트 유닛에서 스킬 대상위치까지의 방향은?
						  HexaDirection direction = GetDirection(castUnit->GetPos(), target->GetPos());
						  int power = skillData.skillRank + 1;

						  // 마나1 소모
						  m_CanCommand--;

						  UnitPush(target, power, direction);
	}break;

	default:
		break;
	}

	// 유닛이 어떻게 어떻게 이동했는지 통째로 알려준다!
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

	IsGameOver();

	IsNearArca();	// 아르카스톤에 대한 턴 처리 해주고..

	m_PlayTurn++;	// 턴 경과요~

	{
		// 너 마나 이만큼 남았어~
		Packet::CostRenewalResult outPacket;
		outPacket.mCost = m_CanCommand;

		int attackerIndex = GetPlayerIndexByPlayerNumber(m_Attacker);
		outPacket.mMaxCost = m_MaxTurn[attackerIndex];
		auto session = GClientManager->GetClient(m_Attacker);
		if (session != nullptr)
			session->SendRequest(&outPacket);
	}

	// 남은 턴 횟수가 없다면
	if (m_CanCommand <= 0)
	{
		m_IsFirstTurn = false;

		if (m_Attacker == m_PlayerList[0])
		{
			// 공격자를 바꾸고
			m_Attacker = m_PlayerList[1];
			// 얼마나 움직일 수 있는지 알려준다.
			m_CanCommand = m_MaxTurn[1];
			// 플레이어의 마나량을 재밍하는 스킬을 넣고싶다면 이 값에 일정값을 빼주면 된다.
		}
		else
		{
			m_Attacker = m_PlayerList[0];
			m_CanCommand = m_MaxTurn[0];
		}

		// 공격하라는 신호를 보낸다!
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

HexaDirection Game::GetDirection(Coord point1, Coord point2)
{
	Coord vec = point2 - point1;

	if (vec.y < 0 && vec.x == 0)
	{
		return HD_NORTH;
	}
	else if (vec.x > 0 && vec.y < 0)
	{
		if (abs(vec.x) != abs(vec.y)) return HD_NONE;
		return HD_NORTHEAST;
	}
	else if (vec.x < 0 && vec.y == 0)
	{
		return HD_NORTHWEST;
	}
	else if (vec.y > 0 && vec.x == 0)
	{
		return HD_SOUTH;
	}
	else if (vec.x > 0 && vec.y == 0)
	{
		return HD_SOUTHEAST;
	}
	else if (vec.x < 0 && vec.y > 0)
	{
		if (abs(vec.x) != abs(vec.y)) return HD_NONE;
		return HD_SOUTHWEST;
	}

	return HD_NONE;
}

// 연쇄충돌 처리
void Game::UnitPush(Unit* unit, int power, HexaDirection direction)
{
	Coord			unitPos = unit->GetPos();
	int				unitWeight = unit->GetWeight();
	Unit*			crashGuy = nullptr; // 충돌한 유닛을 찾는다
	Coord			movePos = unitPos;
	int				moveRange = 0;

	// TODO : 아래의 코드는 직선이동하는 캐릭터의 이동과 같음.
	// 전에 내가 짜놓은 UnitPush에서는 첫번째 충돌인지 아닌지만 확인해서
	// 직선이동의 코드를 재사용하려고 만든 것임.

	// 아래 이동하는 부분은 직선이동 재사용 하시고,
	// 전에 내가 짜놓은 코드를 리팩토링할거면, 거기에서 필요한 부분들은 다 가지고 그림을 칠판에 그려보시고 짜세요,
	// 핵심 코드들 빠진게 너무 많음.


	power -= unitWeight;
	for (int l = 1; l <= power; l++)
	{
		// unit의 direction 방향으로 l만큼 거리에 서있는 유닛
		Unit* standUnit = GetUnitInPosition(movePos + GetUnitVector(direction));
		if (nullptr != standUnit) // 서있을 시 충돌
		{
			crashGuy = standUnit;
			break;
		}
		movePos = movePos + GetUnitVector(direction);
		moveRange++;
	}

	// 액션큐에 이동 위치를 담아줍니다
	unit->SetPosition(movePos);

	UnitAction action;
	action.mActionType = UAT_MOVE;
	action.mUnitId = unit->GetID();
	action.mMoveData.mRange = moveRange;
	action.mMoveData.mDirection = direction;
	action.mMoveData.mFinalX = movePos.x;
	action.mMoveData.mFinalY = movePos.y;

	m_UnitActionQueue.push_back(action);

#ifdef DEBUG_PRINT
	PrintUnitActionQueue(action);
#endif

	// 충돌유닛이 있으면
	if (nullptr != crashGuy)
	{
		if (unit->GetOwner() != crashGuy->GetOwner())
		{
			// 적이면 서로 데미지 먹임
			crashGuy->SetHP(crashGuy->GetHP() - unit->GetAttack());
			unit->SetHP(unit->GetHP() - crashGuy->GetAttack());
		}

		if (unit->GetHP() <= 0)
			KillThisUnit(crashGuy);

		UnitAction collisionAct;
		collisionAct.mActionType = UAT_COLLISION;
		collisionAct.mUnitId = unit->GetID();
		collisionAct.mCollisionData.mTarget = crashGuy->GetID();
		collisionAct.mCollisionData.mMyHP = crashGuy->GetHP();
		collisionAct.mCollisionData.mTargetHP = crashGuy->GetHP();
		m_UnitActionQueue.push_back(collisionAct);
#ifdef DEBUG_PRINT
		PrintUnitActionQueue(collisionAct);
#endif

		if (crashGuy->GetHP() <= 0)
			KillThisUnit(crashGuy);

		// 유닛이 맞아죽지 않았으면 밀려나게 합니다
		if (crashGuy->GetUnitStatus() != UST_DEAD)
		{
			int pushPower = power - moveRange;
			if (pushPower > 0) UnitPush(crashGuy, pushPower, direction);
		}
	}
	else if (!m_GameField.IsInsideOfField(movePos))
	{
		KillThisUnit(unit);
	}
	return;
}

void Game::KillThisUnit(Unit* unit)
{
	unit->setStatus(UST_DEAD);

	UnitAction action;
	action.mActionType = UAT_DIE;
	action.mUnitId = unit->GetID();

	m_UnitActionQueue.push_back(action);
#ifdef DEBUG_PRINT
	PrintUnitActionQueue(action);
#endif
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

void Game::IsNearArca()
{
	// 옆에 알카스톤이 있는지 보고, 전에 없었는데 이번에 있으면 이동가능횟수++
	// 반대면 --

	// 첫번? 턴이면 알카스톤적용X
	if (m_IsFirstTurn)
		return;

	if (!USE_ARCA)
		return;

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
	// ?아르카스톤 없는데여?
	if (arcaStone == nullptr)
	{
		// 그럼 그냥 지나가
		return;
	}

	for (Unit* unit : m_UnitList)
	{
		//내 유닛이 아르카 스톤 옆에 있습니까?
		if (unit->GetOwner() == m_Attacker)
		{
			for (int i = 1; i <= 6; i++) // Itor for HexaDirection
			{
				Coord positionGap = Coord(arcaStone->GetPos().x - unit->GetPos().x, arcaStone->GetPos().y - unit->GetPos().y);
				// 처리
				if (positionGap == GetUnitVector((HexaDirection)i))
				{
					isNearAlkaStone = true;
					goto finishFindUnitNearArcastone;
				}
			}
		}
	}
finishFindUnitNearArcastone:

	int whosTurn;
	if (m_Attacker == m_PlayerList.at(0))
		whosTurn = 0;
	else whosTurn = 1;

	if (isNearAlkaStone)	// 이번 턴에 아르카스톤이 옆에 있고
	{
		if (m_IsNearArca[whosTurn])	// 전 턴에도 옆에 있었다면
		{
			// 알게뭐람
		}
		else				// 전 턴에는 옆에 없었다면
		{
			// 최대 이동가능 횟수를 1 증가시킨다.
			m_MaxTurn[whosTurn]++;
			// 현재 이동가능 횟수도 1 증가시킨다.
			m_CanCommand++;
			// 그대 곁에 아르카스톤이 있으라
			m_IsNearArca[whosTurn] = true;
		}
	}
	else					// 이번 턴에 아르카스톤이 옆에 없고
	{
		if (m_IsNearArca[whosTurn])	// 전 턴에는 옆에 있었다면
		{
			m_MaxTurn[whosTurn]--;
			m_CanCommand--;
			m_IsNearArca[whosTurn] = false;
		}
		else				// 전 턴에도 옆에 없었다면
		{
			// 무시
		}
	}
}

void Game::IsGameOver()
{
	// 유닛 카운팅!
	UnitCounting();

	// m_IsGameOver = false; 게임이 종료되지 않은 상태에서 이 함수가 실행榮鳴?가정 .

	// 플레이어1 전멸이요~
	if (m_LivingUnitCount[0] == 0)
	{
		// 그럼 게임을 종료시켜야지!
		m_IsGameOver = true;
		m_Winner = WW_PLAYER1;
		GameOver();
	}

	// 플레이어2 전멸이요~
	if (m_LivingUnitCount[1] == 0)
	{
		m_Winner = WW_PLAYER2;
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
				if (m_PlayerList.at(0) == m_Attacker)
				{
					m_Winner = WW_PLAYER1;
				}
				// 아니라면 플레이어2가 이긴거군
				else
				{
					m_Winner = WW_PLAYER2;
				}
			}
		}
		m_IsGameOver = true;
		GameOver();
	}
}


void Game::GameOverForSurrender(PlayerNumber srrender)
{
	if (m_PlayerList.at(0) == srrender)
	{
		m_Winner = WW_PLAYER2;
	}
	else if (m_PlayerList.at(1) == srrender)
	{
		m_Winner = WW_PLAYER1;
	}
	else
	{
		// 에러상황
		m_Winner = WW_DRAW;
	}
	m_IsGameOver = true;
	GameOver();
}

void Game::GameOver()
{
	if (m_IsGameOver)
	{
		// 게임이 끝났으면 승자 알려줌
		auto session1 = GClientManager->GetClient(m_PlayerList[0]);
		auto session2 = GClientManager->GetClient(m_PlayerList[1]);

		if (session1 == nullptr && session2 == nullptr) return;

		Packet::GameOverResult outPacket1;
		Packet::GameOverResult outPacket2;


		switch (m_Winner)
		{
		case WW_PLAYER1:
		{
			outPacket1.mIsMyWin = true;
			outPacket2.mIsMyWin = false;
		}break;
		case WW_PLAYER2:
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
			assert(m_Winner != WW_NONE && "invalid winner player number");
		}

		session1->SendRequest(&outPacket1);
		session2->SendRequest(&outPacket2);
		return;
	}
}

void Game::SetUpNPC(UnitType unitType, Coord unitPos)
{
	// create arcastone(npc)
	Unit* npc = Unit::CreateUnit(unitType);

	npc->SetOwner(PLAYER_NUMBER_NPC);
	npc->SetId(GenerateUnitIdentityNumber());
	npc->SetPosition(unitPos); // Center of Map
	m_UnitList.push_back(npc);

}

void Game::UnitCounting()
{
	for (int i = 0; i < 3; ++i)
	{
		// 0으로 초기화
		m_UnitCount[i] = 0;
		m_LivingUnitCount[i] = 0;
	}

	for (Unit* unit : m_UnitList)
	{
		// 살아있는 유닛
		if (unit->GetUnitStatus() != UST_DEAD)
		{
			if (unit->GetOwner() == m_PlayerList.at(0))
			{
				// 플레이어1 유닛 하나요~
				m_UnitCount[0]++;
				m_LivingUnitCount[0]++;
			}
			else if (unit->GetOwner() == m_PlayerList.at(1))
			{
				m_UnitCount[1]++;
				m_LivingUnitCount[1]++;
			}
			else
			{
				// NPC 의 유닛수도 세주자. 필요할진 모르겠당!
				m_UnitCount[2]++;
				m_LivingUnitCount[2]++;
			}
		}
		// 죽은 유닛
		else
		{
			if (unit->GetOwner() == m_PlayerList.at(0))
			{
				m_UnitCount[0]++;
			}
			else if (unit->GetOwner() == m_PlayerList.at(1))
			{
				m_UnitCount[1]++;
			}
			else
			{
				m_UnitCount[2]++;
			}
		}
	}
}

bool Game::IsCorrectAttack(PlayerNumber attacker, AttackData attackData)
{
	switch (attackData.attackType)
	{
	case UMT_STRAIGHT:
		// 공격을 취소하는 경우
		if (attackData.range == 0)
			return false;
		break;
	case UMT_JUMP:
		if (attackData.range == 0)
			return false;
		break;
	case UMT_DASH:
		if (attackData.range == 0)
			return false;
		break;
	case UMT_TELEPORT:
		// 텔포하려는 위치에 유닛이 있는 경우 -> 합!체!
		if (GetUnitInPosition(attackData.position[0]) != nullptr)
		{
			// 하면 안되지!
			return false;
		}
		break;
	}

	// 아직 턴이 아닌데 공격을 시도하면
	if (m_Attacker != attacker)
	{
		// 클라야.. 아직 너차례 아니란다..
		return false;
	}

	// 게임이 끝났는데 무슨 공격이야!
	if (m_IsGameOver)
	{
		return false; // 무시!
	}

	// 잘못된 유닛으로 공격하려고 햇으니까
	if (GetUnit(attackData.id) == nullptr)
	{
		//무시해!
		return false;
	}

	// 올바른 공격이군! 통과!
	return true;
}

void Game::PrintUnitActionQueue(UnitAction attackData)
{
	printf("Add Unit Action Queue\n");
	switch (attackData.mActionType)
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
		(int)attackData.mUnitId,
		attackData.mMoveData.mRange,
		(int)attackData.mMoveData.mDirection,
		(int)attackData.mMoveData.mFinalX,
		(int)attackData.mMoveData.mFinalY);
	printf("\n");

	return;

UnitCollision:
	printf("Attacker ID : %d\n", (int)attackData.mUnitId);
	printf("Target ID : %d\n", (int)attackData.mCollisionData.mTarget);
	printf("Attacker HP : %d\n", attackData.mCollisionData.mMyHP);
	printf("Target HP : %d\n", attackData.mCollisionData.mTargetHP);

	return;

UnitDie:
	printf("Die Unit ID : %d\n", (int)attackData.mUnitId);

	return;
}

int Game::GetPlayerIndexByPlayerNumber(PlayerNumber playerNumber)
{
	if (playerNumber == m_PlayerList.at(0))
		return 0;
	else return 1;
}


Unit* Game::GetUnit(UnitIdentityNumber id) {
	for (auto unit : m_UnitList)
		if (unit->GetID() == id)
			return unit;
	return nullptr;
}

bool Game::IsPlayerInThisGame(PlayerNumber playerNumber) {
	for (auto playerNumber_ : m_PlayerList)
		if (playerNumber == playerNumber_)
			return true;
	return false;
}

Unit* Game::GetUnitInPosition(Coord position){
	for (auto unit : m_UnitList)
		if (unit->GetPos() == position)
			return unit;
	return nullptr;
}

UnitIdentityNumber Game::GenerateUnitIdentityNumber() {
	return m_UnitIdentityNumberCounter++;
}