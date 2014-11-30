#pragma once

#include "UnitData.h"

class Game;
class Unit
{
public:
	Unit();
	virtual ~Unit();

	static Unit*			CreateUnit(UnitType unitType);

	void					InitUnit(UnitType unitType);

	void					SetId(UnitIdentityNumber unitId){ m_ID = unitId; }
	UnitIdentityNumber		GetID(){ return m_ID; }

	void					SetOwner(PlayerNumber playerNumber) { m_Owner = playerNumber; }
	PlayerNumber			GetOwner(){ return m_Owner; }

	void					SetPosition(Coord position) { m_Position = position; }
	Coord					GetPos(){ return m_Position; }

	void					SetHP(int hp) { m_HP = hp; }
	int						GetHP(){ return m_HP; }

	void					setStatus(UnitStatusType status){ m_UnitStatus = status; }

	UnitType				GetUnitType(){ return m_UnitType; }
	UnitStatusType			GetUnitStatus(){ return m_UnitStatus; }
	UnitMoveType			GetUnitMoveType(){ return m_UnitMoveType; }
	int						GetAttack(){ return m_Attack; }
	int						GetWeight(){ return m_Weight; }
	int						GetMoveRange(){ return m_MoveRange; }

	void					SetIsNearArca(bool is){ m_IsNearArca = is; }
	bool					GetIsNearArca(){ return m_IsNearArca; }

	void					UnitMove(Game* game, AttackData* attackData);
	void					UnitPush(Game* game, int power, HexaDirection direction);
	void					KillThisUnit(Game* game);

protected:
	PlayerNumber			m_Owner;
	UnitIdentityNumber		m_ID;

	UnitType				m_UnitType;
	// TODO : 여러개의 상태를 가질 수 있도록
	UnitStatusType			m_UnitStatus;
	UnitMoveType			m_UnitMoveType;
	int						m_HP;
	int						m_Attack;
	int						m_Weight;
	int						m_MoveRange;

	Coord					m_Position;

	bool					m_IsNearArca;	// 아르카스톤이 옆에 있는지에 대한 상태변수
};

