#pragma once
class GameManager
{
private:
	GameManager();
	~GameManager();
	static GameManager* pInstance;

public:
	static GameManager* GetInstance()
	{
		if (nullptr == pInstance)
		{
			pInstance = new GameManager();
		}
		return pInstance;
	}

	void FreeInstance()
	{
		if (nullptr != pInstance)
		{
			delete pInstance;
			pInstance = nullptr;
		}
		return;
	}
};

