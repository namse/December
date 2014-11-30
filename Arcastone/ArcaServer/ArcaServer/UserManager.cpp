#include "stdafx.h"
#include "UserManager.h"
#include "AutoMatcher.h"
#include "GameManager.h"


UserManager* GUserManager = nullptr;

UserManager::UserManager()
{
}


UserManager::~UserManager()
{
	for (auto userIt : m_UserList)
	{
		delete userIt.second;
	}

	m_UserList.clear();
}

UserNumber UserManager::CreateUser()
{
	static UserNumber userNum = 0;
	userNum++;
	auto newUser = new User(userNum);
	m_UserList.insert(UserList::value_type(userNum, newUser));
	
	return userNum;
}

User* UserManager::GetUser(UserNumber userNum){
	auto userIt = m_UserList.find(userNum);
	if (userIt == m_UserList.end())
		return nullptr;
	return userIt->second;
}


void UserManager::DeleteUser(UserNumber userNum)
{
	auto userIt = m_UserList.find(userNum);
	if (userIt == m_UserList.end())
		return;

	GAutoMatcher->DeleteWaitUser(userNum);
	GGameManager->CloseGameByUserNumber(userNum);

	delete userIt->second;
	m_UserList.erase(userIt);
}