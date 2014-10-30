#pragma once

#define MAX_CHAT_LEN	256

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40

enum PacketTypes
{
	PKT_NONE	= 0,

	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize ;
	short mType ;
} ;


/*
yeje
struct LoginRequest : public PacketHeader
{
	LoginRequest()
	{
		mSize = sizeof(LoginRequest) ;
		mType = PKT_CS_LOGIN ;
		mPlayerId = -1 ;
	}

	int	mPlayerId ;
} ;
*/

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

}


#pragma pack(pop)