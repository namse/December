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
    if ( !LayerColor::initWithColor(ccc4(255,255,255,225)))
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// ������ ���� ���¸� �α��� ������ ����
	m_GameState = GS_BEFORE_LOGIN;

	// ���콺 �̺�Ʈ�� ����ϱ� ���� �ʱ�ȭ
	touchEventInit();
	
	// �������� �׸�. Header.h �� #define ������ �����Ͽ� �������� ���� ���� ����
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
	if (!m_IsMyTurn) return false;			// �ڽ��� ���� ��쿡�� ���콺 �Է��� �޴´�.
		
	m_StartPoint.x = touch->getLocation().x;
	m_StartPoint.y = touch->getLocation().y;

	m_SelectedUnit = NON_SELECT_UNIT;
	
	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);

		// �ڽ��� �������� Ȯ���Ѵ�
		if (unit->getOwner() != UO_ME) continue;

		// �ڽ� ������ ��ǥ�ε����� ȭ��� ��ġ�� ��ȯ�Ͽ�
		ScreenPoint screenPoint = unit->getPosition().HexaToScreen();

		// �ڽ��� ������ Ŭ���ߴ��� �����Ѵ�.
		if (touchCheck(m_StartPoint, screenPoint))
		{
			// �� ������ �����ߴٴ� ���� �����ϱ� ���� id �� ��������� �����Ѵ�.
			m_SelectedUnit = unit->getID();
			return true;
		}
	}

	return false;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	// ���콺�� (�ε���������) �̵��ߴ��� Ȯ���ϰ� �̵��������� �׳� return;
	HexaPoint touchIndex(touch->getLocation().x, touch->getLocation().y);
	if (HexaPoint(m_CursoredPoint.ScreenToCoord()) == touchIndex) return;
	m_CursoredPoint = ScreenPoint(touch->getLocation());

	Unit* unit = getUnitByID(m_SelectedUnit);
	// ������ ������ �������� �ʾҰų�, ������ nullptr �̸� �н�
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;


	// ������ ���ϴ� ��ġ ã��
	HexaDirection direction = getPointToPointDirection(m_StartPoint, m_CursoredPoint);
	assert(direction != HD_NONE);

	int distance = getPointToPointDistance(m_StartPoint, m_CursoredPoint);
	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();

	// �̵� ��θ� �׷��ݴϴ�

	// ǥ�õ� �̵���� �ʱ�ȭ
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();
	drawUnitMove(unit, direction, distance);

}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();


	// Ŭ���� ���·� ���� �Ѿ�� ��� ���콺 ���� �� �ڵ� �ȵ��ư��� ����

	Unit* unit = getUnitByID(m_SelectedUnit);

	HexaDirection direction = getPointToPointDirection(m_StartPoint, m_CursoredPoint);
	int distance = getPointToPointDistance(m_StartPoint, m_CursoredPoint);

	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();

	// ������ ����ϴ� ���
	if (distance == 0)
		return;
 
	if (DEBUG_PRINT_PACKET) 
		printf("%f\n", direction);


	TcpClient::getInstance()->attackRequest(m_SelectedUnit, distance, direction);
	m_SelectedUnit = NON_SELECT_UNIT;
}


void GameScene::drawUnitMove(Unit* unit, HexaDirection direction, int range)
{

	auto signcolor = (unit->getID() == m_SelectedUnit) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;

	// �̵���� ǥ��
	for (int i = 1; i <= range; ++i)
	{
		HexaPoint attackCourse = getPointMoveDirection(unit->getPosition(), direction, i);

		// �̵���ο� �ٸ� ������ �����ϴ°�?
		if (Unit* crashUnit = getUnitByPos(attackCourse))
		{
			// �׷� �ű⼭���� �ٸ� ������ �̵���θ� �׸����� �Ͻÿ�

			int attackRange;
			if (unit->getID() == m_SelectedUnit) // ù �浹�̸� ���� ������ ���ݷ� - �浿 ������ ���Ը�ŭ
				attackRange = unit->getAttack() - crashUnit->getWeight();
			else // �ι�° ���Ŀ��� (���� �̵� ��ġ - ������� �׷��� �Ÿ�) ��ŭ
				attackRange = (range - i + 1);

			// ��������
			drawUnitMove(crashUnit, direction, attackRange);
			break;
		}

		// �������� ������, �ٴڿ� ���� �̵� ��ġ�� �׷��ش�
		Hexagon* courseSignHexagon = createHexagon(attackCourse.HexaToScreen(), HEXAGON_LENGTH);
		DrawNode* courseSignNode = DrawNode::create();

		courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, signcolor, 1, signcolor);
		this->addChild(courseSignNode,99);
		m_CourseSignNode.push_back(courseSignNode);
	}
	

}

// Ŭ���� ��ǥ�� Ư�� ������ �ȿ� ����ִ��� Ȯ���ϴ� �Լ�
bool GameScene::touchCheck(ScreenPoint touch, ScreenPoint anchor)
{
	float msin = sin(RADIANS_60);

	// Ŭ���� ��ǥ�� �������� x ������ ���ϴ��� Ȯ��
	if (touch.y > anchor.y - HEXAGON_LENGTH * msin &&
		touch.y < anchor.y + HEXAGON_LENGTH * msin)
	{

		float yHowFarToAnchor = abs(touch.y - anchor.y);

		// �������� �߽ɿ��� y �Ÿ��� �־������� x ������ �������� ���� �����ϴ� �ڵ�
		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;

		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

// ������ ã���ϴ�
Unit* GameScene::getUnitByID(int unitID)
{
	for (auto unit : m_UnitList)
	{
		if (unitID == unit->getID())
			return unit;
	}
	return nullptr; // �˻��� �����ϸ� null return
}

Unit* GameScene::getUnitByPos(HexaPoint unitPos)
{
	for (auto unit : m_UnitList)
	{
		if (unitPos == unit->getPosition())
			return unit;
	}
	return nullptr; // �˻��� �����ϸ� null return
}

// �� Point ������ Direction �� ���ϴ� �Լ�
HexaDirection GameScene::getPointToPointDirection(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float degree = CC_RADIANS_TO_DEGREES(atan2(y, x));
 
	// 3,4�и�� 270������ ������ǥ�� ����
	if ((x < 0 && y < 0) || (x >= 0 && y < 0))
		degree += 360;

	// ���׸��� ��ǥ�� ��ȯ
	HexaDirection Direction = (HexaDirection)((int)(degree / 60) + 1);

	return Direction;
}

// �� Point ������ Distance �� ���ϴ� �Լ�
int GameScene::getPointToPointDistance(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	return (int)(sqrt(x*x + y*y))/30;
}

void GameScene::drawHexaGrid()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node,100);

	for (int i = 0; i < MAP_SIZEX; ++i)
	{
		for (int j = 0; j < MAP_SIZEY; ++j)
		{
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// ���������� �׸��� ���� ���ǹ�
				continue;

			ScreenPoint point = HexaPoint(i, j).HexaToScreen();

			if (drawToRect(point.y) && MAP_IS_RECT)	// �簢������ �׸��� ���� ���ǹ�
				continue;

			m_HexagonPoint.push_back(Coord(i, j));	// m_HexagonPoint �� ȭ�鿡 �׷����� ��ǥ(0~x, 0~y)���� ����

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, COLOR_OF_GRID);

			// �ӽ÷� ��� �̹��� �׷���
			Sprite* fieldBlock;
			switch (rand() % 3)
			{
			case 0: fieldBlock = Sprite::create("block1.png");  break;
			case 1: fieldBlock = Sprite::create("block2.png");  break;
			case 2: fieldBlock = Sprite::create("block3.png");  break;
			}
			 ;
			float scale = HEXAGON_LENGTH * 2 / fieldBlock->getContentSize().width;
			fieldBlock->setScale(scale);
			fieldBlock->setAnchorPoint(Vec2(0.5f, 0.67f));
			fieldBlock->setPosition(point);
			this->addChild(fieldBlock,j);
			
			if(DRAW_HEXA_NUMBER) drawText(i, j, hexa);	// ���� �ȿ� ������ �ε��� ���� ������ ���ΰ�?
		}
	}
}

// ������ �ȿ� ������ �ε��� ���� �����ִ� �Լ� .
// DRAW_HEXA_POSITION �� true�� �ε��� ���� �ƴ� ��ġ ���� ������
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


// �簢���� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ� .
bool GameScene::drawToRect(float y)	
{
	if (y <= MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1) ||
		y >= MAP_YSTART + HEXAGON_LENGTH * sin(RADIANS_60) * (MAP_SIZEY - MAP_SIZEX*0.5 + 1))
		return true;

	return false;
}

// �������� �ǵ��� �׷��ִ� �κ��� �����ϱ� ���� �Լ� .
bool GameScene::drawToHexa(int x, int y)
{
	if (x + y >= MAP_SIZEX / 2 &&
		x + y <= MAP_SIZEX / 2 + MAP_SIZEY - 1)
		return false;

	return true;
}

// �߽����� ũ�⸦ ������, �׿� ���� �������� �� �������� ���Ϳ� �־ �����ϴ� �Լ� .
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

// �����κ��� ���� ���� �����͸� field�� unit �� �����ϴ� �Լ�
// ������ ������ ��, ȭ�鿡 ������ �׸���. �׸��� ���� ���� ���·� ��ȯ�Ѵ�.
void GameScene::ReadUnitData(UnitData unitData[], int length)
{
	m_UnitList.reserve(length);

	for (int i = 0; i < length; ++i)
	{
		Unit* unit = Unit::create(unitData[i]);

		m_UnitList.push_back(unit);

		// ������ ��ġ�� ȭ����� ��ǥ�� ��ȯ�ϰ� screenPoint �� ����
		Sprite* unitSprite = unit->getSprite();
		ScreenPoint unitRealPoint = unit->getPosition().HexaToScreen();

		// ȭ����� ��ǥ�� ������ ��ġ
		assert(unitSprite != nullptr);
		unitSprite->setPosition(Point(unitRealPoint.x, unitRealPoint.y));
		this->addChild(unitSprite,100);
	}

	m_GameState = GS_GAME_START;
}


void GameScene::ReadActionQueue(UnitAction unitActionQueue[], int length)
{
	for (int i = 0; i < length; ++i)
	{
		// i ��° �׼��� ������
		UnitAction action = unitActionQueue[i];

		// id�� ����Ͽ� i ��° �׼��� ������ ������ ã��
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

				// ������ �ε����� �̵�
				pUnit->setPosition(hMovePoint);
				// ��������Ʈ�� ��ǥ�� �̵�
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
