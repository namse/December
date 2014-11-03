#pragma once

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

