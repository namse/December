#pragma once
class AutoMatcher
{
public:
	AutoMatcher();
	~AutoMatcher();
	
	void		AddWaitPlayer(PlayerNumber playerId);

private:
	std::queue <PlayerNumber>	m_WaitPlayerList;

};

extern AutoMatcher* GAutoMatcher;