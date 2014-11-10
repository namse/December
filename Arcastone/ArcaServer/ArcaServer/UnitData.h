#pragma once

// Unit Data For Player's Group. not for ingame.
// so, when game start, just copy this data and make GameObjects.
struct UnitData
{
	UnitData()
	{
		m_UnitType = UT_NONE;
		m_UnitMoveType = UMT_NONE;
		m_HP = m_Attack = m_Weight = m_MoveRange = m_Position.x = m_Position.y -1;
	};

	UnitType		m_UnitType;
	UnitMoveType	m_UnitMoveType;
	int				m_HP;
	int				m_Attack;
	int				m_Weight;
	int				m_MoveRange;
	Coord			m_Position;			// 여러가지 유닛 배치가 있으므로, 좌표도 저장해야함
};

