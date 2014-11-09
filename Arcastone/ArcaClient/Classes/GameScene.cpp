#include "GameScene.h"
#include "base/CCEventKeyboard.h"
#include "TcpClient.h"

#include "Header.h"
#include "Game.h"

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = GameScene::create();

    // GameScene layer 를 scene 에 추가
    scene->addChild(layer, 1, string("base_layer"));

    return scene;
}

bool GameScene::init()
{
    if ( !LayerColor::initWithColor(ccc4(255,255,255,32)))
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 게임의 진행 상태를 로그인 전으로 설정
	m_GameState = GS_BEFORE_LOGIN;

	// 마우스 이벤트를 사용하기 위해 초기화
	touchEventInit();
	
	// 육각형을 그림. Header.h 의 #define 값들을 수정하여 육각형의 형태 조절 가능
	drawHexaGrid();

	this->schedule(schedule_selector(GameScene::gameLogic), 0.0f);

    return true;
}

void GameScene::gameLogic(float dt)
{
	switch (m_GameState)
	{
	case GS_BEFORE_LOGIN:
	{
		auto scene = Director::getInstance()->getRunningScene();
		if (scene != nullptr && this->getParent() == scene)
		{
			TcpClient::getInstance()->loginRequest();
			m_GameState = GS_WAIT_GAME;
		}
	}
		break;
	case GS_WAIT_GAME:
	{

	}
		break;
	case GS_GAME_START:			// GameStartResult 를 받은 상태
	{
									//if (m_IsAction && m_IsMyTurn) TcpClient::getInstance()->TurnEndRequest();
	}
		break;
	default:
		break;
	}
	
}

void GameScene::touchEventInit()
{
	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(true);

	_touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();

	dispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return false;			// 자신의 턴인 경우에만 마우스 입력을 받는다.
		
	m_StartPoint.x = touch->getLocation().x;
	m_StartPoint.y = touch->getLocation().y;
	
	
	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);
		// 자신의 유닛인지 확인한다
		if (unit->getOwner() != UO_ME) return false;

		// 자신 유닛의 좌표인덱스를 화면상 위치로 변환하여
		ScreenPoint screenPoint = unit->getPosition().HexaToScreen();

		// 자신의 유닛을 클릭했는지 판정한다.
		if (touchCheck(m_StartPoint, screenPoint))
		{
			// 그 유닛을 선택했다는 것을 지정하기 위해 id 를 멤버변수에 저장한다.
			m_SelectedUnit = unit->getID();
			return true;
		}
	}

	return false;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	// 현재 마우스가 상주한 index 를 저장해두고, 이 index 값이 바뀔 때만 아래 코드 실행함
	ScreenPoint currentPoint(touch->getLocation().x, touch->getLocation().y);
	if (m_CursoredPoint == currentPoint) return;
	m_CursoredPoint = currentPoint;

	Unit* unit = getUnit(m_SelectedUnit);
	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;


	// 공격이 향하는 위치 찾기
	HexaDirection direction = getPointToPointDirection(m_StartPoint, m_CursoredPoint);
	int distance = getPointToPointDistance(m_StartPoint, m_CursoredPoint);
	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();


	// 표시된 이동경로 초기화
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();

	// 공격 경로에 이동경로 표시
	for (int i = 1; i <= distance; ++i)
	{
		HexaPoint attackCourse = getPointMoveDirection(unit->getPosition(), direction, i);
		Hexagon* courseSignHexagon = createHexagon(attackCourse.HexaToScreen(), HEXAGON_LENGTH);
		DrawNode* courseSignNode = DrawNode::create();

		courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, ccc4f(0.2f, 1.0f, 0.2f, 0.3f), 0.5, ccc4f(0.2f, 1.0f, 0.2f, 0.3f));
		this->addChild(courseSignNode,0);
		m_CourseSignNode.push_back(courseSignNode);
	}

}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();


	// 클릭한 상태로 턴이 넘어갔을 경우 마우스 떼도 이 코드 안돌아가니 주의

	Unit* unit = getUnit(m_SelectedUnit);

	HexaDirection direction = getPointToPointDirection(m_StartPoint, m_CursoredPoint);
	int distance = getPointToPointDistance(m_StartPoint, m_CursoredPoint);

	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();

	// 공격을 취소하는 경우
	if (distance == 0)
		return;
 
	if (DEBUG_PRINT_PACKET) 
		printf("%f\n", direction);

	TcpClient::getInstance()->attackRequest(m_SelectedUnit, distance, direction);
	m_SelectedUnit = NON_SELECT_UNIT;
}

// 클릭한 좌표가 특정 육각형 안에 들어있는지 확인하는 함수
bool GameScene::touchCheck(ScreenPoint touch, ScreenPoint anchor)
{
	float msin = sin(RADIANS_60);

	// 클릭한 좌표가 육각형의 x 범위에 속하는지 확인
	if (touch.y > anchor.y - HEXAGON_LENGTH * msin &&
		touch.y < anchor.y + HEXAGON_LENGTH * msin)
	{

		float yHowFarToAnchor = abs(touch.y - anchor.y);

		// 육각형의 중심에서 y 거리로 멀어질수록 x 판정이 좁아지는 것을 구현하는 코드
		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;

		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

// 유닛을 찾습니다
Unit* GameScene::getUnit(int unitID)
{
	for (auto unit : m_UnitList)
	{
		if (unitID == unit->getID())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
}

// 두 Point 사이의 Direction 을 구하는 함수
HexaDirection GameScene::getPointToPointDirection(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float degree = CC_RADIANS_TO_DEGREES(atan2(y, x));
 
	// 직교좌표계 보정
	if ((x < 0 && y < 0) || (x > 0 && y < 0))
		degree += 360;

	// 헥사그리드 좌표로 변환
	HexaDirection Direction = (HexaDirection)((int)(degree / 60) + 1);

	return Direction;
}

// 두 Point 사이의 Distance 를 구하는 함수
int GameScene::getPointToPointDistance(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	return (int)(sqrt(x*x + y*y))/30;
}

void GameScene::drawHexaGrid()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node);

	for (int i = 0; i < MAP_SIZEX; ++i)
	{
		for (int j = 0; j < MAP_SIZEY; ++j)
		{
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// 육각형으로 그리기 위한 조건문
				continue;

			ScreenPoint point = HexaPoint(i, j).HexaToScreen();

			if (drawToRect(point.y) && MAP_IS_RECT)	// 사각형으로 그리기 위한 조건문
				continue;

			m_HexagonPoint.push_back(Coord(i, j));	// m_HexagonPoint 에 화면에 그려지는 좌표(0~x, 0~y)들을 저장

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, ccc4f(0.2f, 1.0f, 0.2f, 0.3f));
			
			if(DRAW_HEXA_NUMBER) drawText(i, j, hexa);	// 헥사곤 안에 정수형 인덱스 값을 보여줄 것인가?
		}
	}
}

// 육각형 안에 정수형 인덱스 값을 보여주는 함수 .
// DRAW_HEXA_POSITION 이 true면 인덱스 값이 아닌 위치 값을 보여줌
void GameScene::drawText(int i, int j, Hexagon* hexa)
{
	int f = i;
	if (DRAW_HEXA_POSITION) f = hexa->anchor.x;
	char szBuf1[8];
	itoa(f, szBuf1, 10);

	f = j;
	if (DRAW_HEXA_POSITION) f = hexa->anchor.y;
	char szBuf2[8];
	itoa(f, szBuf2, 10);

	LabelTTF* vLabelx;
	LabelTTF* vLabely;
	vLabelx = LabelTTF::create(szBuf1, "Hevetica", 12);
	vLabely = LabelTTF::create(szBuf2, "Hevetica", 12);

	vLabelx->setPosition(Point(hexa->anchor.x - 7, hexa->anchor.y + 5));
	vLabely->setPosition(Point(hexa->anchor.x + 7, hexa->anchor.y - 5));

	vLabelx->setColor(Color3B(255, 255, 255));
	vLabely->setColor(Color3B(255, 0, 0));

	this->addChild(vLabelx);
	this->addChild(vLabely);
}


// 사각형이 되도록 그려주는 부분을 제외하기 위한 함수 .
bool GameScene::drawToRect(float y)	
{
	if (y <= MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1) ||
		y >= MAP_YSTART + HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1))
		return true;

	return false;
}

// 육각형이 되도록 그려주는 부분을 제외하기 위한 함수 .
bool GameScene::drawToHexa(int x, int y)
{
	if (x + y >= MAP_SIZEX / 2 &&
		x + y <= MAP_SIZEX / 2 + MAP_SIZEY - 1)
		return false;

	return true;
}

// 중심점과 크기를 넣으면, 그에 대한 육각형의 각 꼭짓점을 벡터에 넣어서 리턴하는 함수 .
Hexagon* GameScene::createHexagon(ScreenPoint anchor, int size)
{
	Hexagon* newHexa = new Hexagon;
	newHexa->anchor = Point(anchor.x, anchor.y);

	float param_rad, param_cos, param_sin;
	for (int i = 0; i < 6; ++i)
	{
		param_rad = CC_DEGREES_TO_RADIANS(360 / 6 * i);
		param_cos = cos(param_rad);
		param_sin = sin(param_rad);

		Point vertexPoint = Point(anchor.x + (size * param_cos), anchor.y + (size * param_sin));
		newHexa->vertex.push_back(vertexPoint);
	}
	return newHexa;
}

// 서버로부터 받은 유닛 데이터를 field와 unit 에 저장하는 함수
// 유닛을 저장한 후, 화면에 유닛을 그린다. 그리고 게임 시작 상태로 전환한다.
void GameScene::ReadUnitData(UnitData unitData[], int length)
{
	m_UnitList.reserve(length);

	for (int i = 0; i < length; ++i)
	{
		Unit* unit = Unit::create(unitData[i]);

		m_UnitList.push_back(unit);

		// 유닛의 위치를 화면상의 좌표로 변환하고 screenPoint 에 저장
		Sprite* unitSprite = unit->getSprite();
		ScreenPoint unitRealPoint = unit->getPosition().HexaToScreen();

		// 화면상의 좌표에 유닛을 배치
		unitSprite->setPosition(Point(unitRealPoint.x, unitRealPoint.y));
		this->addChild(unitSprite);
	}

	m_GameState = GS_GAME_START;
}


void GameScene::ReadActionQueue(UnitAction unitActionQueue[], int length)
{
	for (int i = 0; i < length; ++i)
	{
		// i 번째 액션을 가져옴
		UnitAction action = unitActionQueue[i];

		// id를 사용하여 i 번째 액션을 적용할 유닛을 찾음
		for (int j = 0; j < m_UnitList.size(); ++j)
		{
			Unit* pUnit = m_UnitList.at(j);
			int unitId = pUnit->getID();
			if (unitId == action.mUnitId)
			{
				HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
				ScreenPoint sMovePoint = hMovePoint.HexaToScreen();
				Point movePoint(sMovePoint.x, sMovePoint.y);

				if (DEBUG_PRINT_PACKET)
					printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

				// 유닛은 인덱스로 이동
				pUnit->setPosition(hMovePoint);
				// 스프라이트는 좌표로 이동
				pUnit->getSprite()->setPosition(movePoint);
				break;
			}
		}
	}
}

HexaPoint GameScene::getPointMoveDirection(HexaPoint start, HexaDirection direction, int range)
{
	HexaPoint retPoint = start;

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
