#pragma once

#include <WinSock2.h>
#include "Config.h"
#include "..\..\PacketType.h"
#include "ObjectPool.h"


/// 모든 DB관련 작업은 아래의 추상 클래스를 상속받아 사용해야 함
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