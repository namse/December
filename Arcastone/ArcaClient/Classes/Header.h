#pragma once

#include "cocos2d.h"
#include "Enums.h"
#include "../../PacketType.h"
#include <vector>

#define LOGIN_IPADDR				"127.0.0.1"
#define DEBUG_PRINT_PACKET			false

#define MAP_IS_RECT					false
#define MAP_IS_HEXA					true
#define MAP_SIZEX					7
#define MAP_SIZEY					10

#define MAP_XSTART					400
#define MAP_YSTART					320
#define HEXAGON_LENGTH				30

#define DRAW_HEXA_NUMBER			true
#define DRAW_HEXA_POSITION			false

#define RADIANS_60					CC_DEGREES_TO_RADIANS(60)

#define MAX_PLAYER_HAVE_UNITS		10

typedef Packet::GameStartResult::UnitData UnitData;

using namespace std;
using namespace cocos2d;

class Header
{
public:
	Header();
	~Header();
};

#define NON_SELECT_UNIT INT_MAX

