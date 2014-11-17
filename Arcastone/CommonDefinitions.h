#pragma once

#pragma pack(push, 1)

#define MAX_UNIT_ON_GAME	30
#define MAX_UNIT_ACTION		50
#define MAX_FIELD_BLOCK		7*11
#define MAX_MOVE_RANGE		20
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
	// 그 외 상태이상들
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
	FBT_NONE = 0,
	FBT_NORMAL = 1,
	FBT_HOLE = 2,
};

enum FieldBlockStatus
{
	FBS_NONE = 0,


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
	WW_PLAYER1 = 1,
	WW_PLAYER2 = 2,
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

<<<<<<< HEAD
enum UnitSkillType{
	USK_NONE = 0,
	USK_FIREBALL = 1,
};

enum UnitSkillRank{
	USR_NONE = 0,
	USR_RANKONE = 1,
	USR_RANKTWO = 2,
	USR_RANKTHREE = 3,
};

=======
>>>>>>> b14ce2695e4cd4541555c1c3d6bf7fa740d9137c
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
	UnitIdentityNumber mUnitId;
	UnitActionType mActionType;

	// FOR MOVE
	struct MoveData
	{
		int mRange;
		HexaDirection mDirection;
		int mFinalX, mFinalY;
	}mMoveData;

	// FOR COLLIDE
	struct CollisionData
	{
		UnitIdentityNumber mTarget;
		int mMyHP;
		int mTargetHP;
	}mCollisionData;
};

struct AttackData{
	UnitIdentityNumber id;
	UnitMoveType		attackType;
	int					range;
	HexaDirection		direction;
	Coord				position[MAX_MOVE_RANGE];
};

<<<<<<< HEAD
struct SkillData{
	UnitIdentityNumber	id;
	UnitSkillType		skillType;
	UnitSkillRank		skillRank;
	int					range;
	HexaDirection		direction;
	Coord				position[MAX_SKILL_RANGE];
};
=======
>>>>>>> b14ce2695e4cd4541555c1c3d6bf7fa740d9137c


struct FieldBlock{
	FieldBlockType	m_Type;
	FieldBlockStatus m_Status;
	Coord m_Position;
};


#pragma pack(pop)