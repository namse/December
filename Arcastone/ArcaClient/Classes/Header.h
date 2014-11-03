#pragma once

#include "cocos2d.h"
#include "Enums.h"
#include "../../PacketType.h"
#include <vector>

#define MAP_IS_RECT					false
#define MAP_IS_HEXA					true
#define MAP_SIZEX					7
#define MAP_SIZEY					10
/*
#define MAP_SIZEY					12
#define MAP_SIZEY					10 + 2
#define MAP_SIZEY					10 + (MAP_IS_RECT)? MAP_SIZEX*0.5 : 0
#define MAP_SIZEY					(int)10 + (MAP_IS_RECT)? MAP_SIZEX*0.5 : 0
#define MAP_SIZEY					(int)(10 + (MAP_IS_RECT)? MAP_SIZEX*0.5 : 0)
각자 다양한 버그를 일으키는데 이유를 모르겠음. 보류
*/

#define MAP_XSTART					300
#define MAP_YSTART					320
#define HEXAGON_LENGTH				25

#define DRAW_HEXA_NUMBER			true

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

