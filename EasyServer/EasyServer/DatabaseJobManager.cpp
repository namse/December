#include "stdafx.h"
#include "EasyServer.h"
#include "ThreadLocal.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

DatabaseJobManager* GDatabaseJobManager = nullptr ;


/// �Ʒ� �Լ��� DB ó�� �����忡�� �ҷ��� �Ѵ�
void DatabaseJobManager::ExecuteDatabaseJobs()
{
	assert( LThreadType == THREAD_DATABASE ) ;

	DatabaseJobContext* jobContext = nullptr ;
	while ( true )
	{
		mDbJobRequestQueue.Consume(jobContext) ;

		/// ���⼭ DBȣ���ؼ� ó���ϰ� 
		jobContext->mSuccess = jobContext->OnExecute() ;

		/// �� ����� result queue�� ��� ����
		mDbJobResultQueue.Produce(jobContext) ;
	}
}

/// �Ʒ� �Լ��� Ŭ���̾�Ʈ ó�� �����忡�� �ҷ��� �Ѵ�
void DatabaseJobManager::PushDatabaseJobRequest(DatabaseJobContext* jobContext)
{
	assert( LThreadType == THREAD_CLIENT ) ;
	mDbJobRequestQueue.Produce(jobContext) ;
}

/// �Ʒ� �Լ��� Ŭ���̾�Ʈ ó�� �����忡�� �ҷ��� �Ѵ�
bool DatabaseJobManager::PopDatabaseJobResult(DatabaseJobContext*& jobContext)
{
	assert( LThreadType == THREAD_CLIENT ) ;

	/// DB �۾� �Ϸ�� ��ٸ��� �ʴ´�
	return mDbJobResultQueue.Consume(jobContext, false) ;
}