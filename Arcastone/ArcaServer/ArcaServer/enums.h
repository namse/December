#pragma once
#include "../../PacketType.h"
#define PLAYER_UNIT_MAX 6
#define MAX_UNIT_TYPE 100

#define MAP_FIELD_WIDTH 7
#define MAP_FIELD_HEIGHT 11
#define MAP_FIELD_CENTER_WIDTH 3
#define MAP_FIELD_CENTER_HEIGHT 5

#define PLAYER_NUMBER_NPC INT_MAX
#define MAX_TURN	 2							// 기본적인 플레이어의 최대 턴 수
#define COLLET_GARBAGESESSIONS_TICK 1000		// 가비지콜렉팅하는 주기
#define USE_DRAW true	// 드로우(무승부) 있게 할거야?
#define USE_ARCA true

typedef int PlayerNumber;
typedef int GameNumber;


// enum FieldBlockType
// enum FieldBlockStatus
// enum UnitType
// enum UnitMoveType
// enum HexaDirection
// Above guys is in PacketType.h