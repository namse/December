#pragma once
#include "CommonDefinitions.h"
#include "../../CommonDefinitions.h"


enum PacketTypes
{
	PKT_NONE = 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_SC_GAME_START = 3,

	PKT_CS_ACTION = 4,
	PKT_SC_ACTION = 5,

	PKT_SC_YOUR_TURN = 6,

	PKT_SC_GAME_OVER = 7,

	PKT_SC_COST_RENEWAL = 8,

	PKT_CS_SKILL = 9,

	PKT_SC_CHANGE_FIELD = 10,

	PKT_CS_TOSS_TURN = 11,

	PKT_SC_WARN = 12,

	PKT_SC_SETUNIT = 13,

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

	struct ActionRequest : public PacketHeader
	{
		ActionRequest(){
			mSize = sizeof(ActionRequest);
			mType = PKT_CS_ACTION;
		}
		ActionData				mAction;
	};

	struct TossTurnRequest : public PacketHeader
	{
		TossTurnRequest(){
			mSize = sizeof(TossTurnRequest);
			mType = PKT_CS_TOSS_TURN;
		}
	};

	struct AttackResult : public PacketHeader
	{
		AttackResult(){
			mSize = sizeof(AttackResult);
			mType = PKT_SC_ACTION;
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

	struct FieldBlockWarning : public PacketHeader
	{
		FieldBlockWarning(){
			mSize = sizeof(FieldBlockWarning);
			mType = PKT_SC_WARN;
		}
		Coord pos;
	};
}

#pragma pack(pop)