#pragma once
#include "User.h"

class UserManager
{
public:
	UserManager();
	~UserManager();

	UserNumber		CreateUser();
	void				DeleteUser(UserNumber userNum);
	User*				GetUser(UserNumber userNum);

private:
	typedef				std::map<UserNumber, User*> UserList;
	UserList			m_UserList;
};

extern UserManager* GUserManager;
