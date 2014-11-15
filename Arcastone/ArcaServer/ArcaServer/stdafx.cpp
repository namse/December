// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// EasyServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

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
