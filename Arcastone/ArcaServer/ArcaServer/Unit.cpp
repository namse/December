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



void Unit::UnitMove(Game* game, ActionData* actionData)
{
	UnitMoveType	unitMoveType = m_UnitMoveType;
	Coord			unitPos = m_Position;
	int				unitAtk = m_Attack;

	int				range = actionData->range;
	HexaDirection	direction = actionData->direction;
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
							 Unit* standUnit = game->GetUnitInPosition(movePos + GetUnitVector(direction));
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
					 movePos = Coord(unitPos + (GetUnitVector(direction) * range));
					 moveRange = range;
					 Unit* standUnit = game->GetUnitInPosition(movePos);
					 if (nullptr != standUnit)
					 {
						 // 그럼 호..혹시 그 전칸에도 유닛이 있니?
						 movePos = Coord(movePos - GetUnitVector(direction));
						 moveRange = range - 1;
						 Unit* standUnitBefore = game->GetUnitInPosition(movePos);

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
					 for (int move = 0; move < range; ++move)
					 {
						 HexaDirection direction = GetHexaDirection(movePos, actionData->position[move]);
						 // 아! 물론 이동하려는 위치에 유닛이 있으면
						 Unit* standUnit = game->GetUnitInPosition(actionData->position[move]);
						 if (nullptr != standUnit)
						 {
							 crashGuy = standUnit;
							 break;
						 }
						 // 유닛을 만나지 않으면 계속 질주하세욧!
						 else
						 {
							 SetPosition(actionData->position[move]);

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
							 dashAction.mUnitId = m_ID;
							 dashAction.mMoveData.mRange = 1;
							 dashAction.mMoveData.mDirection = direction;
							 dashAction.mMoveData.mFinalX = actionData->position[move].x;
							 dashAction.mMoveData.mFinalY = actionData->position[move].y;

							 game->SetActionQueue(&dashAction);
#ifdef _DEBUG
							 game->PrintUnitActionQueue(dashAction);
#endif
						 }
						 movePos = actionData->position[move];
					 }

	}break;

	case UMT_TELEPORT:
	{
						 if (game->GetUnitInPosition(actionData->position[0]) != nullptr) return;

						 if (game->GetField()->IsInsideOfField(actionData->position[0]) == false) return;

						 actionType = UAT_TELEPORT;
						 movePos = actionData->position[0];
	}break;

	default:
		assert(false && "HandleAction In UnitMoveType Wrong");
	}


	// 액션큐에 이동 위치를 담아줍니다. 대쉬 유닛의 경우 패스
	if (unitMoveType != UMT_DASH)
	{
		SetPosition(movePos);

		UnitAction action;
		action.mActionType = actionType;
		action.mUnitId = m_ID;
		action.mMoveData.mRange = moveRange;
		action.mMoveData.mDirection = direction;
		action.mMoveData.mFinalX = movePos.x;
		action.mMoveData.mFinalY = movePos.y;

		game->SetActionQueue(&action);
#ifdef _DEBUG
		game->PrintUnitActionQueue(action);
#endif
	}


	// 충돌유닛이 있으면
	if (nullptr != crashGuy)
	{
		if (m_Owner != crashGuy->GetOwner())
		{
			// 적이면 서로 데미지 먹임
			crashGuy->SetHP(crashGuy->GetHP() - m_Attack);
			SetHP(m_HP - crashGuy->GetAttack());
		}

		if (m_HP <= 0)
			crashGuy->KillThisUnit(game);

		UnitAction collisionAct;
		collisionAct.mActionType = UAT_COLLISION;
		collisionAct.mUnitId = m_ID;
		collisionAct.mCollisionData.mTarget = crashGuy->GetID();
		collisionAct.mCollisionData.mMyHP = crashGuy->GetHP();
		collisionAct.mCollisionData.mTargetHP = crashGuy->GetHP();

		game->SetActionQueue(&collisionAct);
#ifdef _DEBUG
		game->PrintUnitActionQueue(collisionAct);
#endif

		// 얻어맞은 유닛은 죽은 후에도 밀려난 다음에 사망
		if (crashGuy->GetHP() <= 0)
			crashGuy->KillThisUnit(game);

		// 유닛이 맞아죽지 않았으면 밀려나게 합니다
		if (crashGuy->GetUnitStatus() != UST_DEAD)
		{
			if (unitAtk > 0) crashGuy->UnitPush(game, unitAtk, direction);
		}
	}
	else if (!game->GetField()->IsInsideOfField(movePos))
	{
		KillThisUnit(game);
	}
	return;

}

// 연쇄충돌 처리
void Unit::UnitPush(Game* game, int power, HexaDirection direction)
{
	Coord			unitPos = m_Position;
	int				unitWeight = m_Weight;
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
		Unit* standUnit = game->GetUnitInPosition(movePos + GetUnitVector(direction));
		if (nullptr != standUnit) // 서있을 시 충돌
		{
			crashGuy = standUnit;
			break;
		}
		movePos = movePos + GetUnitVector(direction);
		moveRange++;
	}

	// 액션큐에 이동 위치를 담아줍니다
	SetPosition(movePos);

	UnitAction action;
	action.mActionType = UAT_MOVE;
	action.mUnitId = m_ID;
	action.mMoveData.mRange = moveRange;
	action.mMoveData.mDirection = direction;
	action.mMoveData.mFinalX = movePos.x;
	action.mMoveData.mFinalY = movePos.y;

	game->SetActionQueue(&action);
#ifdef _DEBUG
	game->PrintUnitActionQueue(action);
#endif


	// 충돌유닛이 있으면
	if (nullptr != crashGuy)
	{
		if (m_Owner != crashGuy->GetOwner())
		{
			// 적이면 서로 데미지 먹임
			crashGuy->SetHP(crashGuy->GetHP() - m_Attack);
			SetHP(m_HP - crashGuy->GetAttack());
		}

		if (m_HP <= 0)
			crashGuy->KillThisUnit(game);

		UnitAction collisionAct;
		collisionAct.mActionType = UAT_COLLISION;
		collisionAct.mUnitId = m_ID;
		collisionAct.mCollisionData.mTarget = crashGuy->GetID();
		collisionAct.mCollisionData.mMyHP = crashGuy->GetHP();
		collisionAct.mCollisionData.mTargetHP = crashGuy->GetHP();

		game->SetActionQueue(&collisionAct);
#ifdef _DEBUG
		game->PrintUnitActionQueue(collisionAct);
#endif


		if (crashGuy->GetHP() <= 0)
			crashGuy->KillThisUnit(game);

		// 유닛이 맞아죽지 않았으면 밀려나게 합니다
		if (crashGuy->GetUnitStatus() != UST_DEAD)
		{
			int pushPower = power - moveRange;
			if (pushPower > 0) crashGuy->UnitPush(game, pushPower, direction);
		}
	}
	else if (!game->GetField()->IsInsideOfField(movePos))
	{
		KillThisUnit(game);
	}
	return;
}

void Unit::KillThisUnit(Game* game)
{
	setStatus(UST_DEAD);

	UnitAction action;
	action.mActionType = UAT_DIE;
	action.mUnitId = m_ID;

	game->SetActionQueue(&action);
#ifdef _DEBUG
	game->PrintUnitActionQueue(action);
#endif

	// 유닛 수 초기화
	game->UnitCounting();
}