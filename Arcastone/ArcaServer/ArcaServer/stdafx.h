﻿// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <assert.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <atomic>
#include <typeinfo>
#include <map>
#include <queue>
#include <hash_map>
#include <fstream>
#include <time.h>
#include <json.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include "../../PacketType.h"
#include "enums.h"

Coord GetUnitVector(HexaDirection direction);
HexaDirection GetHexaDirection(Coord from, Coord to);
int GetVectorSize(Coord from, Coord to);
HexaDirection GetDirection(Coord point1, Coord point2);
Coord GetPositionByRange(Coord position, HexaDirection direction, int range);