#include "stdafx.h"
#include "Unit.h"


Unit::Unit()
{
	m_ObjectType = OT_UNIT;
}

Unit::~Unit()
{
}

void Unit::InitObject(ObjectNumber objectNum, PlayerNumber playerNum)
{
	GameObject::InitObject(objectNum);
	m_PlayerNum = playerNum;
}
