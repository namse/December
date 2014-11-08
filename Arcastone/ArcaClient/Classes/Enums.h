#pragma once

#include "HexaPoint.h"
#include "ScreenPoint.h"

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

#define NON_SELECT_UNIT INT_MAX


enum PlayerWho
{
	PW_PLAYERONE = 0,
	PW_PLAYERTWO = 1,
};

struct Hexagon
{
	cocos2d::Point anchor;
	std::vector<cocos2d::Point> vertex;
};

class Enums
{
public:
	Enums();
	~Enums();
};

typedef Packet::GameStartResult::UnitData UnitData;

class ScreenPoint{
public:
	ScreenPoint(){}
	ScreenPoint(Coord point){
		x = point.x;
		y = point.y;
	}
	ScreenPoint(float _x, float _y){
		x = _x;
		y = _y;
	}
	Coord ScreenToCoord()
	{
		Coord retCoord;
		retCoord.x = x;
		retCoord.y = y;

		return retCoord;
	}

public:
	float x, y;
};

class HexaPoint{
public:
	HexaPoint(){}
	HexaPoint(Coord point){
		x = point.x;
		y = point.y;
	}
	HexaPoint(int _x, int _y){
		x = _x;
		y = _y;
	}
	ScreenPoint HexaToScreen()
	{
		ScreenPoint retPoint;

		// MAP_START 를 중앙에 위치하도록 그려주기 위한 수식들 .
		retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (x - (MAP_SIZEX - 1)*0.5);
		retPoint.y = MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (y * 2 - MAP_SIZEY + x - (MAP_SIZEX - 3)*0.5);

		return retPoint;
	}
	Coord HexaToCoord()
	{
		Coord retCoord;
		retCoord.x = x;
		retCoord.y = y;

		return retCoord;
	}

public:
	int x, y;
};
