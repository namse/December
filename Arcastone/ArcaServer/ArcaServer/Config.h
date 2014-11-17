#pragma once


/// DB 연결 스트링 
#define LISTEN_PORT	9001

/// 타이머 실행 간격 (ms)
#define APC_TIMER_INTERVAL	100

/// 세션별 send/recv 버퍼 크기
#define BUFSIZE	(1024*10)