#pragma once

#define MAX_CHAT_LEN		256

#define MAX_NAME_LEN		30
#define MAX_COMMENT_LEN		40
#define MAX_UNIT_ON_GAME	30
#define MAX_UNIT_ACTION		50
#define MAX_FIELD_BLOCK		7*11
#define MAX_MOVE_RANGE		20
#define MAX_SKILL_RANGE		20
#define MXX_UNIT_ACTION_QUEUE 64

typedef int UnitIdentityNumber;

enum WrongAttackType
{
	WAT_NONE = 0,
	WAT_NOT_YOUR_TURN = 1,
	WAT_NO_ENOUGH_COST = 2,
	WAT_CANT_TELEPORT_THERE = 3,
	WAT_CANT_JUMP_THERE = 4,
};

enum UnitType{
	UT_NONE = 0,

	UT_SOLDIER = 1,
	UT_KNIGHT = 2,
	UT_MAGICIAN = 3,
	UT_ROOK = 4,
	UT_PRINCESS = 5,
	UT_PRINCE = 6,
	UT_ARCASTONE = 7,
	UT_RIDER = 8,
	UT_ROCK = 9,
	UT_TREE = 10,
	UT_BOMB = 11,
	UT_PEBBLE = 12,
	UT_POTION = 13,

	UT_MAX = 1024,
};

enum UnitMoveType{
	UMT_NONE = 0,

	UMT_STRAIGHT = 1,
	UMT_DASH = 2,
	UMT_JUMP = 3,
	UMT_TELEPORT = 4,
};

enum UnitStatusType{
	UST_NONE = 0,
	UST_DEAD = 1,
	UST_CASTING = 2,
	UST_UNABLE_ATTACK = 3,
	// 그 외 상태이상들
};

enum UnitAttackOrSkill{
	UAS_NONE = 0,
	UAS_ATTACK = 1,
	UAS_SKILL = 2,
};

enum HexaDirection{
	HD_NONE = 0,

	// 시계 반대방향
	HD_NORTHEAST = 1,
	HD_NORTH = 2,
	HD_NORTHWEST = 3,
	HD_SOUTHWEST = 4,
	HD_SOUTH = 5,
	HD_SOUTHEAST = 6,
};

enum FieldBlockType
{
	// 필드 블록의 종류 (ex-돌바닥, 흙바닥, 강, 눈길 등)
	FBT_NONE = 0,
	FBT_NORMAL = 1,
	FBT_PITCHING = 2,
	FBT_GRASS = 3,
	FBT_WATER = 4,
	FBT_COVER_WATER = 5,
};

enum FieldBlockStatus
{
	// 필드의 상태 (안개, 구멍, 불탐, 빙결, 물)
	FBS_NONE = 0,
	FBS_NORMAL = 1,
	FBS_HOLE = 2,
	FBS_WATER = 3,
	FBS_WARN = 4,
};

enum UnitOwner
{
	UO_NONE = 0,
	UO_ME = 1,
	UO_ENEMY = 2,
	UO_NPC = 3,
};

enum WhosWinner
{
	WW_NONE = 0,
	WW_USER1 = 1,
	WW_USER2 = 2,
	WW_DRAW = 3,	// 무승부
};

enum UnitActionType{
	UAT_NONE = 0,
	UAT_MOVE = 1,
	UAT_STRAIGHT = 2,
	UAT_JUMP = 3,
	UAT_DASH = 4,
	UAT_TELEPORT = 5,
	UAT_DIE = 6,
	UAT_COLLISION = 7,
};

enum UnitSkillType{
	USK_NONE = 0,
	USK_FIREBALL = 1,
	USK_STAMP = 2,
};

enum UnitSkillRank{
	USR_NONE = 0,
	USR_RANKONE = 1,
	USR_RANKTWO = 2,
	USR_RANKTHREE = 3,
};

#pragma pack(push, 1)

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
		return Coord(x - rhs.x, y - rhs.y);
	}
	Coord operator * (const int& rhs) const{
		return Coord(x*rhs, y*rhs);
	}
	Coord operator / (const int& rhs) const{
		return Coord(x / rhs, y / rhs);
	}
};

struct UnitAction{
	// FOR MOVE
	struct MoveData
	{
		int mRange;
		HexaDirection mDirection;
		int mFinalX, mFinalY;
	};

	// FOR COLLIDE
	struct CollisionData
	{
		UnitIdentityNumber mTarget;
		int mMyHP;
		int mTargetHP;
	};

	UnitIdentityNumber mUnitId;
	UnitActionType mActionType;
	MoveData mMoveData;
	CollisionData mCollisionData;
};

struct ActionData{
	UnitAttackOrSkill	type;

	UnitIdentityNumber	id;
	UnitMoveType		attackType;

	UnitSkillType		skillType;
	UnitSkillRank		skillRank;

	/*  가변 길이로  */
	int					range;
	HexaDirection		direction;
	Coord				position[MAX_MOVE_RANGE];
};


struct FieldBlock{
	FieldBlockType	type;
	FieldBlockStatus status;
	Coord position;
};

#pragma pack(pop)