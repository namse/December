#pragma once
#include "Field.h"
#include "Unit.h"


class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void						InitGame(PlayerNumber player1, PlayerNumber player2);
	void						StartGame();

	std::vector<Unit*>			GetUnitList() { return m_UnitList; }
	Field*						GetField(){ return &m_GameField; }
	PlayerNumber				GetAttacker(){ return m_Attacker; }
	Unit*						GetUnit(UnitIdentityNumber id);
	Unit*						GetUnitInPosition(Coord position);
	int							GetPlayerIndexByPlayerNumber(PlayerNumber playerNumber);
	bool						IsPlayerInThisGame(PlayerNumber playerNumber);
	GameNumber					GetGameNumber(){ return m_GameNum; }

	void						HandleAttack(PlayerNumber attacker, AttackData attackData);
	void						HandleSkill(PlayerNumber attacker, SkillData skillData);
	void						GameOverForSurrender(PlayerNumber srrender);

private:
	UnitIdentityNumber			GenerateUnitIdentityNumber();
	void						SetUpNPC(UnitType unitType, Coord unitPos);
	HexaDirection				GetDirection(Coord point1, Coord point2);

	// For Attack Handling
	void						UnitPush(Unit* unit, int power , HexaDirection direction);
	void						UnitMove(Unit* unit, AttackData attackData);
	void						UnitApplyDamageWithCollision(Unit* thisGuy, Unit* thatGuy);
	void						PrintUnitActionQueue(UnitAction attackData);
	void						KillThisUnit(Unit* unit);

	bool						IsCorrectAttack(PlayerNumber attacker, AttackData attackData);
	void						IsNearArca();
	void						IsGameOver();
	void						UnitCounting();

	void						GameOver();


private:

	///# 아래 멤버 변수들  나중에 Init같은데서 초기화 하더라도, 디폴트 값으로 미리 생성자에서 초기화를 꼭 하삼...

	GameNumber					m_GameNum;
	Field						m_GameField;
	PlayerNumber				m_Attacker;
	int							m_CanCommand;	// 플레이어의 남은 명령 횟수
	bool						m_IsNearArca[2];	// 아르카스톤이 옆에 있는지에 대한 상태변수
	int							m_MaxTurn[2];
	int							m_PlayTurn;	// 경과한 턴
	bool						m_IsFirstTurn;
	bool						m_IsGameOver;
	int							m_UnitCount[3];	// 플레이어1, 플레이어2, NPC
	int							m_LivingUnitCount[3]; // 이 변수들은 사용하기 전에 UnitCounting() 해서 초기화해주세요~
	WhosWinner					m_Winner;

	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
	std::vector<UnitAction>		m_UnitActionQueue;

	UnitIdentityNumber			m_UnitIdentityNumberCounter;
};