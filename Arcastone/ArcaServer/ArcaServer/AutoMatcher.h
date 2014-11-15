#pragma once
class AutoMatcher
{
public:
	AutoMatcher();
	~AutoMatcher();
	
	void						AddWaitPlayer(PlayerNumber playerId);
	bool						DeleteWaitPlayer(PlayerNumber plyaerId);

private:
	std::list <PlayerNumber>	m_WaitPlayerList;

};

extern AutoMatcher* GAutoMatcher;