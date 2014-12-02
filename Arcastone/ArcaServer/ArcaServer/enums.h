#pragma once
#include "../../PacketType.h"

/* In Game Value  */
#define USER_UNIT_MAX					10
#define USE_ARCA						true
#define USE_ROCK						true
#define USE_SKILL						false
#define USE_ARCA_HIT_COST				false

#define RANDOM_ATTACKER					false
#define MAX_TURN						3		// 기본적인 플레이어의 최대 턴 수
#define BREAK_DOWN_TURN					10
#define BREAK_BLOCK						1
#define USE_DRAW						true

#define MAP_FIELD_WIDTH					7
#define MAP_FIELD_HEIGHT				11

/*  Out Game Value  */
#define COLLET_GARBAGESESSIONS_TICK		1000	// 가비지콜렉팅하는 주기

#define PLAYER_COUNT					2
#define NON_PLAYER_COUNT				1
#define PLAYER_COUNT_ALL				PLAYER_COUNT + NON_PLAYER_COUNT

#define USER_NUMBER_NPC					INT_MAX
#define PLAYER_NUMBER_NPC				2

#define UNNOWN_ERROR					true

typedef int UserNumber;
typedef int GameNumber;


enum PlayerNumber
{
	PLAYER_ONE = 0,
	PLAYER_TWO = 1,
	PLAYER_NPC = 2,
};