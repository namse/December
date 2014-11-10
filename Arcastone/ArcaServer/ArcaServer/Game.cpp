#include "stdafx.h"
#include "Game.h"
#include "PlayerManager.h"
#include "Unit.h"
#include "Soldier.h"
#include "Knight.h"
#include "Rider.h"
#include "ArcaStone.h"
#include "ClientManager.h"
#include "ClientSession.h"


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
		Player* pPlayer = GPlayerManager->GetPlayer(playerNumber);
		std::vector<UnitData>* group = pPlayer->GetGroup(0)->GetUnitList();

		// get unit data from group
		for (int i = 0; i < group->size(); ++i)
		{
			UnitData unitData = group->at(i);

			Unit* unit = nullptr;

			switch (unitData.m_UnitType)
			{
				case UT_SOLDIER:
				{
									unit = new Soldier();
									break;
				}
				case UT_KNIGHT:
				{
									unit = new Knight();
									break;
				}
				case UT_RIDER:
				{
									unit = new Rider();
									break;
				}
				case UT_ARCASTONE:
				{
										unit = new ArcaStone();
										break;
				}
			}
			assert(unit != nullptr);

			// init by copying unitData
			unit->InitUnit(unitData, playerNumber, GenerateUnitIdentityNumber());

			Coord position;
			
			if (playerNumber == player1)
			{
				position = unitData.m_Position; // locate unit by group data
			}
			else if (playerNumber == player2)
			{
				// ��Ī����
				int x = MAP_FIELD_CENTER_WIDTH * 2 - unitData.m_Position.x;
				int y = MAP_FIELD_CENTER_HEIGHT * 2 - unitData.m_Position.y;
				position = Coord(x, y);
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

	m_UnitActionQueue.clear();

	Unit* attackUnit = GetUnit(attackData.id);
	// ���� �̵� ����!
	UnitMove(attackData.direction, attackData.Range, attackUnit, true);	// �Լ� ������ ���ֵ� �������� �������~

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

	IsArca();	// �Ƹ�ī���濡 ���� �� ó�� ���ְ�..

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
		Unit* standGuy = GetUnitInPosition(unit->GetPos() + GetUnitVector(direction) * l);
		if (standGuy == nullptr) // �� ���ִ°�? - �ƴ�
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
				firstGuy = standGuy;
			}
			lastGuy = standGuy;
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

void Game::UnitPush(Unit* pusher, Unit* target, int power, bool isFirstPush)
{
	if (isFirstPush) // ù �浹�̸� ���ݷ¿� ����ؼ� �а�
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), pusher->GetAttack() - target->GetWeight(), target, false);
	else // �ι�° �̻��� �浹�̸� ���������� �и� ������ �����ؼ� �а�
		UnitMove(GetHexaDirection(pusher->GetPos(), target->GetPos()), power - target->GetWeight(), target, false);

	//���̸� ������ ���̰�
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

void Game::IsArca()
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
		if (unit->GetUnitStatusTypeType() != UST_DEAD)
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