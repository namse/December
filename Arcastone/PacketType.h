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

enum PacketTypes
{
	PKT_NONE	= 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_SC_GAME_START = 3,

	PKT_CS_ATTACK = 4,
	PKT_SC_ATTACK = 5,

	PKT_SC_YOUR_TURN = 6,

	PKT_SC_GAME_OVER = 7,

	PKT_SC_COST_RENEWAL = 8,

	PKT_CS_SKILL = 9,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

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
	UO_NONE=0,
	UO_ME=1,
	UO_ENEMY=2,
	UO_NPC=3,
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
	UnitIdentityNumber	id;
	UnitMoveType		attackType;
	int					range;
	HexaDirection		direction;
	Coord				position[MAX_MOVE_RANGE];
};

struct SkillData{
	UnitIdentityNumber	id;
	UnitSkillType		skillType;
	UnitSkillRank		skillRank;
	int					range;
	HexaDirection		direction;
	Coord				position[MAX_SKILL_RANGE];
};

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize;
	short mType;
};

struct FieldBlock{
	FieldBlockType	m_Type;
	FieldBlockStatus m_Status;
	Coord m_Position;
};

namespace Packet
{
	struct LoginRequest : public PacketHeader
	{
		LoginRequest()
		{
			mSize = sizeof(LoginRequest);
			mType = PKT_CS_LOGIN;
		}
	};


	struct LoginResult : public PacketHeader
	{
		LoginResult()
		{
			mSize = sizeof(LoginResult);
			mType = PKT_SC_LOGIN;
		}
	};

	struct GameStartResult : public PacketHeader
	{
		GameStartResult(){
			mSize = sizeof(GameStartResult);
			mType = PKT_SC_GAME_START;
			mUnitLength = 0;
			mFieldLength = 0;
			mFieldSizeX = 0;
			mFieldSizeY = 0;
			memset(mUnit, 0, sizeof(mUnit));
			memset(mFieldList, 0, sizeof(mFieldList));
		}
		struct UnitData{
			UnitType			unitType;
			UnitMoveType		unitMoveType;
			UnitStatusType		unitStatusType;
			UnitOwner			unitOwner;
			int					hp;
			int					weight;
			int					attack;
			int					moveRange;
			Coord				point;
			UnitIdentityNumber	id;
		};
		int						mUnitLength;
		int						mFieldLength;
		int						mFieldSizeX, mFieldSizeY;
		UnitData				mUnit[MAX_UNIT_ON_GAME];
		FieldBlock				mFieldList[MAX_FIELD_BLOCK];
	};

	struct AttackRequest : public PacketHeader
	{
		AttackRequest(){
			mSize = sizeof(AttackRequest);
			mType = PKT_CS_ATTACK;
		}
		AttackData				mAttack;
	};

	struct SkillRequest : public PacketHeader
	{
		SkillRequest(){
			mSize = sizeof(SkillRequest);
			mType = PKT_CS_SKILL;
		}
		SkillData			mSkill;
	};

	struct AttackResult : public PacketHeader
	{
		AttackResult(){
			mSize = sizeof(AttackResult);
			mType = PKT_SC_ATTACK;
			mQueueLength = 0;
			memset(mUnitActionQueue, 0, sizeof(mUnitActionQueue));
		}
		int mQueueLength;
		UnitAction mUnitActionQueue[MXX_UNIT_ACTION_QUEUE];
	};

	struct YourTurnResult : public PacketHeader
	{
		YourTurnResult(){
			mSize = sizeof(YourTurnResult);
			mType = PKT_SC_YOUR_TURN;
			mIsReallyMyTurn = false;
		}
		bool mIsReallyMyTurn; // if it's false, that may 99% mean Enemy Turn Now.
	};

	struct GameOverResult : public PacketHeader
	{
		GameOverResult(){
			mSize = sizeof(GameOverResult);
			mType = PKT_SC_GAME_OVER;
			mWhoIsWinner = WW_NONE;
		}
		WhosWinner mWhoIsWinner;
	};

	struct CostRenewalResult : public PacketHeader
	{
		CostRenewalResult(){
			mSize = sizeof(CostRenewalResult);
			mType = PKT_SC_COST_RENEWAL;
			mCost = -1;
			mMaxCost = -1;
		}
		int mCost, mMaxCost;
	};
}


#pragma pack(pop)