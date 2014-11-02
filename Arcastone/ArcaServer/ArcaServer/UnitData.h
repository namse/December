#pragma once

// Unit Data For Player's Group. not for ingame.
// so, when game start, just copy this data and make GameObjects.
struct UnitData
{
	UnitData()
	{
		m_UnitMoveType = UMT_NONE;
		m_HP = m_Attack = m_Weight = m_MoveRange = -1;
	};

	UnitType		m_UnitType;
	UnitMoveType	m_UnitMoveType;
	int				m_HP;
	int				m_Attack;
	int				m_Weight;
	int				m_MoveRange;
};

