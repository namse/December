#pragma once

enum UnitType
{
	UT_PC,
	UT_NPC,
	UT_NONE,
};

enum PieceType
{
	PT_PAWN,
	PT_NONE,
};

enum MoveType
{
	MT_NORMAL,
	NT_NONE,
};

enum AttackType
{
	AT_NORMAL,
	AT_NONE,
};

enum PlayerWho
{
	PW_PLAYERONE = 0,
	PW_PLAYERTWO = 1,
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

struct Hexagon
{
	cocos2d::Point anchor;
	std::vector<cocos2d::Point> vertex;
};

class Enums
{
public:
	Enums();
	~Enums();
};

