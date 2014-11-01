#pragma once

#define PLAYER_UNIT_MAX 6
#define MAP_WIDTH 7
#define MAP_HEIGHT 8

typedef int PlayerNumber;
typedef int GameNumber;
typedef int ObjectNumber;
typedef int GroupNumber;

struct Coord{
	int x, y;

	// 연산자 오버라이딩
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
};

struct FieldBlock{
	Coord			position;
	ObjectNumber	objectNum;
};

enum ObjectType{
	OT_UNIT,
	OT_NPC,
	OT_ITEM,
};

enum UnitType{
	UC_PAWN,
	UC_KNIGHT,
	UC_BISHOP,
	UC_ROOK,
	UC_KING,
};

enum NPCType{
	NT_ARCASTONE,
};

enum ObjectMoveType{
	MT_NONE,
	MT_STRAIGHT,
	MT_DASH,
	MT_JUMP,
	MT_TELEPORT,
};

enum HexaDirection{
	HD_NORTH,
	HD_NORTHEAST,
	HD_NORTHWEST,
	HD_SOUTHEAST,
	HD_SOUTHWEST,
	HD_SOUTH,
};