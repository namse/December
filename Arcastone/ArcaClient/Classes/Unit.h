#pragma once

#include "Header.h"

class Unit
{
public:
	Unit();
	virtual ~Unit();

	static Unit*				create(UnitData unitData);
	virtual void				init(UnitData unitData);
	virtual void				initSprite();

	void						setPosition(HexaPoint setPos){ m_Position = setPos; }
	void						setScreenPos(ScreenPoint setPos);
	void						SetHP(int hp);
	void						SetStatus(UnitStatusType status){ m_StatusType = status; }

	Sprite*						GetSprite(){ return m_Sprite; }
	int							GetHP(){ return m_HP; }
	int							GetID(){ return m_ID; }
	int							GetAttack(){ return m_Attack; }
	int							GetMoveRange(){ return m_MoveRange; }
	int							GetWeight(){ return m_Weight; }
	HexaPoint					GetPosition(){ return m_Position; }
	UnitOwner					GetOwner(){ return m_Owner; }
	UnitMoveType				GetMoveType(){ return m_MoveType; }
	UnitStatusType				GetStatusType(){ return m_StatusType; }
	Skill						GetSkill(){ return m_Skill; }
	
protected:
	UnitType					m_UnitType;
	UnitMoveType				m_MoveType;
	UnitOwner					m_Owner;
	UnitStatusType				m_StatusType;
	Skill						m_Skill;

	int							m_ID;
	int							m_HP;
	int							m_Attack;
	int							m_MoveRange;
	int							m_Weight;
	
	HexaPoint					m_Position;

	Sprite*						m_Sprite;
};

