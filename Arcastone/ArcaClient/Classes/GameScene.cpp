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

	m_GameState = GS_BEFORE_LOGIN;		// 게임의 진행 상태를 로그인 전으로 설정

	touchEventInit();					// 마우스 이벤트를 사용하기 위해 초기화
	m_TouchDrawNode = CCDrawNode::create();
	addChild(m_TouchDrawNode);

	drawHexagon();						// 육각형을 그림. Header.h 의 #define 값들을 수정하여 육각형의 형태 조절 가능

	m_IsAction = false;

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
	//if (m_IsMyTurn && !m_IsAction)			// 자신의 턴이고 행동하지 않은 경우에만 마우스 입력을 받는다.
	{
		Point clickPoint = touch->getLocation();

		m_SelectedUnitIndex = NON_SELECT_UNIT;
		m_TouchDrawNode->clear();

		Player* pPlayer = m_Game.getPlayer(PW_PLAYERONE);
		for (int i = 0; i < pPlayer->getUnitCounter(); ++i)
		{
			int x = pPlayer->getUnit(i)->getUnitStatus().x;
			int y = pPlayer->getUnit(i)->getUnitStatus().y;
			// 자신의 유닛들의 좌표인덱스를 하나씩 참조하고
			Point point = conversionIndexToPoint(Point(x, y));
			// 좌표인덱스를 화면상 위치로 변환하여

			if (touchCheck(clickPoint, point))
			{
				m_TouchDrawNode->drawSegment(point, point, 15, ccc4f(0.7, 0.7, 0.7, 0.7));

				m_SelectedUnitIndex = pPlayer->getUnit(i)->getUnitStatus().id;
				// 그 유닛을 선택했다는 것을 지정하기 위해 id 를 멤버변수에 저장한다.
				return true;
			}
		}
		if (m_SelectedUnitIndex == NON_SELECT_UNIT)
			return false; // for skip TouchMoved
	}
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	//if (m_IsMyTurn && !m_IsAction)
	{

		Point clickPoint = touch->getLocation();
		
		Player* pPlayer = m_Game.getPlayer(PW_PLAYERONE);
		auto unit = pPlayer->getUnit(m_SelectedUnitIndex);

		if (m_SelectedUnitIndex != NON_SELECT_UNIT && unit != nullptr)
		{
			int x = pPlayer->getUnit(m_SelectedUnitIndex)->getUnitStatus().x;
			int y = pPlayer->getUnit(m_SelectedUnitIndex)->getUnitStatus().y;
			Point pointFrom = conversionIndexToPoint(Point(x, y));
			Point pointTo = conversionIndexToPoint(conversionPointToIndex(clickPoint));

			m_TouchDrawNode->clear();
			m_TouchDrawNode->drawSegment(pointFrom, pointTo, 15, ccc4f(0.7, 0.7, 0.7, 0.7));
		}
	}
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	//if (m_IsMyTurn && !m_IsAction)
	{
		// 클릭한 상태로 턴이 넘어갔을 경우 마우스 떼도 이 코드 안돌아가니 주의

// 		Point clickPoint = touch->getLocation();
// 
// 		float direction = getPointToPointDirection(m_PathPoint.at(0), clickPoint);
// 		float distance = getPointToPointDirection(m_PathPoint.at(0), clickPoint);
// 
// 		HexaDirection hDirection;
// 		if (direction < 60)
// 			hDirection = HD_SOUTHEAST;
// 		else if (direction < 120)
// 			hDirection = HD_SOUTH;
// 		else if (direction < 180)
// 			hDirection = HD_SOUTHWEST;
// 		else if (direction < 240)
// 			hDirection = HD_NORTHWEST;
// 		else if (direction < 300)
// 			hDirection = HD_NORTH;
// 		else hDirection = HD_NORTHEAST;
		// enum 값 조절하면 div 함수 이용- 한줄로 표현 가능

		// TODO : hDirection 과 distance 를 이용해서 선택한 유닛(m_SelectedUnitIndex) 에게 이동.공격 명령 하기
		// 공격하거나 이동했을 경우 m_IsAction = true; 하여 행동했음을 알린다.


		m_TouchDrawNode->clear();
	}
}

bool GameScene::touchCheck(Point touch, Point anchor)
{
	// 클릭한 좌표가 특정 육각형 안에 들어있는지 확인하는 함수
	float msin = sin(RADIANS_60);

	if (touch.y > anchor.y - HEXAGON_LENGTH * msin &&
		touch.y < anchor.y + HEXAGON_LENGTH * msin)
	{
		// 클릭한 좌표가 육각형의 x 범위에 속하는지 확인

		float yHowFarToAnchor = abs(touch.y - anchor.y);

		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;
		// 육각형의 중심에서 y 거리로 멀어질수록 x 판정이 좁아지는 것을 구현하는 코드

		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

float GameScene::getPointToPointDirection(Point point1, Point point2)
{
	// 두 Point 사이의 Direction 을 구하는 함수
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float r = sqrt(x*x + y*y);
	return acos(x / r);
}

float GameScene::getPointToPointDistance(Point point1, Point point2)
{
	// 두 Point 사이의 Distance 를 구하는 함수
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	return sqrt(x*x + y*y);
}

void GameScene::drawHexagon()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node);

	Point point;

	for (int i = 0; i < MAP_SIZEX; ++i)
	{
		for (int j = 0; j < MAP_SIZEY; ++j)
		{
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// 육각형으로 그리기 위한 조건문
				continue;

			point = conversionIndexToPoint(Point(i, j));

			if (drawToRect(point.y) && MAP_IS_RECT)	// 사각형으로 그리기 위한 조건문
				continue;

			m_HexagonPoint.push_back(Point(i, j));	// m_HexagonPoint 에 화면에 그려지는 좌표(0~x, 0~y)들을 저장

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, ccc4f(0.2f, 1.0f, 0.2f, 0.3f));
			
			if(DRAW_HEXA_NUMBER) drawText(i, j, hexa);	// 헥사곤 안에 정수형 인덱스 값을 보여줄 것인가?
		}
	}
}

void GameScene::drawText(int i, int j, Hexagon* hexa)
{
	// 육각형 안에 정수형 인덱스 값을 보여주는 함수 .
	// DRAW_HEXA_POSITION 이 true면 인덱스 값이 아닌 위치 값을 보여줌
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
Point GameScene::conversionIndexToPoint(Point point)
{
	Point retPoint;

	retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (point.x - (MAP_SIZEX - 1)*0.5);
	retPoint.y = MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (point.y * 2 - MAP_SIZEY + point.x - (MAP_SIZEX - 3)*0.5);
	// MAP_START 를 중앙에 위치하도록 그려주기 위한 수식들 .

	return retPoint;
}

// 입력한 화면상의 좌표를 인덱스좌표로 변환해주는 함수 .
Point GameScene::conversionPointToIndex(Point point)
{
	Point retPoint;

	retPoint.x = (point.x - MAP_XSTART) / HEXAGON_LENGTH / 1.5 + (MAP_SIZEX - 1)*0.5;
	retPoint.y = ((point.y - MAP_YSTART) / -HEXAGON_LENGTH / sin(RADIANS_60) + MAP_SIZEY - retPoint.x + (MAP_SIZEX - 3)*0.5)*0.5;

	return retPoint;
}

bool GameScene::drawToRect(float y)	
{
	// 사각형이 되도록 그려주는 부분을 제외하기 위한 함수 .
	if (y <= MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1) ||
		y >= MAP_YSTART + HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1))
		return true;

	return false;
}

bool GameScene::drawToHexa(int x, int y)
{
	// 육각형이 되도록 그려주는 부분을 제외하기 위한 함수 .
	if (x + y >= MAP_SIZEX / 2 &&
		x + y <= MAP_SIZEX / 2 + MAP_SIZEY - 1)
		return false;

	return true;
}

Hexagon* GameScene::createHexagon(Point anchor, int size)
{
	// 중심점과 크기를 넣으면, 그에 대한 육각형의 각 꼭짓점을 벡터에 넣어서 리턴하는 함수 .
	Hexagon* newHexa = new Hexagon;
	newHexa->anchor = anchor;

	Point pos;
	float param_rad, param_cos, param_sin;
	for (int i = 0; i < 6; ++i)
	{
		param_rad = CC_DEGREES_TO_RADIANS(360 / 6 * i);
		param_cos = cos(param_rad);
		param_sin = sin(param_rad);

		pos.x = anchor.x + (size * param_cos);
		pos.y = anchor.y + (size * param_sin);
		newHexa->vertex.push_back(pos);
	}
	return newHexa;
}

void GameScene::ReadUnitData(UnitData unitData[], int length)
{
	// 서버로부터 받은 유닛 데이터를 field와 unit 에 저장하는 함수
	// 유닛을 저장한 후, 화면에 유닛을 그린다. 그리고 게임 시작 상태로 전환한다.
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
		m_UnitDrawNode[i] = CCDrawNode::create();
		this->addChild(m_UnitDrawNode[i]);

		int xIndex = pField->getUnitData(i).x;
		int yIndex = pField->getUnitData(i).y;
		// i 번째 유닛 데이터의 x, y 인덱스를
		Point screenPoint = conversionIndexToPoint(Point(xIndex, yIndex));
		// 화면상의 좌표로 변환하고 screenPoint 에 저장

		Hexagon* hexa = createHexagon(screenPoint, 15);

		if (pField->getUnitData(i).unitOwner == UO_ME)
			m_UnitDrawNode[i]->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.5f, 0.8f), 1, ccc4f(0.0f, 1.0f, 0.2f, 0.3f));
		else
			m_UnitDrawNode[i]->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.5f, 0.0f, 0.0f, 0.8f), 1, ccc4f(0.0f, 1.0f, 0.2f, 0.3f));
		// 화면상의 좌표에 폴리곤을 그림. 유닛의 주인에 따라 다른 색

		
		//  현재 스프라이트 생성하는 것에 문제가 있어서 drawPolygon 으로 대체한 상태임. 추후 수정 요망

		m_UnitSprite[i] = Sprite::create("Player.png"); // ("WhithPawn.png"); <- 이미지 파일 어디감?
		if (pField->getUnitData(i).unitType != UT_NONE)
		{
			int xIndex = pField->getUnitData(i).x;
			int yIndex = pField->getUnitData(i).y;
			// i 번째 유닛 데이터의 x, y 인덱스를

			Point screenPoint = conversionIndexToPoint(Point(xIndex, yIndex));
			// 화면상의 좌표로 변환하고 screenPoint 에 저장

			m_UnitSprite[i]->setPosition(screenPoint);
			// 화면상의 좌표에 유닛을 배치

			this->addChild(m_UnitSprite[i]);
		}
		
	}
}