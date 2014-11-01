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
    if ( !LayerColor::initWithColor(ccc4(255,255,255,255)))
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	TcpClient::getInstance()->loginRequest();
	// ������ �α��� �ϰ� ����� !!

	// TODO : ������ ���� ��ġ( ���� ���� ) �ּ��� !!

	// TODO : ������ ���� �������� �˷���� !!

	touchEventInit();

	Game::getInstance();

	drawHexagon();
    
	this->schedule(schedule_selector(GameScene::gameLogic), 0.0f);
	// ���� ���� ����

	this->setKeyboardEnabled(true);

    return true;
}

void GameScene::gameLogic(float dt)
{
	
}

void GameScene::touchEventInit()
{
	m_PointPathIndex = 0;
	m_IsClicked = false;

	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(true);

	_touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);

	EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();

	dispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	while (m_PointPath.size() > 0)
		m_PointPath.pop_back();						// �� ����

	Point clickPoint = touch->getLocation();
	m_PointPath.push_back(clickPoint);				// ���� ����

	for (int i = 0; i < m_HexagonPoint.size(); ++i)
	{
		Point point = pointConversion(m_HexagonPoint.at(i));	// i ��° �ε����� ȭ��� ��ġ�� point�� ����

		if (touchCheck(clickPoint, point))
		{
			LabelTTF* vLabelx;
			vLabelx = LabelTTF::create("CLICK!", "Hevetica", 20);
			vLabelx->setPosition(Point(point.x, point.y));
			vLabelx->setColor(Color3B(0, 255, 0));

			this->addChild(vLabelx);
		}
	}

	m_IsClicked = true;

	return true;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	Point clickPoint = touch->getLocation();

	for (int i = 0; i < m_HexagonPoint.size(); ++i)
	{
		Point point = pointConversion(m_HexagonPoint.at(i));	// i ��° �ε����� ȭ��� ��ġ�� point�� ����

		if (touchCheck(clickPoint, point))
		{
			LabelTTF* vLabelx;
			vLabelx = LabelTTF::create("DRAG!", "Hevetica", 20);
			vLabelx->setPosition(Point(point.x, point.y));
			vLabelx->setColor(Color3B(0, 255, 0));

			this->addChild(vLabelx);
		}
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
		// yHowFarToAnchor : �������� �ܽɿ��� Ŭ���� ��ǥ�� y �������� ������ ����

		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;
		// �������� �߽ɿ��� y �Ÿ��� �־������� x ������ �������� ���� �����ϱ� ���ؼ� yTuningValue �� ������� .
		
		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	m_PointPathIndex = 0;

	m_IsClicked = false;
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

			point = pointConversion(Point(i, j));

			if (drawToRect(point.y) && MAP_IS_RECT)	// �簢������ �׸��� ���� ���ǹ�
				continue;

			m_HexagonPoint.push_back(Point(i, j));	// m_HexagonPoint �� ȭ�鿡 �׷����� ��ǥ(0~x, 0~y)���� ����

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.8f, 0.0f, 0.8f, 0.0f), 1, ccc4f(0.8f, 0.0f, 0.0f, 0.8f));

			if(DRAW_HEXA_NUMBER) drawText(i, j, hexa);
		}
	}
}

void GameScene::drawText(int i, int j, Hexagon* hexa)
{
	// ������ �ȿ� ������ �ε��� ���� �����ִ� �Լ�
	int f = i;
	char szBuf1[8];
	itoa(f, szBuf1, 10);

	f = j;
	char szBuf2[8];
	itoa(f, szBuf2, 10);

	LabelTTF* vLabelx;
	LabelTTF* vLabely;
	vLabelx = LabelTTF::create(szBuf1, "Hevetica", 11);
	vLabely = LabelTTF::create(szBuf2, "Hevetica", 11);

	vLabelx->setPosition(Point(hexa->anchor.x - 6, hexa->anchor.y));
	vLabely->setPosition(Point(hexa->anchor.x + 6, hexa->anchor.y));

	vLabelx->setColor(Color3B(0, 0, 0));
	vLabely->setColor(Color3B(0, 0, 255));

	this->addChild(vLabelx);
	this->addChild(vLabely);
}

Point GameScene::pointConversion(Point point)
{
	// �Է��� index ������ point ���� ȭ�鿡 �׸� �� �ִ� point ������ ��ȯ���ִ� �Լ��� .
	Point retPoint;

	retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * point.x;
	retPoint.y = MAP_YSTART - HEXAGON_LENGTH * 2 * sin(RADIANS_60) * (point.y + point.x*0.5 - MAP_SIZEY*0.5);

	return retPoint;
}

bool GameScene::drawToRect(float y)	
{
	// �簢���� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ��̴� .
	if (y <= MAP_YSTART - HEXAGON_LENGTH * 2 * sin(RADIANS_60) * (MAP_SIZEY*0.5) ||
		y >= MAP_YSTART - HEXAGON_LENGTH * 2 * sin(RADIANS_60) * (MAP_SIZEX*0.5 - MAP_SIZEY*0.5 - 1))
		return true;

	return false;
}

bool GameScene::drawToHexa(int x, int y)
{
	// �������� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ��̴� .
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