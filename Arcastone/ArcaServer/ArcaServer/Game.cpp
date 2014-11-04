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
				position = START_POINT_PLAYER2 - originPosition; // ��Ī����
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
	if (m_Attacker != attacker) // ���� ���� �ƴѵ� ������ �õ��ϸ�
	{
		//����
		return;
	}

	Unit* attackUnit = GetUnit(attackData.id);
	if (attackUnit == nullptr)// �߸��� �������� �����Ϸ��� �����ϱ�
	{
		//������!
		return;
	}

	//���� �����Ϳ� ������ ������
	if (attackData.Range == 0)
	{
		//����
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

		// �Ƹ�ī���� ã��
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
			//�� ������ �Ƹ�ī ���� ���� �ֽ��ϱ�?
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
