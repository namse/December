#pragma once

#define MAX_CHAT_LEN		256

#define MAX_NAME_LEN		30
#define MAX_COMMENT_LEN		40
#define MAX_UNIT_ON_GAME	30
#define MAX_UNIT_ACTION		50
#define MAX_FIELD_BLOCK		7*11

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

	PKT_SC_WRONG_ATTACK = 8,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize;
	short mType;
};

enum WrongAttackType
{
	WAT_NONE = 0,
	WAT_NOT_YOUR_TURN = 1,
	WAT_NO_ENOUGH_COST = 2,
};


enum UnitType{
	UT_NONE = 0,

	UT_SOLDIER = 1,
	UT_KNIGHT = 2,
	UT_BISHOP = 3,
	UT_ROOK = 4,
	UT_KING = 5,
	UT_ARCASTONE = 6,
	UT_RIDER = 7,

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
	//FBT_FROZEN = 1,
};
enum FieldBlockStatus
{
	FBS_NONE = 0,
	//FBS_BURNIG = 1,
};

struct FieldBlock{
	FieldBlockType	m_Type;
	FieldBlockStatus m_Status;
	Coord m_Position;
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

typedef int UnitIdentityNumber;

struct AttackData{
	UnitIdentityNumber id;
	int					Range;
	HexaDirection		direction;
};

enum UnitActionType{
	UAT_NONE = 0,
	UAT_MOVE = 1,
	UAT_DIE = 2,
	UAT_COLLISION = 3,
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
			mLength = 0;
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
		int						mLength;
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

	struct AttackResult : public PacketHeader
	{
		AttackResult(){
			mSize = sizeof(AttackResult);
			mType = PKT_SC_ATTACK;
			mQueueLength = 0;
			memset(mUnitActionQueue, 0, sizeof(mUnitActionQueue));
		}
		int mQueueLength;
		UnitAction mUnitActionQueue[50];
	};

	struct WrongAttackResult : public PacketHeader // 너 공격(스킬) 이상하게했어 임마
	{
		WrongAttackResult(){
			mSize = sizeof(WrongAttackResult);
			mType = PKT_SC_WRONG_ATTACK;
			mWrongType = WAT_NONE;
		}
		WrongAttackType mWrongType;
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
}


#pragma pack(pop)