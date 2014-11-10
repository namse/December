#pragma once
#include "../../PacketType.h"
#define PLAYER_UNIT_MAX 6
#define MAX_UNIT_TYPE 100

#define MAP_FIELD_WIDTH 7
#define MAP_FIELD_HEIGHT 11
#define MAP_FIELD_CENTER_WIDTH 3
#define MAP_FIELD_CENTER_HEIGHT 5

#define PLAYER_NUMBER_NPC INT_MAX
#define MAX_TURN	 2							// �⺻���� �÷��̾��� �ִ� �� ��
#define COLLET_GARBAGESESSIONS_TICK 1000		// �������ݷ����ϴ� �ֱ�
#define USE_DRAW true	// ��ο�(���º�) �ְ� �Ұž�?
#define USE_ARCA true

typedef int PlayerNumber;
typedef int GameNumber;


// enum FieldBlockType
// enum FieldBlockStatus
// enum UnitType
// enum UnitMoveType
// enum HexaDirection
// Above guys is in PacketType.h