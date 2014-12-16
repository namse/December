#pragma once

#include "HexaPoint.h"
#include "ScreenPoint.h"

#define LOGIN_IPADDR				"127.0.0.1"

#define NON_SELECT_UNIT INT_MAX
#define PACKET_MAX_SIZE				1024

/*  Debug  */
#define OPEN_DEBUG_CONSOLE			false
#define DRAW_KNOCKBACK				false
#define DRAW_HEXA_NUMBER			false
#define DRAW_HEXA_POSITION			false
#define USE_SKILL					false

/* In Game Value  */
#define MAX_HAVE_COST				5
#define MAX_PLAYER_HAVE_UNITS		10
#define MOVE_DURATION				0.3f

/*  Screen  */
#define WIDTH_OF_GRID				1
#define RESOLUTION_SIZEX			1536
#define RESOLUTION_SIZEY			2048
#define DISPLAY_SIZEX				640
#define DISPLAY_SIZEY				740
#define MAP_XSTART					DISPLAY_SIZEX/2
#define MAP_YSTART					DISPLAY_SIZEY/2
#define RADIANS_60					CC_DEGREES_TO_RADIANS(60)
#define PIXEL_TO_RANGE_MULT			0.03
#define HEXAGON_LENGTH				38

/*  Sound  */
#define USE_SOUND					false
#define ARCA_SOUND_GAMESTART		"start.mp3"
#define ARCA_SOUND_MOVE				"move.mp3"
#define ARCA_SOUND_DIE				"die.mp3"
#define ARCA_SOUND_COLLISION		"collision.mp3"
#define ARCA_SOUND_BGM				"bgm.mp3"
#define ARCA_SOUND_MYTURN			"myturn.mp3"

/*  Colors  */
#define COLOR_OF_GRID				ccc4f(0.0f, 0.0f, 0.0f, 0.2f)
#define COLOR_OF_PLAYER				ccc4f(0.0f, 1.0f, 0.6f, 0.5f)
#define COLOR_OF_ENEMY				ccc4f(1.0f, 0.0f, 0.2f, 0.5f)
#define COLOR_OF_CRASHED			ccc4f(1.0f, 0.0f, 0.0f, 0.65f)
#define COLOR_OF_CANTMOVE			ccc4f(0.8f, 0.2f, 0.2f, 0.5f)
#define COLOR_OF_EXPECT				ccc4f(0.0f, 0.6f, 1.0f, 0.2f)
#define COLOR_OF_SKILL				ccc4f(0.5f, 1.0f, 1.0f, 0.4f)

/*  ZOrder  */
#define ZORDER_BLOCK				200
#define ZORDER_CHARACTER			300
#define ZORDER_STAT					400
#define ZORDER_UI					500

/*  UI Sprites  */
#define FRAME_HP					"frame_hp.png"
#define FRAME_ATK					"frame_atk.png"

/*  PC Sprites  */
#define SPRITE_SOLDIER				"character.png"
#define SPRITE_RIDER				"character.png"
#define SPRITE_PRINCE				"character.png"
#define SPRITE_PRINCESS				"character.png"
#define SPRITE_MAGICION				"unit_magician.png"
#define SPRITE_KNIGHT				"character.png"

/*  NPC Sprites  */
#define SPRITE_ROCK					"npc_rock.png"
#define SPRITE_TREE					"npc_tree.png"
#define SPRITE_ARCASTONE			"npc_arcastone.png"
#define SPRITE_BOMB					"npc_bomb.png"
#define SPRITE_PEBBLE				"npc_pebble.png"
#define SPRITE_POTION				"npc_potion.png"

// enum
enum PlayerWho
{
	PW_PLAYERONE = 0,
	PW_PLAYERTWO = 1,
};

// typedef 
typedef Packet::GameStartResult::UnitData UnitData; ///< UnitData를 공유해 쓰려면, 내부 struct가 아니라 외부로 뺄 것. 
///# 그리고 패킷에서 쓰는 구조체를 게임 중에 계속 사용하는 경우는 비효율적임. 왜냐하면 전송용은 1바이트 패킹이 효율적이지만, 
/// 메모리에서 사용하는 경우는 word단위로 alignment되는게 더 효율적이기 때문.


/// 아래 구조체들을 왜 enums.h에 넣어 놓는거지? 이 파일에는 인클루드 디펜던시 없는 상수 리터럴만 모아놓는 파일 아닌가?
// struct
struct Hexagon
{
	Hexagon();
	Hexagon(cocos2d::Point point)
	{
		vertex.reserve(6);
		anchor = point;

		float param_rad, param_cos, param_sin;
		for (int i = 0; i < 6; ++i)
		{
			param_rad = CC_DEGREES_TO_RADIANS(60 * i);
			param_cos = cos(param_rad);
			param_sin = sin(param_rad);

			cocos2d::Point vertexPoint = cocos2d::Point(anchor.x + (HEXAGON_LENGTH * param_cos), anchor.y + (HEXAGON_LENGTH * param_sin));
			vertex.push_back(vertexPoint);
		}
	}
	cocos2d::Point anchor;
	std::vector<cocos2d::Point> vertex;
};

struct Skill
{
	Skill(){
		type = USK_NONE;
		rank = USR_NONE;
	}
	Skill(UnitSkillType _type){
		type = _type;
		rank = USR_RANKONE;
	}
	Skill(UnitSkillType _type, UnitSkillRank _rank){
		type = _type;
		rank = _rank;
	}

	UnitSkillType type;
	UnitSkillRank rank;
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

	HexaDirection GetDirection()
	{
		float degree = CC_RADIANS_TO_DEGREES(atan2(y, x));

		// 3,4분면과 270도에서 직교좌표계 보정
		if ((x < 0 && y < 0) || (x >= 0 && y < 0))
			degree += 360;

		// 헥사그리드 좌표로 변환
		HexaDirection Direction = (HexaDirection)((int)(degree / 60) + 1);

		return Direction;
	}

	int GetRange()
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
	Coord HexaToCoord()
	{
		Coord retCoord;
		retCoord.x = x;
		retCoord.y = y;

		return retCoord;
	}

	HexaDirection GetDirection(HexaPoint point)
	{
		HexaPoint vec;
		vec.x = point.x - x;
		vec.y = point.y - y;

		if (vec.y < 0 && (int)vec.x == 0)
		{
			return HD_NORTH;
		}
		else if (vec.x > 0 && vec.y < 0)
		{
			if ((int)abs(vec.x) != (int)abs(vec.y)) return HD_NONE;

			if (abs(vec.x) != abs(vec.y)) return HD_NONE;  ///< 헐?? float 끼리는 == 비교 하면 안된다. 왜 그런지 공부해보삼.. 0.999999 == 1.00000인 경우는?

			return HD_NORTHEAST;
		}
		else if (vec.x < 0 && (int)vec.y == 0)
		{
			return HD_NORTHWEST;
		}
		else if (vec.y > 0 && (int)vec.x == 0)
		{
			return HD_SOUTH;
		}
		else if (vec.x > 0 && (int)vec.y == 0)
		{
			return HD_SOUTHEAST;
		}
		else if (vec.x < 0 && vec.y > 0)
		{
			if ((int)abs(vec.x) != (int)abs(vec.y)) return HD_NONE;
			return HD_SOUTHWEST;
		}

		return HD_NONE;
	}

	int GetRange()
	{
		if (x == 0) return abs(y);
		if (y == 0) return abs(x);
		if ((int)abs(x) == (int)abs(y)) return abs(x);
	}

	bool isAround(HexaPoint point, int range)
	{
		HexaPoint vec;
		vec.x = point.x - x;
		vec.y = point.y - y;

		if ((int)abs(vec.x) > range || (int)abs(vec.y) > range || (int)abs(vec.x + vec.y) > range)
		{
			return false;
		}
		return true;
	}

	HexaPoint GetMovePoint(HexaDirection direction, int range)
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