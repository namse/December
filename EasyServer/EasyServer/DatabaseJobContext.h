#pragma once

#include <WinSock2.h>
#include "Config.h"
#include "..\..\PacketType.h"
#include "ObjectPool.h"


/// ��� DB���� �۾��� �Ʒ��� �߻� Ŭ������ ��ӹ޾� ����ؾ� ��
struct DatabaseJobContext
{
	DatabaseJobContext() : mSuccess(false)
	{}

	DatabaseJobContext(SOCKET socketKey) :  mSockKey(socketKey), mSuccess(false) 
	{}

	virtual ~DatabaseJobContext() {}

	virtual bool OnExecute() = 0 ;

	void SetSuccess(bool success) { mSuccess = success ; }

	SOCKET	mSockKey ;
	bool	mSuccess ;
} ;


/// player load �۾�
struct LoadPlayerDataContext : public DatabaseJobContext, public ObjectPool<LoadPlayerDataContext>
{
	LoadPlayerDataContext(SOCKET socketKey, int playerId) : DatabaseJobContext(socketKey)
		, mPlayerId(playerId), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
	}


	virtual bool OnExecute() ;


	int		mPlayerId ;

	///����: ���� DB������ xyz��ǥ�� double�� ���� ������, ���ӿ����� float�� xy�� ���� ����
	double	mPosX ;
	double	mPosY ;
	double	mPosZ ; 
	char	mPlayerName[MAX_NAME_LEN] ;

} ;


/// Player ���� �۾�
struct CreatePlayerDataContext : public DatabaseJobContext, public ObjectPool<CreatePlayerDataContext>
{
	CreatePlayerDataContext() 
		: mPlayerId(-1), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
		memset(mComment, 0, sizeof(mComment)) ;
	}

	virtual bool OnExecute() ;

	int		mPlayerId ;

	double	mPosX ;
	double	mPosY ;
	double	mPosZ ;
	char	mPlayerName[MAX_NAME_LEN] ;
	char	mComment[MAX_COMMENT_LEN] ;
} ;



/// Player ���� �۾�
struct DeletePlayerDataContext : public DatabaseJobContext, public ObjectPool<DeletePlayerDataContext>
{
	DeletePlayerDataContext(int playerId) : mPlayerId(playerId)
	{}

	virtual bool OnExecute() ;

	int		mPlayerId ;

} ;

/// Player ������Ʈ �۾�
struct UpdatePlayerDataContext : public DatabaseJobContext, public ObjectPool<UpdatePlayerDataContext>
{
	UpdatePlayerDataContext(SOCKET socketKey, int playerId) : DatabaseJobContext(socketKey)
		, mPlayerId(playerId), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mComment, 0, sizeof(mComment)) ;
	}

	virtual bool OnExecute() ;

	int		mPlayerId ;

	double	mPosX ;
	double	mPosY ;
	double	mPosZ ;
	char	mComment[MAX_COMMENT_LEN] ;
} ;
