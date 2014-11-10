#pragma once

#include "Header.h"
#include "Header.h"

class Unit
{
public:
	virtual ~Unit();

	static Unit*				create(UnitData unitData);
	virtual void				init(UnitData unitData);
	virtual void				initSprite();

	bool						setPosition(HexaPoint setPos);
	void						setHP(int hp);

	Sprite*						getSprite(){ return m_Sprite; }
	int							getHP(){ return m_HP; }
	int							getID(){ return m_ID; }
	int							getAttack(){ return m_Attack; }
	int							getMoveRange(){ return m_MoveRange; }
	int							getWeight(){ return m_Weight; }
	HexaPoint					getPosition(){ return m_Position; }
	UnitOwner					getOwner(){ return m_Owner; }
	UnitMoveType					getMoveType(){ return m_MoveType; }
	
protected:
	UnitType					m_UnitType;
	UnitMoveType				m_MoveType;
	UnitOwner					m_Owner;
	UnitStatusType				m_State;

	int							m_ID;
	int							m_HP;
	int							m_Attack;
	int							m_MoveRange;
	int							m_Weight;
	
	HexaPoint					m_Position;

	Sprite*						m_Sprite;
};

