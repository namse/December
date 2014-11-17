#pragma once
#include "CommonDefinitions.h";
#pragma pack(push, 1)

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

	PKT_SC_WRONG_ATTACK = 8,

	PKT_SC_COST_RENEWAL = 9,

	PKT_MAX = 1024
};

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