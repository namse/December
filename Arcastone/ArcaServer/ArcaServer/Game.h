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

	Player* GetAttacker();
	void TossTurn();

	/*  in line functions  */
	GameNumber					GetGameNumber(){ return m_GameNum; }
	UserNumber					GetUserNumberByPlayerNumber(PlayerNumber playerNumber);
	Player*						GetPlayerByUserName(UserNumber userNumber);
	Player*						GetPlayerList(){ return m_Player; }
	Field*						GetField(){ return &m_Field; }
	TurnManager*				GetTurnManager(){ return &m_Turnmanager; }
	std::vector<Unit*>*			GetUnitList(){ return &m_AllUnit; }

	/*  for unit management  */
	Unit*						GetUnit(UnitIdentityNumber id);
	Unit*						GetUnitInPosition(Coord position);
	bool						IsUserInThisGame(UserNumber userNumber);
	UnitIdentityNumber			GenerateUnitIdentityNumber();
	void						UnitCounting();

	/*  Push Action Queue  */
	void						SetActionQueue(UnitAction* action){ m_UnitActionQueue.push_back(*action); }
	void						HandleAction(UserNumber user, ActionData* actionData);
	bool						IsCorrectAction(UserNumber user, ActionData* actionData);
	Skill*						InitSkill(UnitSkillType type);
	void						PrintUnitActionQueue(UnitAction actionData);
	void						OperatingUnitAction(UserNumber user, ActionData* actionData);

	/*  In Game Event  */
	void						StartBreakDown();
	void						NearArcaCheck();
	Player*						AttackerSwap();
	void						SetUpNPC(UnitType unitType, Coord unitPos);

	void						IsGameOver();
	void						GameOver();
	void						GameOverForSurrender(UserNumber srrender);

	/*  Send Packet  */
	void						SendActionQueue();
	void						SendCurrendtCost();
	void						SendWhosTurn();

private:
	GameNumber					m_GameNum;

	UserNumber					m_User[2];
	Player						m_Player[PLAYER_COUNT_ALL];
	bool						m_IsGameOver;
	WhosWinner					m_Winner;

	UnitIdentityNumber			m_UnitIdentityNumberCounter;
	std::vector<Unit*>			m_AllUnit;
	int							m_UnitCount[PLAYER_COUNT_ALL];
	int							m_LivingUnitCount[PLAYER_COUNT_ALL];

	std::vector<UnitAction>		m_UnitActionQueue;

	Field						m_Field;
	TurnManager					m_Turnmanager;
};