#pragma once
#include "Field.h"
#include "Unit.h"


class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void			InitGame(PlayerNumber player1, PlayerNumber player2);
	void			StartGame();

	int				GetUnit(OUT Unit* unitArr[]);
	PlayerNumber	GetAttacker(){ return m_Attacker; }


	void			HandleAtttack(PlayerNumber attacker, AttackData attackData);

	bool		IsPlayerInThisGame(PlayerNumber playerNumber) {
		for (auto playerNumber_ : m_PlayerList)
			if (playerNumber == playerNumber_)
				return true;
		return false;
	}


private:
	// For Attack Handling
	void UnitMove(HexaDirection direction, int range, Unit* unit, bool isFirstMove);
	Unit* GetUnitInPosition(Coord position){
		for (auto unit : m_UnitList)
			if (unit->GetPos() == position)
				return unit;
		return nullptr;
	}
	void KillThisUnit(Unit* unit);
	void UnitPush(Unit* pusher, Unit* target, int power, bool isFirstPush);
	void UnitApplyDamageWithCollision(Unit* thisGuy, Unit* thatGuy);

private:
	GameNumber		m_GameNum;
	Field			m_GameField;
	PlayerNumber	m_Attacker;
	int				m_PlayTurn;
	bool			m_IsFirstTurn;
	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
};