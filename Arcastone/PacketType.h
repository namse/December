#pragma once

#define MAX_CHAT_LEN		256

#define MAX_NAME_LEN		30
#define MAX_COMMENT_LEN		40
#define MAX_UNIT_ON_GAME	30

enum PacketTypes
{
	PKT_NONE	= 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_SC_GAME_START = 3,

	PKT_CS_ATTACK = 4,
	PKT_SC_ATTACK = 5,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize;
	short mType;
} ;


enum UnitType{
	UT_NONE = 0,

	UT_PAWN = 1,
	UT_KNIGHT = 2,
	UT_BISHOP = 3,
	UT_ROOK = 4,
	UT_KING = 5,
	UT_ARCASTONE = 6,

	UT_MAX = 1024,
};

enum UnitMoveType{
	UMT_NONE = 0,

	UMT_STRAIGHT = 1,
	UMT_DASH = 2,
	UMT_JUMP = 3,
	UMT_TELEPORT = 4,
};

enum HexaDirection{
	HD_NONE = 0,

	HD_NORTH = 1,
	HD_NORTHEAST = 2,
	HD_NORTHWEST = 3,
	HD_SOUTHEAST = 4,
	HD_SOUTHWEST = 5,
	HD_SOUTH = 6,
};

enum FieldBlockType
{
	FBT_NONE = 0,
};
enum FieldBlockStatus
{
	FBS_NONE = 0,
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
		}
		struct UnitData{
			UnitType			unitType;
			UnitMoveType		unitMoveType;
			int					ownerPlayer;
			int					hp;
			int					weight;
			int					attack;
			int					moveRange;
			int					x, y;
		};
		struct FieldData{
			int					fieldWidth, fieldHeight;
		};
		struct GameData{
			int					attacker;
		};
		int						mLength;
		UnitData				mUnit[MAX_UNIT_ON_GAME];
		FieldData				mField;
		GameData				mGameData;
	};

	struct AttackRequest : public PacketHeader
	{
		AttackRequest(){
			mSize = sizeof(AttackRequest);
			mType = PKT_CS_ATTACK;
		}
		struct AttackData{
			int					x, y;
			int					Range;
			HexaDirection		direction;
		};
		AttackData				mAttack;
	};
}


#pragma pack(pop)