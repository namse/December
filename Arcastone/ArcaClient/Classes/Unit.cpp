#include "Unit.h"


Unit::Unit()
{
}


Unit::~Unit()
{
}

bool Unit::setPosition(Position args)
{
	if ((args.x < 0 || args.x > MAP_SIZEX) || (args.y < 0 || args.y > MAP_SIZEY))
		return false;

	m_Position.x = args.x;
	m_Position.y = args.y;
	return true;
}