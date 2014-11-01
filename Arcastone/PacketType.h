#pragma once

#define MAX_CHAT_LEN	256

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40
#define MAX_UNIT_ON_GAME 30

enum PacketTypes
{
	PKT_NONE	= 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_SC_GAME_START = 3,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize ;
	short mType ;
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
	UMT_NONE,
	UMT_STRAIGHT,
	UMT_DASH,
	UMT_JUMP,
	UMT_TELEPORT,
};

enum HexaDirection{
	HD_NORTH,
	HD_NORTHEAST,
	HD_NORTHWEST,
	HD_SOUTHEAST,
	HD_SOUTHWEST,
	HD_SOUTH,
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
			memset(mData, 0, sizeof(mData));
		}
		int mLength;
		struct UnitData{
			int x, y;
			int hp;
			int weight;
			// TODO : Add What Client Need
		};
		UnitData mData[MAX_UNIT_ON_GAME];
	};
}


#pragma pack(pop)