// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// EasyServer.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.

Coord GetUnitVector(HexaDirection direction)
{
	switch (direction)
	{
	case HD_NONE:
		return Coord();
		break;
	case HD_NORTH:
		return Coord(0, -1);
		break;
	case HD_NORTHEAST:
		return Coord(1, -1);
		break;
	case HD_NORTHWEST:
		return Coord(-1, 0);
		break;
	case HD_SOUTHEAST:
		return Coord(1, 0);
		break;
	case HD_SOUTHWEST:
		return Coord(-1, 1);
		break;
	case HD_SOUTH:
		return Coord(0, 1);
		break;
	default:
		return Coord();
		break;
	}
}

HexaDirection GetHexaDirection(Coord from, Coord to)
{
	Coord vec = to - from;
	int vectorSize = GetVectorSize(from, to);
	Coord normalizedVec = vec / vectorSize;

	for (int i = 1; i <= 6; i++)
	{
		if (GetUnitVector((HexaDirection)i) == normalizedVec)
			return (HexaDirection)i;
	}
	return HexaDirection::HD_NONE;
}

int GetVectorSize(Coord from, Coord to)
{
	Coord vec = to - from;
	if (abs(vec.x) > 0) return abs(vec.x);
	else return abs(vec.y);
}
