// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <json.h>
#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <string>
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
#include "enums.h"

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include "../../PacketType.h"

Coord GetUnitVector(HexaDirection direction);
HexaDirection GetHexaDirection(Coord from, Coord to);
int GetVectorSize(Coord from, Coord to);