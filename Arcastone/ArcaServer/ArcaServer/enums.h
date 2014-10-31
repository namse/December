#pragma once

typedef int PlayerNumber;
typedef int GameNumber;
typedef int UnitNumber;

enum UnitType{
	PC,
	NPC,
	ITEM,
};

enum PCType{
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	KING,
};

enum NPCType{
	ARCASTONE,

};

enum UnitMoveType{
	NONE,
	STRAIGHT,
	DASH,
	JUMP,
	TELEPORT,
};

enum HexaDirection{
	NORTH,
	NORTHEAST,
	NORTHWEST,
	SOUTHEAST,
	SOUTHWEST,
	SOUTH,
};