#pragma once

class Game
{
public:
	Game(GameNumber gameId);
	~Game();

	void InitGame(PlayerNumber player1, PlayerNumber player2);

private:
	GameNumber m_GameID;
	std::vector<PlayerNumber> m_PlayerList;
};

