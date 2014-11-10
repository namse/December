#include <thread>
#include "TcpClient.h"
#include "platform/CCFileUtils.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "2d/CCLabel.h"
#include "GameScene.h"
#include "../../PacketType.h"

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#define sleep(x) Sleep(x)
#endif

static TcpClient* s_TcpClient = nullptr;

TcpClient::TcpClient() : m_recvBuffer(BUF_SIZE), m_sock(NULL), m_loginId(-1)
{

}

TcpClient::~TcpClient()
{
#ifndef _WIN32
	close(m_sock);
#else
	closesocket(m_sock);
	WSACleanup();
#endif

}

bool TcpClient::initialize()
{

#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;
#endif


	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		return false;

	/// thread start
	auto t = thread(CC_CALLBACK_0(TcpClient::networkThread, this));
	t.detach();

	return true;
}


TcpClient* TcpClient::getInstance()
{
	if (nullptr == s_TcpClient)
	{
		s_TcpClient = new TcpClient();
		if (false == s_TcpClient->initialize())
			return nullptr;

		string ipaddr = UserDefault::getInstance()->getStringForKey("ipaddr", string(LOGIN_IPADDR));
		int port = UserDefault::getInstance()->getIntegerForKey("port", 9001);

		s_TcpClient->connect(ipaddr.c_str(), port);
	}


	return s_TcpClient;
}

void TcpClient::destroyInstance()
{
	CC_SAFE_DELETE(s_TcpClient);
}

bool TcpClient::connect(const char* serverAddr, int port)
{
	struct hostent* host;
	struct sockaddr_in hostAddr;

	if ((host = gethostbyname(serverAddr)) == 0)
		return false;

	memset(&hostAddr, 0, sizeof(hostAddr));
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr_list[0]))->s_addr;
	hostAddr.sin_port = htons(port);

	if (SOCKET_ERROR == ::connect(m_sock, (struct sockaddr*)&hostAddr, sizeof(hostAddr)))
	{
		CCLOG("CONNECT FAILED");
		return false;
	}

	//u_long arg = 1;
	//ioctlsocket(mSocket, FIONBIO, &arg);

	/// nagle �˰��� ����
	int opt = 1;
	setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

	return true;
}

bool TcpClient::send(const char* data, int length)
{
	int count = 0;
	while (count < length)
	{
		int n = ::send(m_sock, data + count, length, 0);
		if (n == SOCKET_ERROR)
		{
			CCLOG("SEND ERROR");
			return false;
		}
		count += n;
		length -= n;
	}

	return true;
}

void TcpClient::networkThread()
{
	while (true)
	{
		char inBuf[4096] = { 0, };

		int n = ::recv(m_sock, inBuf, 4096, 0);

		if (n < 1)
		{
			sleep(0); ///< for cpu low-utilization
			continue;
		}


		if (!m_recvBuffer.Write(inBuf, n))
		{
			/// ���� ��á��. 
			assert(false);
		}

		processPacket();
	}
}

void TcpClient::processPacket()
{
	auto scheduler = Director::getInstance()->getScheduler();

	/// ��Ŷ�� �Ľ��ؼ� �ϼ��Ǵ� ��Ŷ�� ������, �ش� �ݹ��� �ҷ��ش�. 
	while (true)
	{
		PacketHeader header;

		if (false == m_recvBuffer.Peek((char*)&header, sizeof(PacketHeader)))
			break;


		if (header.mSize > m_recvBuffer.GetStoredSize())
			break;


		switch (header.mType)
		{
		case PKT_SC_LOGIN:
		{
			Packet::LoginResult recvData;
			bool ret = m_recvBuffer.Read((char*)&recvData, header.mSize);
		}
			break;
		case PKT_SC_GAME_START:
		{
			Packet::GameStartResult recvData;
			bool ret = m_recvBuffer.Read((char*)&recvData, header.mSize);

			auto director = Director::getInstance();
			auto scene = director->getRunningScene();
			auto layer = scene->getChildByName("base_layer");
			scheduler->performFunctionInCocosThread(CC_CALLBACK_0(GameScene::onGameStart, dynamic_cast<GameScene*>(layer), recvData));
		}
			break;
		case PKT_SC_YOUR_TURN:
		{
			Packet::YourTurnResult recvData;
			bool ret = m_recvBuffer.Read((char*)&recvData, header.mSize);
			auto layer = Director::getInstance()->getRunningScene()->getChildByName("base_layer");
			scheduler->performFunctionInCocosThread(CC_CALLBACK_0(GameScene::SetTurn, dynamic_cast<GameScene*>(layer), recvData.mIsReallyMyTurn));
		}break;

		case PKT_SC_ATTACK:
		{
			Packet::AttackResult recvData;
			bool ret = m_recvBuffer.Read((char*)&recvData, header.mSize);
			auto layer = Director::getInstance()->getRunningScene()->getChildByName("base_layer");
			scheduler->performFunctionInCocosThread(CC_CALLBACK_0(GameScene::ReadActionQueue, dynamic_cast<GameScene*>(layer), recvData));
		}break;
		default:
			//assert(false);
			break;
		}

	}
}

void TcpClient::loginRequest()
{
	if (m_loginId > 0)
		return;

	Packet::LoginRequest sendData;

	send((const char*)&sendData, sizeof(Packet::LoginRequest));
}

void TcpClient::attackRequest(int unitId, int attackRange, HexaDirection attackDircetion)
{
	Packet::AttackRequest sendData;

	AttackData attackData;
	attackData.id = unitId;
	attackData.Range = attackRange;
	attackData.direction = attackDircetion;

	sendData.mAttack = attackData;

	send((const char*)&sendData, sizeof(Packet::AttackRequest));
}