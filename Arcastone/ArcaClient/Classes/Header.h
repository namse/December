#pragma once

#include "cocos2d.h"
#include "Enums.h"
#include <vector>

#define MAP_SCALEX					9
#define MAP_SCALEY					4
#define MAP_XSTART					50
#define MAP_YSTART					50
#define HEXAGON_LENGTH				45

#define RADIANS_60					1.0472f		// 60  * 3.14~ / 180

#define MAX_PLAYER_HAVE_UNITS		10

using namespace std;
using namespace cocos2d;

class Header
{
public:
	Header();
	~Header();
};

