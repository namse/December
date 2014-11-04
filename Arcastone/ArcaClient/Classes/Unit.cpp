#include "Unit.h"


Unit::Unit()
{
}


Unit::~Unit()
{
}

bool Unit::setPosition(HexaPoint args)
{
	// TODO : 맵을 그리는 방식(hexa 인지 rect 인지)에 따라 조건문 다르게 할 것
	if ((args.x < 0 || args.x > MAP_SIZEX) || (args.y < 0 || args.y > MAP_SIZEY))
		return false;

	m_UnitStatus.point.x = args.x;
	m_UnitStatus.point.y = args.y;
	return true;
}