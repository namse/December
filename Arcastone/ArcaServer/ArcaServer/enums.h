#pragma once
#include "../../PacketType.h"
#define PLAYER_UNIT_MAX 6
#define MAP_FIELD_WIDTH 7
#define MAP_FIELD_HEIGHT 10
#define PLAYER_NUMBER_NPC INT_MAX
#define MAX_TURN	 2

typedef int PlayerNumber;
typedef int GameNumber;

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
	Coord operator * (const int& rhs) const{
		return Coord(x*rhs, y*rhs);
	}
	Coord operator / (const int& rhs) const{
		return Coord(x / rhs, y / rhs);
	}
};

struct FieldBlock{
	Coord			m_Position;
	FieldBlockType	m_Type;
	FieldBlockStatus m_Status;
};



// enum FieldBlockType
// enum FieldBlockStatus
// enum UnitType
// enum UnitMoveType
// enum HexaDirection
// Above guys is in PacketType.h