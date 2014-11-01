#pragma once

#include "cocos2d.h"
#include "Enums.h"
#include <vector>

#define MAP_SIZEX					9
#define MAP_SIZEY					10
#define MAP_XSTART					50
#define MAP_YSTART					400
#define HEXAGON_LENGTH				30

#define MAP_IS_RECT					false
#define MAP_IS_HEXA					true
#define DRAW_HEXA_NUMBER			true

#define RADIANS_60					CC_DEGREES_TO_RADIANS(60)

#define MAX_PLAYER_HAVE_UNITS		10

using namespace std;
using namespace cocos2d;

class Header
{
public:
	Header();
	~Header();
};

