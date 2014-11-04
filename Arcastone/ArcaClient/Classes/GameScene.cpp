#include "GameScene.h"
#include "base/CCEventKeyboard.h"
#include "TcpClient.h"

#include "Header.h"
#include "Game.h"

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = GameScene::create();

    // GameScene layer �� scene �� �߰�
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

	m_GameState = GS_BEFORE_LOGIN;		// ������ ���� ���¸� �α��� ������ ����

	touchEventInit();					// ���콺 �̺�Ʈ�� ����ϱ� ���� �ʱ�ȭ
	m_TouchDrawNode = CCDrawNode::create();
	addChild(m_TouchDrawNode);

	drawHexagon();						// �������� �׸�. Header.h �� #define ������ �����Ͽ� �������� ���� ���� ����

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
	case GS_GAME_START:			// GameStartResult �� ���� ����
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
	//if (m_IsMyTurn && !m_IsAction)			// �ڽ��� ���̰� �ൿ���� ���� ��쿡�� ���콺 �Է��� �޴´�.
	{
		Point clickPoint = touch->getLocation();

		m_SelectedUnitIndex = NON_SELECT_UNIT;
		m_TouchDrawNode->clear();

		Player* pPlayer = m_Game.getPlayer(PW_PLAYERONE);
		for (int i = 0; i < pPlayer->getUnitCounter(); ++i)
		{
			int x = pPlayer->getUnit(i)->getUnitStatus().x;
			int y = pPlayer->getUnit(i)->getUnitStatus().y;
			// �ڽ��� ���ֵ��� ��ǥ�ε����� �ϳ��� �����ϰ�
			Point point = conversionIndexToPoint(Point(x, y));
			// ��ǥ�ε����� ȭ��� ��ġ�� ��ȯ�Ͽ�

			if (touchCheck(clickPoint, point))
			{
				m_TouchDrawNode->drawSegment(point, point, 15, ccc4f(0.7, 0.7, 0.7, 0.7));

				m_SelectedUnitIndex = pPlayer->getUnit(i)->getUnitStatus().id;
				// �� ������ �����ߴٴ� ���� �����ϱ� ���� id �� ��������� �����Ѵ�.
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
		// Ŭ���� ���·� ���� �Ѿ�� ��� ���콺 ���� �� �ڵ� �ȵ��ư��� ����

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
		// enum �� �����ϸ� div �Լ� �̿�- ���ٷ� ǥ�� ����

		// TODO : hDirection �� distance �� �̿��ؼ� ������ ����(m_SelectedUnitIndex) ���� �̵�.���� ��� �ϱ�
		// �����ϰų� �̵����� ��� m_IsAction = true; �Ͽ� �ൿ������ �˸���.


		m_TouchDrawNode->clear();
	}
}

bool GameScene::touchCheck(Point touch, Point anchor)
{
	// Ŭ���� ��ǥ�� Ư�� ������ �ȿ� ����ִ��� Ȯ���ϴ� �Լ�
	float msin = sin(RADIANS_60);

	if (touch.y > anchor.y - HEXAGON_LENGTH * msin &&
		touch.y < anchor.y + HEXAGON_LENGTH * msin)
	{
		// Ŭ���� ��ǥ�� �������� x ������ ���ϴ��� Ȯ��

		float yHowFarToAnchor = abs(touch.y - anchor.y);

		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;
		// �������� �߽ɿ��� y �Ÿ��� �־������� x ������ �������� ���� �����ϴ� �ڵ�

		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

float GameScene::getPointToPointDirection(Point point1, Point point2)
{
	// �� Point ������ Direction �� ���ϴ� �Լ�
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float r = sqrt(x*x + y*y);
	return acos(x / r);
}

float GameScene::getPointToPointDistance(Point point1, Point point2)
{
	// �� Point ������ Distance �� ���ϴ� �Լ�
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
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// ���������� �׸��� ���� ���ǹ�
				continue;

			point = conversionIndexToPoint(Point(i, j));

			if (drawToRect(point.y) && MAP_IS_RECT)	// �簢������ �׸��� ���� ���ǹ�
				continue;

			m_HexagonPoint.push_back(Point(i, j));	// m_HexagonPoint �� ȭ�鿡 �׷����� ��ǥ(0~x, 0~y)���� ����

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, ccc4f(0.2f, 1.0f, 0.2f, 0.3f));
			
			if(DRAW_HEXA_NUMBER) drawText(i, j, hexa);	// ���� �ȿ� ������ �ε��� ���� ������ ���ΰ�?
		}
	}
}

void GameScene::drawText(int i, int j, Hexagon* hexa)
{
	// ������ �ȿ� ������ �ε��� ���� �����ִ� �Լ� .
	// DRAW_HEXA_POSITION �� true�� �ε��� ���� �ƴ� ��ġ ���� ������
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

// �Է��� index ������ point ���� ȭ�鿡 �׸� �� �ִ� point ������ ��ȯ���ִ� �Լ� .
Point GameScene::conversionIndexToPoint(Point point)
{
	Point retPoint;

	retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (point.x - (MAP_SIZEX - 1)*0.5);
	retPoint.y = MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (point.y * 2 - MAP_SIZEY + point.x - (MAP_SIZEX - 3)*0.5);
	// MAP_START �� �߾ӿ� ��ġ�ϵ��� �׷��ֱ� ���� ���ĵ� .

	return retPoint;
}

// �Է��� ȭ����� ��ǥ�� �ε�����ǥ�� ��ȯ���ִ� �Լ� .
Point GameScene::conversionPointToIndex(Point point)
{
	Point retPoint;

	retPoint.x = (point.x - MAP_XSTART) / HEXAGON_LENGTH / 1.5 + (MAP_SIZEX - 1)*0.5;
	retPoint.y = ((point.y - MAP_YSTART) / -HEXAGON_LENGTH / sin(RADIANS_60) + MAP_SIZEY - retPoint.x + (MAP_SIZEX - 3)*0.5)*0.5;

	return retPoint;
}

bool GameScene::drawToRect(float y)	
{
	// �簢���� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ� .
	if (y <= MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1) ||
		y >= MAP_YSTART + HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1))
		return true;

	return false;
}

bool GameScene::drawToHexa(int x, int y)
{
	// �������� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ� .
	if (x + y >= MAP_SIZEX / 2 &&
		x + y <= MAP_SIZEX / 2 + MAP_SIZEY - 1)
		return false;

	return true;
}

Hexagon* GameScene::createHexagon(Point anchor, int size)
{
	// �߽����� ũ�⸦ ������, �׿� ���� �������� �� �������� ���Ϳ� �־ �����ϴ� �Լ� .
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
	// �����κ��� ���� ���� �����͸� field�� unit �� �����ϴ� �Լ�
	// ������ ������ ��, ȭ�鿡 ������ �׸���. �׸��� ���� ���� ���·� ��ȯ�Ѵ�.
	m_Game.getField()->setUnitData(unitData, length);

	for (int i = 0; i < MAX_UNIT_ON_GAME; ++i)
	{
		switch (unitData[i].unitOwner)				// ������ Owner �� �����Ͽ� �ùٸ� ��󿡰� ������ ����
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
		// i ��° ���� �������� x, y �ε�����
		Point screenPoint = conversionIndexToPoint(Point(xIndex, yIndex));
		// ȭ����� ��ǥ�� ��ȯ�ϰ� screenPoint �� ����

		Hexagon* hexa = createHexagon(screenPoint, 15);

		if (pField->getUnitData(i).unitOwner == UO_ME)
			m_UnitDrawNode[i]->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.5f, 0.8f), 1, ccc4f(0.0f, 1.0f, 0.2f, 0.3f));
		else
			m_UnitDrawNode[i]->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.5f, 0.0f, 0.0f, 0.8f), 1, ccc4f(0.0f, 1.0f, 0.2f, 0.3f));
		// ȭ����� ��ǥ�� �������� �׸�. ������ ���ο� ���� �ٸ� ��

		
		//  ���� ��������Ʈ �����ϴ� �Ϳ� ������ �־ drawPolygon ���� ��ü�� ������. ���� ���� ���

		m_UnitSprite[i] = Sprite::create("Player.png"); // ("WhithPawn.png"); <- �̹��� ���� ���?
		if (pField->getUnitData(i).unitType != UT_NONE)
		{
			int xIndex = pField->getUnitData(i).x;
			int yIndex = pField->getUnitData(i).y;
			// i ��° ���� �������� x, y �ε�����

			Point screenPoint = conversionIndexToPoint(Point(xIndex, yIndex));
			// ȭ����� ��ǥ�� ��ȯ�ϰ� screenPoint �� ����

			m_UnitSprite[i]->setPosition(screenPoint);
			// ȭ����� ��ǥ�� ������ ��ġ

			this->addChild(m_UnitSprite[i]);
		}
		
	}
}