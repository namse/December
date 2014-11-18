#pragma once
#include "Game.h"
#include "Player.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	GameNumber		CreateGame(PlayerNumber player1, PlayerNumber player2);
	Game*			GetGame(GameNumber GameID){ return m_GameList.find(GameID)->second;  } ///# 웬만하면 구현 분리해라.
	Game*			GetGameWithPlayerNumber(PlayerNumber playerNumber); ///# 이거 Game* 리턴할 때, Game*이 사라진 경우에 대한 대비도 된거지?
	bool			CloseGameByPlayerNumber(PlayerNumber playerNumber);

private:
	typedef std::map<GameNumber, Game*> GameList;
	GameList		m_GameList;
};

extern GameManager* GGameManager;