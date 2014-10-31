#pragma once

#define PLAYER_UNIT_MAX 6
#define MAP_WIDTH 7
#define MAP_HEIGHT 8

typedef int PlayerNumber;
typedef int GameNumber;
typedef int ObjectNumber;
typedef int GroupNumber;

typedef struct coord{
	int x, y;

	// ������ �������̵�
	bool operator < (const coord& right) const
	{
		if ((x < right.x) || (x == right.x && y < right.y))
			return true;
		else
			return false;
	}
	
	bool operator > (const coord& right) const
	{
		if ((x > right.x) || (x == right.x && y > right.y))
			return true;
		else
			return false;
	}

	bool operator == (const coord& right) const
	{
		if ((x == right.x) && (y == right.y))
			return true;
		else
			return false;
	}
}Coord;

typedef struct{
	Coord			position;
	ObjectNumber	objectNum;
}FieldBlock;

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