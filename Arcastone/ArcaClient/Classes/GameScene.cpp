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

	m_TouchDrawNode = CCDrawNode::create();
	addChild(m_TouchDrawNode);

	// 육각형을 그림. Header.h 의 #define 값들을 수정하여 육각형의 형태 조절 가능
	drawHexagon();

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
								m_GameState = GS_WAIT_LOGIN;
							}
	}
		break;
	case GS_WAIT_LOGIN:
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

	// 선을 지움
	m_TouchDrawNode->clear();
	
	ScreenPoint clickPoint(touch->getLocation().x, touch->getLocation().y);

	m_SelectedUnitIndex = NON_SELECT_UNIT;

	Player* pPlayer = m_Game.getPlayer(PW_PLAYERONE);
	for (int i = 0; i < pPlayer->getUnitCounter(); ++i)
	{
		// 자신의 유닛들의 좌표인덱스를 하나씩 참조하고
		HexaPoint unitPoint = pPlayer->getUnit(i)->getUnitStatus().point;
		// 좌표인덱스를 화면상 위치로 변환하여
		ScreenPoint screenPoint = conversionIndexToPoint(unitPoint);
		// 자신의 유닛을 클릭했는지 판정한다.
		if (touchCheck(clickPoint, screenPoint))
		{
			Point point(screenPoint.x, screenPoint.y);
			m_TouchDrawNode->drawDot(point, 18, ccc4f(0.7, 0.7, 0.7, 0.7));

			// 그 유닛을 선택했다는 것을 지정하기 위해 id 를 멤버변수에 저장한다.
			m_SelectedUnitIndex = pPlayer->getUnit(i)->getUnitStatus().id;
			m_SelectecUnitIndexOfPlayer = i;
			return true;
		}
	}

	return false;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	Unit* unit = m_Game.getPlayer(PW_PLAYERONE)->getUnit(m_SelectecUnitIndexOfPlayer);

	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnitIndex == NON_SELECT_UNIT || unit == nullptr) return;

	// 마우스 드래그 중 속도 저하 발생할 경우,
	// 현재 마우스가 상주한 index 를 저장해두고, 이 index 값이 바뀔 때만 아래의 for 문을 회전할 것 .

	// 이전의 선을 지움
	m_TouchDrawNode->clear();

	ScreenPoint clickPoint(touch->getLocation().x, touch->getLocation().y);

	for (int i = 0; i < m_HexagonPoint.size(); ++i)
	{
		// 각각의 격자위치를 참조하여 현재 마우스가 어느 인덱스에 있는지 알아냄
		HexaPoint hexaPoint(m_HexagonPoint.at(i));
		ScreenPoint screenPoint = conversionIndexToPoint(hexaPoint);

		// 찾아냈다! 마우스는 i번째 그리드에 있어!
		if (touchCheck(clickPoint, screenPoint))
		{
			HexaPoint unitPoint = unit->getUnitStatus().point;

			// 선택한 유닛부터 마우스가 있는 격자까지 선을 그음
			ScreenPoint pointFrom = conversionIndexToPoint(unitPoint);
			ScreenPoint pointTo = screenPoint;

			Point pF(pointFrom.x, pointFrom.y);
			Point pT(pointTo.x, pointTo.y);
			m_TouchDrawNode->drawSegment(pF, pT, 10, ccc4f(0.7, 0.7, 0.7, 0.7));

			// 마우스가 있는 격자의 위치 저장
			m_CursoredPoint = screenPoint;
			return;
		}
	}
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	// 클릭한 상태로 턴이 넘어갔을 경우 마우스 떼도 이 코드 안돌아가니 주의

	Unit* unit = m_Game.getPlayer(PW_PLAYERONE)->getUnit(m_SelectecUnitIndexOfPlayer);
	HexaPoint unitPoint = unit->getUnitStatus().point;

	ScreenPoint pointFrom = conversionIndexToPoint(unitPoint);
	ScreenPoint pointTo = m_CursoredPoint;

 	float direction = getPointToPointDirection(pointFrom, pointTo);
	float distance = getPointToPointDistance(pointFrom, pointTo);

	int attackRange;
	if (distance < HEXAGON_LENGTH * sin(RADIANS_60) * 1)
		attackRange = 0;
	else if (distance < HEXAGON_LENGTH * sin(RADIANS_60) * 3)
		attackRange = 1;
	else if (distance < HEXAGON_LENGTH * sin(RADIANS_60) * 5)
		attackRange = 2;
	else attackRange = 2;

	// 공격을 취소하는 경우
	if (attackRange == 0)
		return;
 
 	HexaDirection attackDirection;
 	if (direction < 60)
		attackDirection = HD_SOUTHEAST;
 	else if (direction < 120)
		attackDirection = HD_SOUTH;
 	else if (direction < 180)
		attackDirection = HD_SOUTHWEST;
 	else if (direction < 240)
		attackDirection = HD_NORTHWEST;
 	else if (direction < 300)
		attackDirection = HD_NORTH;
	else attackDirection = HD_NORTHEAST;

	TcpClient::getInstance()->attackRequest(m_SelectedUnitIndex, attackRange, attackDirection);

	m_TouchDrawNode->clear();
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

// 두 Point 사이의 Direction 을 구하는 함수
float GameScene::getPointToPointDirection(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float r = sqrt(x*x + y*y);
	return acos(x / r);
}

// 두 Point 사이의 Distance 를 구하는 함수
float GameScene::getPointToPointDistance(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	return sqrt(x*x + y*y);
}

void GameScene::drawHexagon()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node);

	for (int i = 0; i < MAP_SIZEX; ++i)
	{
		for (int j = 0; j < MAP_SIZEY; ++j)
		{
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// 육각형으로 그리기 위한 조건문
				continue;

			ScreenPoint point = conversionIndexToPoint(HexaPoint(i, j));

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

// 입력한 index 형식의 point 값을 화면에 그릴 수 있는 point 값으로 변환해주는 함수 .
ScreenPoint GameScene::conversionIndexToPoint(HexaPoint point)
{
	ScreenPoint retPoint;

	// MAP_START 를 중앙에 위치하도록 그려주기 위한 수식들 .
	retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (point.x - (MAP_SIZEX - 1)*0.5);
	retPoint.y = MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (point.y * 2 - MAP_SIZEY + point.x - (MAP_SIZEX - 3)*0.5);

	return retPoint;
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
	m_Game.getField()->setUnitData(unitData, length);

	for (int i = 0; i < MAX_UNIT_ON_GAME; ++i)
	{
		switch (unitData[i].unitOwner)				// 유닛의 Owner 를 구분하여 올바른 대상에게 유닛을 제공
		{
		case UO_ME:
			m_Game.getPlayer(PW_PLAYERONE)->setUnit(unitData[i]);
			break;
		case UO_ENEMY:
			m_Game.getPlayer(PW_PLAYERTWO)->setUnit(unitData[i]);
			break;
		default:
			break;
		}
	}

	drawUnit();

	m_GameState = GS_GAME_START;
}

void GameScene::drawUnit()
{
	Field* pField = m_Game.getField();

	for (int i = 0; i < pField->getUnitDataLength(); ++i)
	{
		UnitData unitData = pField->getUnitData(i);
		switch (unitData.unitOwner)
		{
		case UO_ME:
			m_UnitSprite[i] = Sprite::create("Me.png");
			break;
		case UO_ENEMY:
			m_UnitSprite[i] = Sprite::create("Enomy.png");
			break;
		case UO_NPC:
			m_UnitSprite[i] = Sprite::create("Projectile.png");
			break;
		default:
			break;
		}

		if (unitData.unitType != UT_NONE)
		{
			// i 번째 유닛 데이터의 x, y 인덱스를
			HexaPoint unitPoint(unitData.point.x, unitData.point.y);

			// 화면상의 좌표로 변환하고 screenPoint 에 저장
			ScreenPoint unitRealPoint = conversionIndexToPoint(unitPoint);

			// 화면상의 좌표에 유닛을 배치
			m_UnitSprite[i]->setPosition(Point(unitRealPoint.x, unitRealPoint.y));

			this->addChild(m_UnitSprite[i]);
		}
		
	}
}