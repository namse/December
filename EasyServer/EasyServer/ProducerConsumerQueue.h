#pragma once

template<typename TElem, int QSize>
class ProducerConsumerQueue
{
public:
	ProducerConsumerQueue() : mOccupiedSize(0), mQueueOffset(0)
	{
		memset(mQueueArray, 0, sizeof(mQueueArray));
		InitializeConditionVariable(&mNotFull);
		InitializeConditionVariable(&mNotEmpty);
		InitializeSRWLock(&mSRWLock);
	}

	~ProducerConsumerQueue() {}

	bool Produce(const TElem& item, bool waitUntilConsume=true)
	{
		AcquireSRWLockExclusive(&mSRWLock);
		
		while (mOccupiedSize == QSize)
		{
			if (waitUntilConsume)
			{
				/// ť�� ���� ���� ���涧���� �ܴ�.
				SleepConditionVariableSRW(&mNotFull, &mSRWLock, INFINITE, 0);
			}
			else
			{
				ReleaseSRWLockExclusive(&mSRWLock);
				return false;
			}
		}
			
		
		mQueueArray[(mQueueOffset + mOccupiedSize) % QSize] = item;
		++mOccupiedSize;

		ReleaseSRWLockExclusive(&mSRWLock);

		WakeConditionVariable(&mNotEmpty);

		return true;
	}

	
	bool Consume(TElem& item, bool waitUntilProduce=true)
	{
		AcquireSRWLockExclusive(&mSRWLock);

		while (mOccupiedSize == 0)
		{
			if (waitUntilProduce)
			{
				/// ť�� ������ ���ö����� �ܴ�
				SleepConditionVariableSRW(&mNotEmpty, &mSRWLock, INFINITE, 0);
			}
			else
			{
				ReleaseSRWLockExclusive(&mSRWLock);
				return false;
			}
			
		}

		item = mQueueArray[mQueueOffset];
		--mOccupiedSize;

		if (++mQueueOffset == QSize)
			mQueueOffset = 0;

		ReleaseSRWLockExclusive(&mSRWLock);

		WakeConditionVariable(&mNotFull);

		return true;
	}

private:

	TElem mQueueArray[QSize];

	uint32_t mOccupiedSize;
	uint32_t mQueueOffset;

	CONDITION_VARIABLE mNotFull;
	CONDITION_VARIABLE mNotEmpty;

	SRWLOCK mSRWLock;

};


