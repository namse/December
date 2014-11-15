#pragma once
class AutoMatcher
{
public:
	AutoMatcher();
	~AutoMatcher();
	
	void						AddWaitPlayer(PlayerNumber playerId);
	bool						DeleteWaitPlayer(PlayerNumber plyaerId);

private:
	std::vector <PlayerNumber>	m_WaitPlayerList;

};

extern AutoMatcher* GAutoMatcher;