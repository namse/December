#include "stdafx.h"
#include "ClientSession.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"

//@{ Handler Helper

typedef void (*HandlerFunc)(ClientSession* session);

static HandlerFunc HandlerTable[PKT_MAX];

static void DefaultHandler(ClientSession* session)
{
	printf("[DEBUG] Invalid packet handler", session->GetPlayerId());
	session->Disconnect();
}

struct InitializeHandlers
{
	InitializeHandlers()
	{
		for (int i = 0; i < PKT_MAX; ++i)
			HandlerTable[i] = DefaultHandler;
	}
} _init_handlers_;

struct RegisterHandler
{
	RegisterHandler(int pktType, HandlerFunc handler)
	{
		HandlerTable[pktType] = handler;
	}
};

#define REGISTER_HANDLER(PKT_TYPE)	\
	static void Handler_##PKT_TYPE(ClientSession* session); \
	static RegisterHandler _register_##PKT_TYPE(PKT_TYPE, Handler_##PKT_TYPE); \
	static void Handler_##PKT_TYPE(ClientSession* session)

//@}

void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject;

	fromClient->DecRefCount();

	if (!fromClient->IsConnected())
		return;

	/// ���� �߻��� �ش� ���� ����
	if (dwError || cbTransferred == 0)
	{
		fromClient->Disconnect();
		return;
	}

	/// ���� ������ ó��
	fromClient->OnRead(cbTransferred);

	/// �ٽ� �ޱ�
	if (false == fromClient->PostRecv())
	{
		fromClient->Disconnect();
		return;
	}
}


void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject;

	fromClient->DecRefCount();

	if (!fromClient->IsConnected())
		return;

	/// ���� �߻��� �ش� ���� ����
	if (dwError || cbTransferred == 0)
	{
		fromClient->Disconnect();
		return;
	}

	fromClient->OnWriteComplete(cbTransferred);

}

///////////////////////////////////////////////////////////

void ClientSession::OnRead(size_t len)
{
	mRecvBuffer.Commit(len);

	/// ��Ŷ �Ľ��ϰ� ó��
	while (true)
	{
		/// ��Ŷ ��� ũ�� ��ŭ �о�ͺ���
		PacketHeader header;
		if (false == mRecvBuffer.Peek((char*)&header, sizeof(PacketHeader)))
			return;

		/// ��Ŷ �ϼ��� �Ǵ°�? 
		if (mRecvBuffer.GetStoredSize() < (size_t) header.mSize)
			return;


		if (header.mType >= PKT_MAX || header.mType <= PKT_NONE)
		{
			printf("[DEBUG] Invalid packet type", GetPlayerId());
			Disconnect();
			return;
		}

		/// packet dispatch...
		HandlerTable[header.mType](this);
	}
}

/////////////////////////////////////////////////////////
REGISTER_HANDLER(PKT_CS_LOGIN)
{
	LoginRequest inPacket;
	if (false == session->ParsePacket(inPacket))
	{
		printf("[DEBUG] packet parsing error", inPacket.mType);
		return;
	}

	LoadPlayerDataContext* newDbJob = new LoadPlayerDataContext(session->GetSocketKey(), inPacket.mPlayerId);
	GDatabaseJobManager->PushDatabaseJobRequest(newDbJob);

}

REGISTER_HANDLER(PKT_CS_CHAT)
{
	ChatBroadcastRequest inPacket;
	if (false == session->ParsePacket(inPacket))
	{
		printf("[DEBUG] packet parsing error", inPacket.mType);
		return;
	}

	if (inPacket.mPlayerId != session->GetPlayerId())
	{
		printf("[DEBUG] PKT_CS_CHAT: invalid player ID", session->GetPlayerId());
		return;
	}

	/// chatting�� ��� ���⼭ �ٷ� ���
	
	ChatBroadcastResult outPacket;
	outPacket.mPlayerId = inPacket.mPlayerId;
	strcpy_s(outPacket.mName, session->GetPlayerName());
	strcpy_s(outPacket.mChat, inPacket.mChat);

	/// ä���� �ٷ� ��� �ϸ� ��
	if (!session->Broadcast(&outPacket))
	{
		session->Disconnect();
	}
}


REGISTER_HANDLER(PKT_CS_MOVE)
{
	MoveRequest inPacket;
	if (false == session->ParsePacket(inPacket))
	{
		printf("[DEBUG] packet parsing error", inPacket.mType);
		return;
	}

	if (inPacket.mPlayerId != session->GetPlayerId())
	{
		printf("[DEBUG] PKT_CS_MOVE: invalid player ID", session->GetPlayerId());
		return;
	}

	/// �÷��̾� ��ǥ ������Ʈ�ϰ� �ٷ� ����ϸ� ��
	session->SetPosition(inPacket.mPosX, inPacket.mPosY);

	MoveBroadcastResult outPacket;
	outPacket.mPlayerId = inPacket.mPlayerId;
	outPacket.mPosX = inPacket.mPosX;
	outPacket.mPosY = inPacket.mPosY;

	if (!session->Broadcast(&outPacket))
	{
		session->Disconnect();
	}

}





