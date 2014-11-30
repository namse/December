#pragma once
class AutoMatcher
{
public:
	AutoMatcher();
	~AutoMatcher();
	
	void						AddWaitUser(UserNumber userId);
	bool						DeleteWaitUser(UserNumber plyaerId);

private:
	std::list <UserNumber>	m_WaitUserList;

};

extern AutoMatcher* GAutoMatcher;