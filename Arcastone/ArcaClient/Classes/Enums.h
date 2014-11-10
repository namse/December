#pragma once

#include "HexaPoint.h"
#include "ScreenPoint.h"

// define

#define LOGIN_IPADDR				"127.0.0.1"
#define DEBUG_PRINT_PACKET			false

#define DISPLAY_SIZEX				480
#define DISPLAY_SIZEY				690

#define MAP_SIZEX					7
#define MAP_SIZEY					11

#define MAP_XSTART					DISPLAY_SIZEX/2
#define MAP_YSTART					DISPLAY_SIZEY/2
#define HEXAGON_LENGTH				35

#define PIXEL_TO_RANGE_MULT			0.03

#define DRAW_HEXA_NUMBER			true
#define DRAW_HEXA_POSITION			false

#define RADIANS_60					CC_DEGREES_TO_RADIANS(60)

#define MAX_PLAYER_HAVE_UNITS		10

#define COLOR_OF_GRID				ccc4f(0.0f, 0.6f, 1.0f, 0.5f)

#define COLOR_OF_PLAYER				ccc4f(0.0f, 0.6f, 1.0f, 0.3f)
#define COLOR_OF_ENEMY				ccc4f(1.0f, 0.0f, 0.2f, 0.5f)
#define COLOR_OF_EXPECT				ccc4f(0.0f, 0.6f, 1.0f, 0.2f)

#define NON_SELECT_UNIT INT_MAX


// enum
enum PlayerWho
{
	PW_PLAYERONE = 0,
	PW_PLAYERTWO = 1,
};

enum UnitState
{
	US_NONE = 0,

	US_NORMAL = 1,
	US_DIE = 2,
};

// typedef 
typedef Packet::GameStartResult::UnitData UnitData;

// struct

struct Hexagon
{
	cocos2d::Point anchor;
	std::vector<cocos2d::Point> vertex;
};

struct ScreenPoint : public cocos2d::Vec2
{
	ScreenPoint(){}
	ScreenPoint(Coord point){
		x = point.x;
		y = point.y;
	}
	ScreenPoint(Vec2 point){
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

	HexaDirection getDirection()
	{
		float xx = x;
		float yy = y;
		float degree = CC_RADIANS_TO_DEGREES(atan2(yy, xx));

		// 3,4분면과 270도에서 직교좌표계 보정
		if ((xx < 0 && yy < 0) || (xx >= 0 && yy < 0))
			degree += 360;

		// 헥사그리드 좌표로 변환
		HexaDirection Direction = (HexaDirection)((int)(degree / 60) + 1);

		return Direction;
	}

	int getRange()
	{
		return getLength() * PIXEL_TO_RANGE_MULT;
	}
};

struct HexaPoint : public cocos2d::Vec2
{
	HexaPoint(){}
	HexaPoint(Coord point){
		x = point.x;
		y = point.y;
	}
	HexaPoint(Vec2 point){
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

	HexaDirection getDirection(HexaPoint point)
	{
		HexaPoint vec;
		vec.x = point.x - x;
		vec.y = point.y - y;

		if (vec.y < 0 && vec.x == 0)
		{
			return HD_NORTH;
		}
		else if (vec.x > 0 && vec.y < 0)
		{
			if (abs(vec.x) != abs(vec.y)) return HD_NONE;
			return HD_NORTHEAST;
		}
		else if (vec.x < 0 && vec.y == 0)
		{
			return HD_NORTHWEST;
		}
		else if (vec.y > 0 && vec.x == 0)
		{
			return HD_SOUTH;
		}
		else if (vec.x > 0 && vec.y == 0)
		{
			return HD_SOUTHEAST;
		}
		else if (vec.x < 0 && vec.y > 0)
		{
			if (abs(vec.x) != abs(vec.y)) return HD_NONE;
			return HD_SOUTHWEST;
		}

		return HD_NONE;
	}

	bool isAround(HexaPoint point, int range)
	{
		HexaPoint vec;
		vec.x = point.x - x;
		vec.y = point.y - y;

		if (abs(vec.x) > range || abs(vec.y) > range || abs(vec.x + vec.y) > range)
		{
			return false;
		}
		return true;
	}

	HexaPoint getMovePoint(HexaDirection direction, int range)
	{
		HexaPoint retPoint;
		retPoint.x = x;
		retPoint.y = y;

		switch (direction)
		{
		case HD_NORTH:
		{
			retPoint.y -= range;
		}break;
		case HD_NORTHEAST:
		{
			retPoint.x += range;
			retPoint.y -= range;
		}break;
		case HD_NORTHWEST:
		{
			retPoint.x -= range;
		}break;
		case HD_SOUTH:
		{
			retPoint.y += range;
		}break;
		case HD_SOUTHEAST:
		{
			retPoint.x += range;
		}break;
		case HD_SOUTHWEST:
		{
			retPoint.y += range;
			retPoint.x -= range;
		}break;
		}

		return retPoint;
	}

};
