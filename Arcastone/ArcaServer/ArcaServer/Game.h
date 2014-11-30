#pragma once

#include "Skill.h"
#include "Player.h"
#include "Field.h"
#include "Unit.h"
#include "TurnManager.h"

class Game
{
public:
	Game(GameNumber gameNum);
	~Game();

	void						InitGame(UserNumber user1, UserNumber user2);
	void						StartGame();

	Field*						GetField(){ return &m_Field; }
	Unit*						GetUnit(UnitIdentityNumber id);
	Unit*						GetUnitInPosition(Coord position);
	bool						IsUserInThisGame(UserNumber userNumber);
	GameNumber					GetGameNumber(){ return m_GameNum; }

	void						HandleAttack(UserNumber user, AttackData* attackData);
	void						GameOverForSurrender(UserNumber srrender);

	std::vector<Unit*>*			GetUnitList(){ return &m_AllUnit; }
	Player*						GetPlayerList(){ return m_Player; }



	void						UnitFall();
	void						StartBreakDown();

	void						SetUpNPC(UnitType unitType, Coord unitPos);

	TurnManager*				GetTurnManager(){ return &m_Turnmanager; }

	int*						GetCurrentCost(){ return &m_CurrentCost; }

	void						SetActionQueue(UnitAction* action){ m_UnitActionQueue.push_back(*action); }
	void						PrintUnitActionQueue(UnitAction attackData);

	void						UnitCounting();

	Player*						AttackerSwap();


private:
	UnitIdentityNumber			GenerateUnitIdentityNumber();

//	void						UnitApplyDamageWithCollision(Unit* thisGuy, Unit* thatGuy);

	bool						IsCorrectAttack(UserNumber user, AttackData* attackData);

	bool						IsInValidPlayerNumber(PlayerNumber playerNumber);

	void						IsGameOver();
	void						GameOver();


	Player*						GetPlayerByUserName(UserNumber userNumber);

	Skill*						SetupSkill(UnitSkillType type);

	void						SendActionQueue();
	void						SendCurrendtCost();
	void						SendWhosTurn();

	void						NearArcaCheck();


private:
	TurnManager					m_Turnmanager;

	GameNumber					m_GameNum;
	Field						m_Field;

	UserNumber					m_User[2];
	Player						m_Player[PLAYER_COUNT_ALL];

	std::vector<Unit*>			m_AllUnit;

	int							m_CurrentCost;		// 현재 턴인 플레이어의 남은 코스트

	bool						m_IsGameOver;

	int							m_UnitCount[PLAYER_COUNT_ALL];
	int							m_LivingUnitCount[PLAYER_COUNT_ALL]; // 이 변수들은 사용하기 전에 UnitCounting() 해서 초기화해주세요~
	WhosWinner					m_Winner;

	std::vector<UnitAction>		m_UnitActionQueue;

	UnitIdentityNumber			m_UnitIdentityNumberCounter;
};