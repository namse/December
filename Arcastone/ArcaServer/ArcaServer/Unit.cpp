#include "stdafx.h"
#include "JsonManager.h"
#include "Game.h"

/*  PC  */
#include "Soldier.h"
#include "Magician.h"
#include "Knight.h"
#include "Prince.h"
#include "Princess.h"
#include "Rider.h"

/*  NPC  */
#include "ArcaStone.h"
#include "Rock.h"
#include "Tree.h"

Unit::Unit()
{
	m_UnitType = UT_NONE;
	m_UnitMoveType = UMT_NONE;
	m_HP = m_Attack = m_Weight = m_MoveRange = -1;
	m_UnitStatus = UST_NONE;

	m_IsNearArca = false;
}

Unit::~Unit()
{
}


Unit* Unit::CreateUnit(UnitType unitType)
{
	Unit* newUnit;

	switch (unitType)
	{
	case UT_SOLDIER:
	{
		newUnit = new Soldier();
	}break;
	case UT_MAGICIAN:
	{
		newUnit = new Magician();
	}break;
	case UT_KNIGHT:
	{
		newUnit = new Knight();
	}break;
	case UT_PRINCE:
	{
		newUnit = new Prince();
	}break;
	case UT_PRINCESS:
	{
		newUnit = new Princess();
	}break;
	case UT_ARCASTONE:
	{
		newUnit = new ArcaStone();
	}break;
	case UT_RIDER:
	{
		newUnit = new Rider();
	}break;
	case UT_ROCK:
	{
		newUnit = new Rock();
	}break;
	case UT_TREE:
	{
		newUnit = new Tree();
	}break;
	default:
		assert(false && "Unit::create - Non defined new unitType");
	}

	if(newUnit != nullptr) newUnit->InitUnit(unitType);

	return newUnit;
}


void Unit::InitUnit(UnitType unitType)
{
	UnitData unitData = GJsonManager->GetUnitData(unitType);
	
	m_UnitType = unitType;
	m_UnitMoveType = unitData.m_UnitMoveType;
	m_HP = unitData.m_HP;
	m_Attack = unitData.m_Attack;
	m_Weight = unitData.m_Weight;
	m_MoveRange = unitData.m_MoveRange;
}


int Unit::UnitMove(Game* game, ActionData* actionData)
{
	int attackCost = 0;

	switch (m_UnitMoveType)
	{
	case UMT_STRAIGHT:
	{
						 UnitMoveStraight(game, actionData);
						 attackCost = 1;
	}break;
	case UMT_JUMP:
	{
					 UnitMoveJump(game, actionData);
					 attackCost = 1;
	}break;
	case UMT_DASH:
	{
					 UnitMoveDash(game, actionData);
					 attackCost = 2;
	}break;
	case UMT_TELEPORT:
	{
						 UnitMoveTeleport(game, actionData);
						 attackCost = 1;
	}break;
	default:
		assert(false && "HandleAction In UnitMoveType Wrong");
	}

	return attackCost;
}


void Unit::UnitPush(Game* game, Unit* target, int power, HexaDirection direction)
{
	if (target == nullptr)
		return;

	// 충돌함
	SetCollisionAction(game, target);

	power -= target->GetWeight();
	if (power <= 0) return;

	UnitActionType type = UAT_MOVE;
	Unit* crashGuy = nullptr;
	int range = power;
	Coord front = target->GetPos();
	// 이동방향으로 한칸앞에 유닛 있는지 확인하고, 없으면 한칸씩 이동
	for (int l = 0; l <= power; ++l)
	{
		front = front + GetUnitVector(direction);
		crashGuy = game->GetUnitInPosition(front);
		if (crashGuy != nullptr)
		{
			range = l;
			break;
		}
		target->SetPosition(front);
		power--;
	}

	target->SetMoveAction(game, type, direction, range, target->GetPos());

	target->UnitPush(game, crashGuy, power, direction);
}

void Unit::UnitKill(Game* game)
{
	setStatus(UST_DEAD);

	UnitAction action;
	action.mActionType = UAT_DIE;
	action.mUnitId = m_ID;

	m_Position = Coord(100, 100);

	game->SetActionQueue(&action);
#ifdef _DEBUG
	game->PrintUnitActionQueue(action);
#endif

	// 유닛 수 초기화
	game->UnitCounting();
}

void Unit::KillCheck(Game* game)
{
	if (m_UnitStatus == UST_DEAD)
		return;

	// 맵나가면 사망
	if (!game->GetField()->IsInsideOfField(m_Position))
		UnitKill(game);

	// 피다달면 사망
	if (m_HP <= 0)
		UnitKill(game);
}

void Unit::SetCollisionAction(Game* game, Unit* crashUnit)
{
	// 적이면
	if (m_Owner != crashUnit->GetOwner())
	{
		// 데미지
		crashUnit->SetHP(crashUnit->GetHP() - m_Attack);
		m_HP -= crashUnit->GetAttack();

		if (crashUnit->GetUnitType() == UT_ARCASTONE && USE_ARCA_HIT_COST)
		{
			*(game->GetPlayerList()[m_Owner].GetCurrentCost()) += 1;
		}
	}

	UnitAction action;

	action.mActionType = UAT_COLLISION;
	action.mUnitId = m_ID;
	action.mCollisionData.mTarget = crashUnit->GetID();
	action.mCollisionData.mMyHP = m_HP;
	action.mCollisionData.mTargetHP = crashUnit->GetHP();

	game->SetActionQueue(&action);
#ifdef _DEBUG
	game->PrintUnitActionQueue(action);
#endif

	KillCheck(game);
	crashUnit->KillCheck(game);
}

void Unit::SetMoveAction(Game* game, UnitActionType type, HexaDirection direction, int range, Coord position)
{
	UnitAction action;

	action.mActionType = type;
	action.mUnitId = m_ID;
	action.mMoveData.mRange = range;
	action.mMoveData.mDirection = direction;
	action.mMoveData.mFinalX = position.x;
	action.mMoveData.mFinalY = position.y;

	game->SetActionQueue(&action);
#ifdef _DEBUG
	game->PrintUnitActionQueue(action);
#endif

	KillCheck(game);
}

void Unit::UnitMoveStraight(Game* game, ActionData* actionData)
{
	UnitActionType type = UAT_STRAIGHT;
	HexaDirection direction = actionData->direction;
	int range = actionData->range;

	Unit* crashGuy = nullptr;
	Coord front = m_Position;

	// 이동방향으로 한칸앞에 유닛 있는지 확인하고, 없으면 한칸씩 이동
	for (int l = 0; l < actionData->range; ++l)
	{
		front = front + GetUnitVector(direction);
		crashGuy = game->GetUnitInPosition(front);

		if (crashGuy != nullptr)
		{
			range = l;
			break;
		}

		m_Position = front;
	}

	SetMoveAction(game, type, direction, range, m_Position);

	UnitPush(game, crashGuy, m_Attack, direction);
}


void Unit::UnitMoveJump(Game* game, ActionData* actionData)
{
	UnitActionType type = UAT_JUMP;
	HexaDirection direction = actionData->direction;
	int range = actionData->range;

	// 이동하는 위치에 이미 유닛이 잇니?
	Coord movePos = GetPositionByRange(m_Position, direction, range);
	Unit* standUnit = game->GetUnitInPosition(movePos);

	if (nullptr != standUnit) // 있어요!
	{
		range--;
		// 그 전칸에는?
		movePos = GetPositionByRange(m_Position, direction, range);
		// 없네요? or 1한칸 이동할거거든요?
		if (game->GetUnitInPosition(movePos) == nullptr || actionData->range == 1)
		{
			// 그럼 통과!
		}
		else // 있어요!
		{
			// 에잉.. 그럼 못가겠네
			return;
		}
	}

	Unit* crashGuy = standUnit;
	m_Position = movePos;

	SetMoveAction(game, type, direction, range, m_Position);

	UnitPush(game, crashGuy, m_Attack, direction);
}

void Unit::UnitMoveDash(Game* game, ActionData* actionData)
{
	UnitActionType type = UAT_DASH;
	HexaDirection direction = actionData->direction;
	int range = actionData->range;

	// 전방확인, 이동 반복
	for (int move = 0; move < range; ++move)
	{
		Coord movePosition = actionData->position[move];
		HexaDirection direction = GetHexaDirection(m_Position, movePosition);
		if (direction == HD_NONE)
			return;

		// 이동하려는 위치에 유닛이 있으면
		Unit* crashGuy = game->GetUnitInPosition(movePosition);
		if (nullptr != crashGuy)
		{
			// 몸통 박치기!
			UnitPush(game, crashGuy, m_Attack, direction);
			break;
		}

		m_Position = m_Position + GetUnitVector(direction);
		SetMoveAction(game, type, direction, 1, m_Position);
	}
}

void Unit::UnitMoveTeleport(Game* game, ActionData* actionData)
{
	if (game->GetUnitInPosition(actionData->position[0]) != nullptr)
		return;

	UnitActionType type = UAT_TELEPORT;
	HexaDirection direction = HD_NONE;
	int range = 0;

	m_Position = actionData->position[0];

	SetMoveAction(game, type, direction, range, m_Position);
}