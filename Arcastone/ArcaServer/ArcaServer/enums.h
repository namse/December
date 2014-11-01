#pragma once
#include "../../PacketType.h"
#define PLAYER_UNIT_MAX 6
#define MAP_WIDTH 7
#define MAP_HEIGHT 8

typedef int PlayerNumber;
typedef int GameNumber;

#define PLAYER_NUMBER_NPC INT_MAX

struct Coord{
	int x, y;
	Coord()
	{
		x = y = 0;
	}
	Coord(int _x, int _y)
	{
		x = _x, y = _y;
	}
	// 연산자 오버로딩
	bool operator < (const Coord& right) const
	{
		if ((x < right.x) || (x == right.x && y < right.y))
			return true;
		else
			return false;
	}

	bool operator >(const Coord& right) const
	{
		if ((x > right.x) || (x == right.x && y > right.y))
			return true;
		else
			return false;
	}

	bool operator == (const Coord& right) const
	{
		if ((x == right.x) && (y == right.y))
			return true;
		else
			return false;
	}

 	Coord operator + (const Coord& rhs) const{
 		return Coord(x + rhs.x, y + rhs.y);
	}

	Coord operator - (const Coord& rhs) const{
		return Coord(x + rhs.x, y + rhs.y);
	}
};



enum FieldBlockType
{
	FBT_NONE,
};
enum FieldBlockStatus
{
	FBS_NONE,
};

struct FieldBlock{
	Coord			m_Position;
	FieldBlockType	m_Type;
	FieldBlockStatus m_Status;
};




// enum UnitType
// enum UnitMoveType
// enum HexaDirection
// Above guys is in PacketType.h