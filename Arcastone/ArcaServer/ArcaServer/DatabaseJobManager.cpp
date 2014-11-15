﻿#include "stdafx.h"
#include "ArcaServer.h"
#include "ThreadLocal.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

DatabaseJobManager* GDatabaseJobManager = nullptr ;


/// 아래 함수는 DB 처리 쓰레드에서 불려야 한다
void DatabaseJobManager::ExecuteDatabaseJobs()
{
	assert( LThreadType == THREAD_DATABASE ) ;

	DatabaseJobContext* jobContext = nullptr ;
	while ( true )
	{
		mDbJobRequestQueue.Consume(jobContext) ;

		/// 여기서 DB호출해서 처리하고 
		jobContext->mSuccess = jobContext->OnExecute() ;

		/// 그 결과를 result queue에 담아 놓음
		mDbJobResultQueue.Produce(jobContext) ;
	}
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
void DatabaseJobManager::PushDatabaseJobRequest(DatabaseJobContext* jobContext)
{
	assert( LThreadType == THREAD_CLIENT ) ;
	mDbJobRequestQueue.Produce(jobContext) ;
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
bool DatabaseJobManager::PopDatabaseJobResult(DatabaseJobContext*& jobContext)
{
	assert( LThreadType == THREAD_CLIENT ) ;

	/// DB 작업 완료는 기다리지 않는다
	return mDbJobResultQueue.Consume(jobContext, false) ;
}