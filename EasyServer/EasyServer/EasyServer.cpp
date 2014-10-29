#include "stdafx.h"
#include "EasyServer.h"

#include "Config.h"
#include "..\..\PacketType.h"

#include "ThreadLocal.h"
#include "Exception.h"
#include "Scheduler.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

#pragma comment(lib,"ws2_32.lib")

typedef ProducerConsumerQueue<SOCKET, 100> PendingAcceptList;

int _tmain(int argc, _TCHAR* argv[])
{
	/// crash �߻��� dump ����� ���ؼ�
	SetUnhandledExceptionFilter(ExceptionFilter) ;

	LThreadType = THREAD_MAIN ;

	/// Manager Init
	GClientManager = new ClientManager ;
	GDatabaseJobManager = new DatabaseJobManager ;

	/// DB Helper �ʱ�ȭ
	if ( false == DbHelper::Initialize(DB_CONN_STR) )
		return -1 ;

	/// ���� �ʱ�ȭ
	WSADATA wsa ;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1 ;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0) ;
	if (listenSocket == INVALID_SOCKET)
		return -1 ;

	int opt = 1 ;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int) ) ;

	/// bind
	SOCKADDR_IN serveraddr ;
	ZeroMemory(&serveraddr, sizeof(serveraddr)) ;
	serveraddr.sin_family = AF_INET ;
	serveraddr.sin_port = htons(LISTEN_PORT) ;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
	int ret = bind(listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) ;
	if (ret == SOCKET_ERROR)
		return -1 ;
	
	/// listen
	ret = listen(listenSocket, SOMAXCONN) ;
	if (ret == SOCKET_ERROR)
		return -1 ;

	/// accepting list
	PendingAcceptList pendingAcceptList;

	/// Client Logic + I/O Thread
	DWORD dwThreadId ;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ClientHandlingThread, (LPVOID)&pendingAcceptList, 0, (unsigned int*)&dwThreadId);
    if (hThread == NULL)
		return -1 ;

	/// DB Thread
	HANDLE hDbThread = (HANDLE)_beginthreadex (NULL, 0, DatabaseHandlingThread, NULL, 0, (unsigned int*)&dwThreadId) ;
	if (hDbThread == NULL)
		return -1 ;

	printf("EasyGameServer Started...\n");

	/// accept loop
	while ( true )
	{
		SOCKET acceptedSocket = accept(listenSocket, NULL, NULL) ;
		if (acceptedSocket == INVALID_SOCKET)
		{
			printf("accept: invalid socket\n") ;
			continue ;
		}

		pendingAcceptList.Produce(acceptedSocket);
	}

	CloseHandle( hThread ) ;
	CloseHandle( hDbThread ) ;

	// ���� ����
	WSACleanup() ;

	DbHelper::Finalize() ;

	delete GClientManager ;
	delete GDatabaseJobManager ;
	return 0 ;
}

unsigned int WINAPI ClientHandlingThread( LPVOID lpParam )
{
	LThreadType = THREAD_CLIENT ;
	LScheduler = new Scheduler;

	PendingAcceptList* pAcceptList = (PendingAcceptList*)lpParam ;

	/// Timer
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL) ;
	if (hTimer == NULL)
		return -1 ;

	LARGE_INTEGER liDueTime ;
	liDueTime.QuadPart = -10000000 ; ///< 1�� �ĺ��� ����
	if (!SetWaitableTimer(hTimer, &liDueTime, APC_TIMER_INTERVAL, TimerProc, NULL, TRUE))
		return -1 ;
		
	while ( true )
	{
		SOCKET acceptSock = NULL;

		/// ���� ������ Ŭ���̾�Ʈ ó��
		if (pAcceptList->Consume(acceptSock, false))
		{
			/// ���� ���� ����ü �Ҵ�� �ʱ�ȭ
			ClientSession* client = GClientManager->CreateClient(acceptSock);

			SOCKADDR_IN clientaddr;
			int addrlen = sizeof(clientaddr);
			getpeername(acceptSock, (SOCKADDR*)&clientaddr, &addrlen);

			// Ŭ�� ���� ó��
			if (false == client->OnConnect(&clientaddr))
			{
				client->Disconnect();
			}

			continue; ///< �ٽ� ����
		}

		/// ���������� Ŭ���̾�Ʈ�鿡 ���� send ��û ó��
		GClientManager->FlushClientSend();

		/// APC Queue�� ���� �۾��� ó��
		SleepEx(INFINITE, TRUE);
	}

	CloseHandle( hTimer ) ;
	return 0;
} 

unsigned int WINAPI DatabaseHandlingThread( LPVOID lpParam )
{
	LThreadType = THREAD_DATABASE ;

	GDatabaseJobManager->ExecuteDatabaseJobs() ;
	
	return 0 ;
}

void CALLBACK TimerProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	assert( LThreadType == THREAD_CLIENT ) ;

	LScheduler->DoTasks(); ///< �ֱ������� task ó��
	
	GClientManager->OnPeriodWork() ;

}
