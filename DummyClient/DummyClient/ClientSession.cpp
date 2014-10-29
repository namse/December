#include "stdafx.h"
#include "ClientSession.h"



void ClientSession::HandleReadPayload(const boost::system::error_code& error)
{
	if (error)
	{
		std::cout << error.message() << std::endl;
		return;
	}

	int pktLen = static_cast<int>(mReadBuf[1]) << 8 | mReadBuf[0];
	int pktType = static_cast<int>(mReadBuf[3]) << 8 | mReadBuf[2];

	switch (pktType)
	{
	case PKT_SC_LOGIN:
		{
			LoginResult pktResult;
			memcpy_s(&pktResult, pktLen, (void*)&mReadBuf, pktLen);

			mLogon = true;

			printf_s("LOGIN OK: ID[%d] Name[%s] POS[%.4f, %.4f]\n", pktResult.mPlayerId, pktResult.mName, pktResult.mPosX, pktResult.mPosY);
			
			/// ������ ��ٸ� �Ŀ� Ÿ�̸� ������
			DoTimer(rand() % 1000, &ClientSession::OnTick);
		}

		break;

	case PKT_SC_CHAT:
		{
			ChatBroadcastResult pktResult;
			memcpy_s(&pktResult, pktLen, (void*)&mReadBuf, pktLen);
			printf_s("CHAT from [%s]: %s\n", pktResult.mName, pktResult.mChat);
		}
		break;

	case PKT_SC_MOVE:
		{
			MoveBroadcastResult pktResult;
			memcpy_s(&pktResult, pktLen, (void*)&mReadBuf, pktLen);

			/// �ڽ��� ��츸 ������Ʈ
			if (mPlayerId == pktResult.mPlayerId)
			{
				mPosX = pktResult.mPosX;
				mPosY = pktResult.mPosY;

				printf_s(" Player [%d] move to (%.2f, %.2f) \n", pktResult.mPlayerId, pktResult.mPosX, pktResult.mPosY);
			}
			
		}
		break;

	}

	PostReadHeader();
}

void ClientSession::OnTick()
{
	if (!mLogon)
		return;

	
	/// �ֱ������� �̵� �Ǵ� ä�� ����
	if (rand() % 5 == 0)
	{
		/// 20% Ȯ���� ä��
		ChatBroadcastRequest sendPkt;
		sendPkt.mPlayerId = mPlayerId;
		sprintf_s(sendPkt.mChat, " TEST CHAT: HELLO~~ This is Player %d =====", mPlayerId);
		SendPacket(sendPkt);
	}
	else
	{
		/// 80% Ȯ���� random walking
		MoveRequest sendPkt;
		sendPkt.mPlayerId = mPlayerId;
		sendPkt.mPosX = mPosX + rand() % 100 - 50;
		sendPkt.mPosY = mPosY + rand() % 60 - 30;
		SendPacket(sendPkt);
	}

	/// 1�ʸ��� �ݺ�
	DoTimer(1000, &ClientSession::OnTick);
}