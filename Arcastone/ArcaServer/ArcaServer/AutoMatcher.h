#pragma once
class AutoMatcher
{
public:
	AutoMatcher();
	~AutoMatcher();
	
	void AddWaitPlayer(PlayerNumber playerId);

private:
	std::vector <PlayerNumber> m_WaitPlayerList;

};

extern AutoMatcher* GAutoMatcher;