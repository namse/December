#include "Unit.h"


Unit::Unit()
{
}


Unit::~Unit()
{
}

bool Unit::setPosition(Point args)
{
	// ���� �׸��� ��Ŀ� ���� ���ǹ� �ٸ��� �� ��
	if ((args.x < 0 || args.x > MAP_SIZEX) || (args.y < 0 || args.y > MAP_SIZEY))
		return false;

	m_UnitStatus.point.x = args.x;
	m_UnitStatus.point.y = args.y;
	return true;
}