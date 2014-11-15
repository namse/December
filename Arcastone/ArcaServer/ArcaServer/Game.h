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
	void						GameOverForSurrender(PlayerNumber srrender);

private:
	UnitIdentityNumber			GenerateUnitIdentityNumber();
	void						SetUpNPC(UnitType unitType, Coord unitPos);

	// For Attack Handling
	void						UnitPush(Unit* pusher, Unit* target, int power, bool isFirstPush);
	void						UnitMove(HexaDirection direction, int range, Unit* unit, bool isFirstMove);
	void						UnitJump(HexaDirection direction, int range, Unit* unit);
	void						UnitApplyDamageWithCollision(Unit* thisGuy, Unit* thatGuy);
	void						PrintUnitActionQueue(UnitAction attackData);
	void						KillThisUnit(Unit* unit);

	bool						IsCorrectAttack(PlayerNumber attacker, AttackData attackData);
	void						IsNearArca();
	void						IsGameOver();
	void						UnitCounting();

	void						GameOver();


private:
	GameNumber					m_GameNum;
	Field						m_GameField;
	PlayerNumber				m_Attacker;
	int							m_CanCommand;	// �÷��̾��� ���� ��� Ƚ��
	bool						m_IsNearArca[2];	// �Ƹ�ī������ ���� �ִ����� ���� ���º���
	int							m_MaxTurn[2];
	int							m_PlayTurn;	// ����� ��
	bool						m_IsFirstTurn;
	bool						m_IsGameOver;
	int							m_UnitCount[3];	// �÷��̾�1, �÷��̾�2, NPC
	int							m_LivingUnitCount[3]; // �� �������� ����ϱ� ���� UnitCounting() �ؼ� �ʱ�ȭ���ּ���~
	WhosWinner					m_Winner;

	std::vector<Unit*>			m_UnitList;
	std::vector<PlayerNumber>	m_PlayerList;
	std::vector<UnitAction>		m_UnitActionQueue;

	UnitIdentityNumber			m_UnitIdentityNumberCounter;
};