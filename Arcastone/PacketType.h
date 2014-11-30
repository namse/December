#pragma once
#include "CommonDefinitions.h"
#include "../../CommonDefinitions.h"


enum PacketTypes
{
	PKT_NONE = 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_SC_GAME_START = 3,

	PKT_CS_ATTACK = 4,
	PKT_SC_ATTACK = 5,

	PKT_SC_YOUR_TURN = 6,

	PKT_SC_GAME_OVER = 7,

	PKT_SC_COST_RENEWAL = 8,

	PKT_CS_SKILL = 9,

	PKT_SC_CHANGE_FIELD = 10,

	PKT_MAX	= 1024,
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize;
	short mType;
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
			mReverseMap = false;
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
		bool					mReverseMap;
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
			mIsDraw = false;
			mIsMyWin = false;
		}
		bool mIsDraw;
		bool mIsMyWin;
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

	struct ChangeFieldResult : public PacketHeader
	{
		ChangeFieldResult(){
			mSize = sizeof(ChangeFieldResult);
			mType = PKT_SC_CHANGE_FIELD;
		}
		FieldBlock mFieldBlock;
	};
}

#pragma pack(pop)