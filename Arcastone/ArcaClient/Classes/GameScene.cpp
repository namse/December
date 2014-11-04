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

	// ������ ���� ���¸� �α��� ������ ����
	m_GameState = GS_BEFORE_LOGIN;

	// ���콺 �̺�Ʈ�� ����ϱ� ���� �ʱ�ȭ
	touchEventInit();

	m_TouchDrawNode = CCDrawNode::create();
	addChild(m_TouchDrawNode);

	// �������� �׸�. Header.h �� #define ������ �����Ͽ� �������� ���� ���� ����
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

	// ���� ����
	m_TouchDrawNode->clear();
	
	ScreenPoint clickPoint(touch->getLocation().x, touch->getLocation().y);

	m_SelectedUnitIndex = NON_SELECT_UNIT;

	Player* pPlayer = m_Game.getPlayer(PW_PLAYERONE);
	for (int i = 0; i < pPlayer->getUnitCounter(); ++i)
	{
		// �ڽ��� ���ֵ��� ��ǥ�ε����� �ϳ��� �����ϰ�
		HexaPoint unitPoint = pPlayer->getUnit(i)->getUnitStatus().point;
		// ��ǥ�ε����� ȭ��� ��ġ�� ��ȯ�Ͽ�
		ScreenPoint screenPoint = conversionIndexToPoint(unitPoint);
		// �ڽ��� ������ Ŭ���ߴ��� �����Ѵ�.
		if (touchCheck(clickPoint, screenPoint))
		{
			Point point(screenPoint.x, screenPoint.y);
			m_TouchDrawNode->drawDot(point, 18, ccc4f(0.7, 0.7, 0.7, 0.7));

			// �� ������ �����ߴٴ� ���� �����ϱ� ���� id �� ��������� �����Ѵ�.
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

	// ������ ������ �������� �ʾҰų�, ������ nullptr �̸� �н�
	if (m_SelectedUnitIndex == NON_SELECT_UNIT || unit == nullptr) return;

	// ���콺 �巡�� �� �ӵ� ���� �߻��� ���,
	// ���� ���콺�� ������ index �� �����صΰ�, �� index ���� �ٲ� ���� �Ʒ��� for ���� ȸ���� �� .

	// ������ ���� ����
	m_TouchDrawNode->clear();

	ScreenPoint clickPoint(touch->getLocation().x, touch->getLocation().y);

	for (int i = 0; i < m_HexagonPoint.size(); ++i)
	{
		// ������ ������ġ�� �����Ͽ� ���� ���콺�� ��� �ε����� �ִ��� �˾Ƴ�
		HexaPoint hexaPoint(m_HexagonPoint.at(i));
		ScreenPoint screenPoint = conversionIndexToPoint(hexaPoint);

		// ã�Ƴ´�! ���콺�� i��° �׸��忡 �־�!
		if (touchCheck(clickPoint, screenPoint))
		{
			HexaPoint unitPoint = unit->getUnitStatus().point;

			// ������ ���ֺ��� ���콺�� �ִ� ���ڱ��� ���� ����
			ScreenPoint pointFrom = conversionIndexToPoint(unitPoint);
			ScreenPoint pointTo = screenPoint;

			Point pF(pointFrom.x, pointFrom.y);
			Point pT(pointTo.x, pointTo.y);
			m_TouchDrawNode->drawSegment(pF, pT, 10, ccc4f(0.7, 0.7, 0.7, 0.7));

			// ���콺�� �ִ� ������ ��ġ ����
			m_CursoredPoint = screenPoint;
			return;
		}
	}
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	// Ŭ���� ���·� ���� �Ѿ�� ��� ���콺 ���� �� �ڵ� �ȵ��ư��� ����

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

	// ������ ����ϴ� ���
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

// �� Point ������ Direction �� ���ϴ� �Լ�
float GameScene::getPointToPointDirection(ScreenPoint point1, ScreenPoint point2)
{
	float x = point1.x - point2.x;
	float y = point1.y - point2.y;
	float r = sqrt(x*x + y*y);
	return acos(x / r);
}

// �� Point ������ Distance �� ���ϴ� �Լ�
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
			if (drawToHexa(i, j) && MAP_IS_HEXA)	// ���������� �׸��� ���� ���ǹ�
				continue;

			ScreenPoint point = conversionIndexToPoint(HexaPoint(i, j));

			if (drawToRect(point.y) && MAP_IS_RECT)	// �簢������ �׸��� ���� ���ǹ�
				continue;

			m_HexagonPoint.push_back(Coord(i, j));	// m_HexagonPoint �� ȭ�鿡 �׷����� ��ǥ(0~x, 0~y)���� ����

			Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, ccc4f(0.2f, 1.0f, 0.2f, 0.3f));
			
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

// �Է��� index ������ point ���� ȭ�鿡 �׸� �� �ִ� point ������ ��ȯ���ִ� �Լ� .
ScreenPoint GameScene::conversionIndexToPoint(HexaPoint point)
{
	ScreenPoint retPoint;

	// MAP_START �� �߾ӿ� ��ġ�ϵ��� �׷��ֱ� ���� ���ĵ� .
	retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (point.x - (MAP_SIZEX - 1)*0.5);
	retPoint.y = MAP_YSTART - HEXAGON_LENGTH * sin(RADIANS_60) * (point.y * 2 - MAP_SIZEY + point.x - (MAP_SIZEX - 3)*0.5);

	return retPoint;
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
			// i ��° ���� �������� x, y �ε�����
			HexaPoint unitPoint(unitData.point.x, unitData.point.y);

			// ȭ����� ��ǥ�� ��ȯ�ϰ� screenPoint �� ����
			ScreenPoint unitRealPoint = conversionIndexToPoint(unitPoint);

			// ȭ����� ��ǥ�� ������ ��ġ
			m_UnitSprite[i]->setPosition(Point(unitRealPoint.x, unitRealPoint.y));

			this->addChild(m_UnitSprite[i]);
		}
		
	}
}