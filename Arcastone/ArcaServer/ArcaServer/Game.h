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
	std::vector<Unit*>	GetUnitList() { return m_UnitList; }
	Field*			GetField(){ return &m_GameField; }
	PlayerNumber	GetAttacker(){ return m_Attacker; }
	Unit*	GetUnit(UnitIdentityNumber id) {
		for (auto unit : m_UnitList)
			if (unit->GetID() == id)
				return unit;
		return nullptr;
	}


	void			HandleAttack(PlayerNumber attacker, AttackData attackData);

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
	UnitIdentityNumber GenerateUnitIdentityNumber() {
		return m_UnitIdentityNumberCounter++;
	}
	void IsArca();
	void IsGameOver();
	void SetUpArca();
	void UnitCounting();

	bool IsCorrectAttack(AttackData attackData);

private:
	GameNumber		m_GameNum;
	Field			m_GameField;
	PlayerNumber	m_Attacker;
	int				m_CanCommand;	// 플레이어의 남은 명령 횟수
	bool			m_IsNearArca[2];	// 아르카스톤이 옆에 있는지에 대한 상태변수
	int				m_MaxTurn[2];
	int				m_PlayTurn;	// 경과한 턴
	bool			m_IsFirstTurn;
	bool			m_IsGameOver;
	int				m_UnitCount[3];	// 플레이어1, 플레이어2, NPC
	int				m_LivingUnitCount[3]; // 이 변수들은 사용하기 전에 UnitCounting() 해서 초기화해주세요~
	WhosWinner		m_Winner;

	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
	std::vector<UnitAction>	m_UnitActionQueue;

	UnitIdentityNumber	m_UnitIdentityNumberCounter;
};