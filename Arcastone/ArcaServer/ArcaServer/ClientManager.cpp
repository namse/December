#include "stdafx.h"
#include "ArcaServer.h"
#include "..\..\PacketType.h"
#include "ThreadLocal.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"
#include "UserManager.h"

ClientManager* GClientManager = nullptr ;

ClientSession* ClientManager::CreateClient(SOCKET sock)
{
	assert(LThreadType == THREAD_CLIENT);

	ClientSession* client = new ClientSession(sock) ;
	mClientList.insert(ClientList::value_type(sock, client)) ;

	return client ;
}



void ClientManager::BroadcastPacket(ClientSession* from, PacketHeader* pkt)
{
	///FYI: C++ STL iterator 스타일의 루프
	for (ClientList::const_iterator it=mClientList.begin() ; it!=mClientList.end() ; ++it)
	{
		ClientSession* client = it->second ;
		
		if ( from == client )
			continue ;
		
		client->SendRequest(pkt) ;
	}
}

void ClientManager::OnPeriodWork()
{
	/// 접속이 끊긴 세션들 주기적으로 정리 (1초 정도 마다 해주자)
	DWORD currTick = GetTickCount() ;
	if ( currTick - mLastGCTick >= COLLET_GARBAGESESSIONS_TICK )
	{
		CollectGarbageSessions() ;
		mLastGCTick = currTick ;
	}
}

void ClientManager::CollectGarbageSessions()
{
	std::vector<ClientSession*> disconnectedSessions ;
	
	///FYI: C++ 11 람다를 이용한 스타일
	std::for_each(mClientList.begin(), mClientList.end(),
		[&](ClientList::const_reference it)
		{
			ClientSession* client = it.second ;

			if ( false == client->IsConnected() && 0 == client->GetRefCount() )
				disconnectedSessions.push_back(client) ;
		}
	) ;
	

	///FYI: C언어 스타일의 루프
	for (size_t i=0 ; i<disconnectedSessions.size() ; ++i)
	{
		ClientSession* client = disconnectedSessions[i] ;
		mClientList.erase(client->mSocket) ;
		delete client ;
	}

}

void ClientManager::FlushClientSend()
{
	for (auto& it : mClientList)
	{
		ClientSession* client = it.second;
		if (false == client->SendFlush())
		{
			client->Disconnect();
		}
	}
}
ClientSession* ClientManager::GetClient(UserNumber userID)
{
	for (auto clientIt : mClientList)
	{
		auto client = clientIt.second;
		if (userID == client->GetUserId())
		{
			return client;
		}
	}
	return nullptr;
}
