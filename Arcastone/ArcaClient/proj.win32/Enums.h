#pragma once

#define MAP_SCALEX					15
#define MAP_SCALEY					25
#define MAX_PLAYER_HAVE_UNITS		10

enum PieceType
{

};

enum MoveType
{

};

enum AttackType
{

};

struct UnitStatus
{
	int Hp;
	int Atk;
	int MoveRange;
	int Weight;

	PieceType PieceType;
	MoveType MoveType;
	AttackType AttackType;
};

struct Position
{
	int x;
	int y;
};

class Enums
{
public:
	Enums();
	~Enums();
};

