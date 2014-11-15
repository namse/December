﻿#pragma once

#include <map>
#include <WinSock2.h>

class ClientSession ;
struct PacketHeader ;
struct DatabaseJobContext ;

class ClientManager
{
public:
	ClientManager() : mLastGCTick(0)
	{}

	ClientSession* CreateClient(SOCKET sock);
	
	void BroadcastPacket(ClientSession* from, PacketHeader* pkt) ;

	void OnPeriodWork() ;

	/// DB에 플레이어 정보를 생성하거나 삭제하는 함수
	void CreatePlayer(int pid, double x, double y, double z, const char* name, const char* comment) ;
	void DeletePlayer(int pid) ;

	void FlushClientSend();

	ClientSession* GetClient(PlayerNumber playerID);

private:
	void CreatePlayerDone(DatabaseJobContext* dbJob) ;
	void DeletePlayerDone(DatabaseJobContext* dbJob) ;

private:
	void CollectGarbageSessions() ;
	void DispatchDatabaseJobResults() ;
	

private:
	typedef std::map<SOCKET, ClientSession*> ClientList;
	ClientList	mClientList ;

	DWORD		mLastGCTick ;
} ;

extern ClientManager* GClientManager ;