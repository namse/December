#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "ArcaStone.h"
#include "ClientManager.h"
#include "ClientSession.h"

#include "Soldier.h"
#include "Knight.h"
#include "Rider.h"
#include "Bishop.h"

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
			case UT_SOLDIER:
			{
				unit = new Soldier();

			}break;
			case UT_KNIGHT:
			{
							  unit = new Knight();
			}break;
			case UT_RIDER:
			{
							 unit = new Rider();
			}break;
			case UT_BISHOP:
			{
							  unit = new Bishop();
			}break;
			}
			assert(unit != nullptr);

			// init by copying unitData
			unit->InitUnit(unitData, playerNumber, GenerateUnitIdentityNumber());
			Coord position;

			if (playerNumber == player1)
			{
				position = Coord(0, 0) + originPosition; // locate unit by group data
			}
			else if (playerNumber == player2)
			{
				position = Coord(6, 10) - originPosition; // ��Ī����
			}
			unit->SetPosition(position);
			m_UnitList.push_back(unit);
		};
	}

	// �Ƹ�ī���� ��ġ
	if (USE_ARCA) SetUpArca();

	// ���� �� �ʱ�ȭ
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
	// ���� ������ �ùٸ��� �ʴٸ�!!
	if (!IsCorrectAttack(attacker, attackData))
	{
		// ����!
		return;
	}

	Unit* attackUnit = GetUnit(attackData.id);
	UnitMoveType unitMoveType = attackUnit->GetUnitMoveType();

	m_UnitActionQueue.clear();
	switch (unitMoveType)
	{
	case UMT_STRAIGHT:
	{
						 // ���� �̵� ����!
						 UnitMove(attackData.direction, attackData.Range, attackUnit, true);	// �Լ� ������ ���ֵ� �������� �������~
	}break;
	case UMT_DASH:
	{
					 // �뽬������ �˱� ���� BeforePosition �� ����غ��ƿ�
					 Coord beforePosition = attackUnit->GetPos();

					 // �Է��� Range ��ŭ '��ĭ��' �̵��ϰھ��~
					 for (int move = 0; move < attackData.Range; ++move)
					 {
						 Unit* target = GetUnitInPosition(attackData.position[move]);
						 // ��! ���� �̵��Ϸ��� ��ġ�� ������ ������
						 if (target != nullptr)
						 {
							 HexaDirection direction = GetHexaDirection(beforePosition, attackData.position[move]);
							 // ���� ������ �о������!
							 UnitPush(attackUnit, target, 0, true);
							 break;
						 }
						 // ������ ������ ������ ��� �����ϼ���!
						 else
						 {
							 HexaDirection direction = GetHexaDirection(beforePosition, attackData.position[move]);

							 attackUnit->SetPosition(attackData.position[move]);

							 UnitAction action;
							 action.mActionType = UAT_DASH;
							 action.mUnitId = attackUnit->GetID();
							 action.mMoveData.mRange = 1;
							 action.mMoveData.mDirection = direction;
							 action.mMoveData.mFinalX = attackData.position[move].x;
							 action.mMoveData.mFinalY = attackData.position[move].y;
							 m_UnitActionQueue.push_back(action);

							 Coord beforePosition = attackData.position[move];
						 }
					 }

	}break;
	case UMT_JUMP:
	{
					 UnitJump(attackData.direction, attackData.Range, attackUnit);

	}break;
	case UMT_TELEPORT:
	{
						 // �����Ϸ��� ��ġ�� ������ �ִ� ��� -> ��!ü!
						 if (GetUnitInPosition(attackData.position[0]) != nullptr)
						 {
							 // �ϸ� �ȵ���!
							 Packet::WrongAttackResult outPacket;
							 outPacket.mWrongType = WAT_CANT_TELEPORT_THERE;
							 auto session = GClientManager->GetClient(attacker);
							 if (session != nullptr)
								 session->SendRequest(&outPacket);

							 return ;
						 }
						 attackUnit->SetPosition(attackData.position[0]);

						 UnitAction action;
						 action.mActionType = UAT_TELEPORT;
						 action.mUnitId = attackUnit->GetID();
						 action.mMoveData.mRange = 0;
						 action.mMoveData.mDirection = HexaDirection();
						 action.mMoveData.mFinalX = attackData.position[0].x;
						 action.mMoveData.mFinalY = attackData.position[0].y;
						 m_UnitActionQueue.push_back(action);
	}break;
	default:
		assert(false && "HandleAttack In UnitMoveType Wrong");
	}


	// �̵������ϱ� �̵� ���� Ƚ�� - 1
	m_CanCommand--;

	// ������ ��� ��� �̵��ߴ��� ��°�� �˷��ش�!
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
	if (m_IsGameOver)
	{
		// ������ �������� ���� �˷���
		Packet::GameOverResult outPacket;
		for (auto playerNumber : m_PlayerList)
		{
			outPacket.mWhoIsWinner = m_Winner;

			auto session = GClientManager->GetClient(playerNumber);
			if (session != nullptr)
				session->SendRequest(&outPacket);
		}
		return;
	}

	// TODO : �÷��̾��� ���� �̵� ���� Ƚ�� ��Ŷ ����

	IsNearArca();	// �Ƹ�ī���濡 ���� �� ó�� ���ְ�..

	m_PlayTurn++;	// �� �����~
	
	// TODO : ���� �� ��, �ִ� �� �� ��Ŷ ����

	// ���� �� Ƚ���� ���ٸ�
	if (m_CanCommand <= 0)
	{
		m_IsFirstTurn = false;

		if (m_Attacker == m_PlayerList[0])
		{
			// �����ڸ� �ٲٰ�
			m_Attacker = m_PlayerList[1];
			// �󸶳� ������ �� �ִ��� �˷��ش�.
			m_CanCommand = m_MaxTurn[1];
			// �÷��̾��� �������� ����ϴ� ��ų�� �ְ�ʹٸ� �� ���� �������� ���ָ� �ȴ�.
		}
		else
		{
			m_Attacker = m_PlayerList[0];
			m_CanCommand = m_MaxTurn[0];
		}

		// �����϶�� ��ȣ�� ������!
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
void Game::UnitMove(HexaDirection direction, int range, Unit* unit, bool isFirstMove)
{
	if (range <= 0)
	{
		// finish Move
		return;
	}

	// ó�� �ε��� ���ְ�, �� ���ο��� ���������� ����Ǿ� �ε��� ������ ã�´�.
	Unit* firstGuy = nullptr;
	Unit* lastGuy = nullptr;
	int costLength = -1;
	for (int l = 1; l <= range; l++)
	{
		// unit�� direction �������� l��ŭ �Ÿ��� ���ִ� ����
		Unit* standUnit = GetUnitInPosition(unit->GetPos() + GetUnitVector(direction) * l);
		if (standUnit == nullptr) // �� ���ִ°�? - �ƴ�
		{
			if (firstGuy == nullptr) // ó������ ���� ���� ���� ���°�?
				continue;
			else // ������ �浹�� �ϱ� �ߴ°�? �׷� �浹�� ����߰ڱ���
				break;
		}
		else{ // �׳�, �ű� ���־�����!
			if (firstGuy == nullptr) // ���� �ƹ��� ������ ���ߴ°�?
			{
				costLength = l;
				firstGuy = standUnit;
			}
			lastGuy = standUnit;
		}
	}

	if (firstGuy != nullptr) // �������� (�Լ�)��ġ�⸦ �߽��ϱ�?
	{
		// ���� �����̿��� ���� ���� ��ĭ �ڿ� ������ ��ġ�ϰ�
		unit->SetPosition(unit->GetPos() + GetUnitVector(direction) * (costLength - 1));

		UnitAction action;
		action.mActionType = UAT_MOVE;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = costLength - 1;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = unit->GetPos().x;
		action.mMoveData.mFinalY = unit->GetPos().y;
		m_UnitActionQueue.push_back(action);


		if (m_GameField.isInsideOfField(unit->GetPos()) == false) // �� ���� �ٱ��� ��������?
			KillThisUnit(unit); //�׷� ����X��!

		UnitPush(unit, lastGuy, range - costLength, isFirstMove); // �� �������� �ִ� �� �о��������.
	}
	else // ��� �տ� �ƹ��� ���ٸ�.. ũŪ �ַα�
	{
		unit->SetPosition(unit->GetPos() + GetUnitVector(direction) * range); // �� ������ ���þ��

		UnitAction action;
		action.mActionType = UAT_MOVE;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = range;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = unit->GetPos().x;
		action.mMoveData.mFinalY = unit->GetPos().y;
		m_UnitActionQueue.push_back(action);

		if (m_GameField.isInsideOfField(unit->GetPos()) == false) // �� ���� �ٱ��� ��������?
			KillThisUnit(unit); //�׷� ����X��!.. �ȱ׷��� ȥ�ܵ� ������ �Ф�
	}
}

void Game::UnitJump(HexaDirection direction, int range, Unit* unit)
{
	// �� ���� ����� �̵��Ұǵ�
	Coord moveVector = Coord(GetUnitVector(direction) * range);
	Coord attackPosition = Coord(unit->GetPos() + moveVector);

	// ���������� �̵��ϴ� ��ġ�� �̹� ������ �մ�?
	Unit* standUnit = GetUnitInPosition(attackPosition);

	if (standUnit == nullptr) // ���µ��?
	{
		// �׷� �׳� ������
		unit->SetPosition(attackPosition);

		UnitAction action;
		action.mActionType = UAT_JUMP;
		action.mUnitId = unit->GetID();
		action.mMoveData.mRange = range;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = attackPosition.x;
		action.mMoveData.mFinalY = attackPosition.y;
		m_UnitActionQueue.push_back(action);
		return;
	}

	else	// �ֳ׿�?
	{
		// �׷� ȣ..Ȥ�� �� ��ĭ���� ������ �ִ�?
		Coord attackPositionBefore = Coord(attackPosition - GetUnitVector(direction));
		Unit* standUnitBefore = GetUnitInPosition(attackPositionBefore);

		if (standUnitBefore == nullptr) // ���׿�?
		{
			// �׷� �����ص� �ǰڱ�!
			unit->SetPosition(attackPositionBefore);

			UnitAction action;
			action.mActionType = UAT_JUMP;
			action.mUnitId = unit->GetID();
			action.mMoveData.mRange = range - 1;
			action.mMoveData.mDirection = direction;
			action.mMoveData.mFinalX = attackPositionBefore.x;
			action.mMoveData.mFinalY = attackPositionBefore.y;
			m_UnitActionQueue.push_back(action);

			UnitPush(unit, standUnit, 0, true);
			return;
		}

		else // �־��!
		{
			// ����.. �׷� �����ڳ�
			// �ű� ������ Ŭ��Ծ�~
			Packet::WrongAttackResult outPacket;
			outPacket.mWrongType = WAT_CANT_TELEPORT_THERE;
			auto session = GClientManager->GetClient(m_Attacker);
			if (session != nullptr)
				session->SendRequest(&outPacket);
			return;
		}
	}
}

void Game::UnitPush(Unit* pusher, Unit* target, int power, bool isFirstPush)
{
	// TODO : ���� �߸���µ�, Ŭ�� �д� �κе� �߸��Ǽ� �̻��ϰ� ���ư���� ���� .
	// ������ �ִϸ��̼��̶���� �߰��Ϸ���, ��ĥ �ʿ� �ִ� �κ��� .

	if (isFirstPush) // ù �浹�̸� ���ݷ¿� ����ؼ� �а�
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), pusher->GetAttack() - target->GetWeight(), target, false);
	else // �ι�° �̻��� �浹�̸� ���������� �и� ������ �����ؼ� �а�
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), power - target->GetWeight(), target, false);

	if (pusher->GetOwner() != target->GetOwner())
	{
		// ���̸� ������ ���̰� �а�
		UnitApplyDamageWithCollision(target, pusher);
	}
	else
	{
		// �Ʊ��̸� �׳� �δ�
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
	unit->setStatus(UST_DEAD);

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

void Game::IsNearArca()
{
	// ���� ��ī������ �ִ��� ����, ���� �����µ� �̹��� ������ �̵�����Ƚ��++
	// �ݴ�� --

	// ù���� ���̸� ��ī��������X
	if (m_IsFirstTurn)
		return;
	
	if (!USE_ARCA)
		return;

	bool isNearAlkaStone = false;
	ArcaStone* arcaStone = nullptr;

	// �Ƹ�ī���� ã��
	for (auto unit : m_UnitList)
	{
		if (unit->GetUnitType() == UT_ARCASTONE)
		{
			arcaStone = dynamic_cast<ArcaStone*>(unit);
			break;
		}
	}
	// �� �Ƹ�ī���� ���µ���?
	if (arcaStone == nullptr)
	{
		// �׷� �׳� ������
		return;
	}

	for (Unit* unit : m_UnitList)
	{
		//�� ������ �Ƹ�ī ���� ���� �ֽ��ϱ�?
		if (unit->GetOwner() == m_Attacker)
		{
			for (int i = 1; i <= 6; i++) // Itor for HexaDirection
			{
				Coord positionGap = Coord(arcaStone->GetPos().x - unit->GetPos().x, arcaStone->GetPos().y - unit->GetPos().y);
				// ó��
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

	if (isNearAlkaStone)	// �̹� �Ͽ� �Ƹ�ī������ ���� �ְ�
	{
		if (m_IsNearArca[whosTurn])	// �� �Ͽ��� ���� �־��ٸ�
		{
			// �˰Թ���
		}
		else				// �� �Ͽ��� ���� �����ٸ�
		{
			// �ִ� �̵����� Ƚ���� 1 ������Ų��.
			m_MaxTurn[whosTurn]++;
			// ���� �̵����� Ƚ���� 1 ������Ų��.
			m_CanCommand++;
			// �״� �翡 �Ƹ�ī������ ������
			m_IsNearArca[whosTurn] = true;
		}
	}
	else					// �̹� �Ͽ� �Ƹ�ī������ ���� ����
	{
		if (m_IsNearArca[whosTurn])	// �� �Ͽ��� ���� �־��ٸ�
		{
			m_MaxTurn[whosTurn]--;
			m_CanCommand--;
			m_IsNearArca[whosTurn] = false;
		}
		else				// �� �Ͽ��� ���� �����ٸ�
		{
			// ����
		}
	}
}

void Game::IsGameOver()
{
	// ���� ī����!
	UnitCounting();

	// m_IsGameOver = false; ������ ������� ���� ���¿��� �� �Լ��� ������ٰ� ���� .

	// �÷��̾�1 �����̿�~
	if (m_LivingUnitCount[0] == 0)
	{
		// �׷� ������ ������Ѿ���!
		m_IsGameOver = true;
		m_Winner = WW_PLAYER1;
	}

	// �÷��̾�2 �����̿�~
	if (m_LivingUnitCount[1] == 0)
	{
		m_Winner = WW_PLAYER2;
		// �÷��̾�1�� �����ε��? �׷� ���� �̱�ž�?
		if (m_IsGameOver)
		{
			// ���ºΰ� �ִ� �����̶�� ���º���!
			if (USE_DRAW)
			{
				m_Winner = WW_DRAW;
			}
			// �ƴϸ� �������� ������ ����� �̱����!
			else
			{
				// �÷��̾�1�� �����ڷ��ſ�?
				if (m_PlayerList.at(0) == m_Attacker)
				{
					m_Winner = WW_PLAYER1;
				}
				// �ƴ϶�� �÷��̾�2�� �̱�ű�
				else
				{
					m_Winner = WW_PLAYER2;
				}
			}
		}
		m_IsGameOver = true;
	}
}

void Game::SetUpArca()
{
	// create arcastone(npc)
	ArcaStone* arcaStone = new ArcaStone();

	arcaStone->SetOwner(PLAYER_NUMBER_NPC);
	arcaStone->SetPosition(Coord(3, 5)); // Center of Map
	m_UnitList.push_back(arcaStone);
}

void Game::UnitCounting()
{
	for (int i = 0; i < 3; ++i)
	{
		// 0���� �ʱ�ȭ
		m_UnitCount[i] = 0;
		m_LivingUnitCount[i] = 0;
	}

	for (Unit* unit : m_UnitList)
	{
		// ����ִ� ����
		if (unit->GetUnitStatus() != UST_DEAD)
		{
			if (unit->GetOwner() == m_PlayerList.at(0))
			{
				// �÷��̾�1 ���� �ϳ���~
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
				// NPC �� ���ּ��� ������. �ʿ����� �𸣰ڴ�!
				m_UnitCount[2]++;
				m_LivingUnitCount[2]++;
			}
		}
		// ���� ����
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
	// �ſ� ª�� �����ؼ� ������ ����ϴ� ���
	if (attackData.Range <= 0)
	{
		// �׳� ����
		return false;
	}

	// ���� ���� �ƴѵ� ������ �õ��ϸ�
	if (m_Attacker != attacker)
	{
		// Ŭ���.. ���� ������ �ƴ϶���..
		Packet::WrongAttackResult outPacket;
		outPacket.mWrongType = WAT_NOT_YOUR_TURN;
		auto session = GClientManager->GetClient(attacker);
		if (session != nullptr)
			session->SendRequest(&outPacket);

		return false;
	}

	// ������ �����µ� ���� �����̾�!
	if (m_IsGameOver)		
	{
		return false; // ����!
	}

	// �߸��� �������� �����Ϸ��� �����ϱ�
	if (GetUnit(attackData.id) == nullptr)
	{
		//������!
		return false;
	}

	// �ùٸ� �����̱�! ���!
	return true;
}