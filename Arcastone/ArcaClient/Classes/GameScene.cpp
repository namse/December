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

	m_StartPoint =ScreenPoint(touch->getLocation());
	m_CursoredPoint = ScreenPoint(touch->getLocation());

	m_SelectedUnit = NON_SELECT_UNIT;

	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);

		// �ڽ��� �������� Ȯ���Ѵ�
		if (unit->getOwner() != UO_ME) continue;

		// �ڽ� ������ ��ǥ�ε����� ȭ��� ��ġ�� ��ȯ�Ͽ�
		ScreenPoint screenPoint = unit->getPosition().HexaToScreen();

		// �ڽ��� ������ Ŭ���ߴ��� �����Ѵ�.
		if (isInHexagon(m_StartPoint, screenPoint))
		{
			// �� ������ �����ߴٴ� ���� �����ϱ� ���� id �� ��������� �����Ѵ�.
			m_SelectedUnit = unit->getID();

			// ������ �̵� ��θ� �׷��ش�
			drawExpectUnitMove(unit);
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

	drawUnitMove(unit);
	// TODO : ��� �̷��� ���� �ʹ� ���� ����� �׸��� �ϴϱ� ��� �� ���ּ���
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;
	
	releaseExpectMoveSign();
	releaseMoveSign();
	m_CourseStack.clear();

	// Ŭ���� ���·� ���� �Ѿ�� ��� ���콺 ���� �� �ڵ� �ȵ��ư��� ����
	Unit* unit = getUnitByID(m_SelectedUnit);

	HexaDirection direction = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).getDirection();
	int distance = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).getRange();

	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();

	// ������ ����ϴ� ���
	if (distance == 0)
		return;

	if (DEBUG_PRINT_PACKET)
		printf("%f\n", direction);


	TcpClient::getInstance()->attackRequest(m_SelectedUnit, distance, direction);
	m_SelectedUnit = NON_SELECT_UNIT;
}

void GameScene::drawExpectUnitMove(Unit* unit)
{
	int unitAtk = unit->getAttack();
	HexaPoint unitPos = unit->getPosition();

	switch (unit->getMoveType())
	{
	case UMT_STRAIGHT:
	case UMT_JUMP:
	{
		for (int i = 1; i <= 6; ++i)
		{
			HexaDirection direction = (HexaDirection)i;
			for (int j = 1; j <= unitAtk; ++j)
			{
				HexaPoint movePoint = unitPos.getMovePoint(direction, j);

				Hexagon* expectSignHexagon = createHexagon(movePoint.HexaToScreen(), HEXAGON_LENGTH);
				DrawNode* expectSignNode = DrawNode::create();

				expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
				this->addChild(expectSignNode, 98);
				m_ExpectSignNode.push_back(expectSignNode);
			}
		}
	}break;

	case UMT_DASH:
	case UMT_TELEPORT:
	{
		for (int x = -unitAtk; x <= unitAtk; ++x)
		{
			for (int y = -unitAtk; y <= unitAtk; ++y)
			{
				if (abs(x + y) > unitAtk) continue;

				HexaPoint movePoint(unitPos.x+x, unitPos.y+y);

				Hexagon* expectSignHexagon = createHexagon(movePoint.HexaToScreen(), HEXAGON_LENGTH);
				DrawNode* expectSignNode = DrawNode::create();

				expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
				this->addChild(expectSignNode, 98);
				m_ExpectSignNode.push_back(expectSignNode);
			}
		}

	}break;

	}
}

void GameScene::releaseExpectMoveSign()
{
	for (auto node : m_ExpectSignNode)
	{
		this->removeChild(node);
	}
	m_ExpectSignNode.clear();
}


void GameScene::drawUnitMove(Unit* unit, HexaDirection direction, int range)
{
	Unit*			atkUnit = getUnitByID(m_SelectedUnit);
	UnitMoveType	atkType = atkUnit->getMoveType();
	HexaPoint		unitPos = unit->getPosition();
	int				unitAtk = unit->getAttack();
	Color4F			signcolor = (unit == atkUnit) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;

	// ������ ���ϴ� ��ġ ã��

	
	if (unit == atkUnit && direction == HD_NONE && range == 0)
	{
		// ���� Ÿ���� STARIGHT�ų� JUMP�̸�
		if ((atkType == UMT_STRAIGHT) || (atkType == UMT_JUMP))
		{
			direction = ((ScreenPoint)(m_StartPoint-m_CursoredPoint)).getDirection();
			range = ((ScreenPoint)(m_StartPoint-m_CursoredPoint)).getRange();
			if (range > unit->getMoveRange()) range = unit->getMoveRange();
			
		}
		// ���� Ÿ���� DASH�̸�
		else if (atkType == UMT_DASH)
		{
			// Ŀ���� ����Ű�� ��ġ �˻�
			HexaPoint cursor;
			bool isHexaGrid = false;
			for (auto hexaPos : m_HexagonPoint)
			{
				ScreenPoint tempPos = HexaPoint(hexaPos).HexaToScreen();
				if (isInHexagon(m_CursoredPoint,tempPos))
				{
					cursor = HexaPoint(hexaPos);
					isHexaGrid = true;
					break;
				}
			}

			if (!isHexaGrid) return;	// ���׸��� ������ �ƴϸ� OUT!
			if (cursor == atkUnit->getPosition())
			{
				// ���� ������ ����Ű�� ���� �ʱ�ȭ
				GameScene::releaseMoveSign();
				m_CourseStack.clear();
				return;
			}

			// ������ �������?
			if (m_CourseStack.empty())
			{
				if (!atkUnit->getPosition().isAround(cursor, 1))
				{
					return;	// ���� ���� �������� 1ĭ ������ ������ OUT!
				}

			}
			else
			{
				if (m_CourseStack.back() == cursor) return;	// ����Ű�� ��ġ�� ������ ������ OUT!

				if (getUnitByPos(m_CourseStack.back()) != nullptr)
				{
					// ������ ��ġ���� �浹������
					if (m_CourseStack.size() == 1)	return;
					m_CourseStack.pop_back();
				}

				if (!m_CourseStack.back().isAround(cursor, 1))
				{
					return;	// ���ÿ� ����� ������ ��� �������� 1ĭ ������ ������ OUT!
				}

				// ���� ���õ� ��ǥ�� �̹� ���ÿ� ������ֽ��ϱ�?
				vector<HexaPoint>::iterator i;
				for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
				{
					if (*i == cursor)
					{
						// ������ִٸ� �ű���� ������ ��ұ��� �����ع�������
						vector<HexaPoint>::iterator j;
						for (j = i; j != m_CourseStack.end();)
						{
							j = m_CourseStack.erase(j);
						}
						break;
					}
				}

				if (m_CourseStack.size() == unitAtk)
				{
					return;	// ���� ������ �� á���? OUT!
				}

			}

			m_CourseStack.push_back(cursor);
			range = m_CourseStack.size();
			if (range >= 2)
			{
				// �ڿ��� �ι�°�� �ִ� ��ҿ� ��
				HexaPoint prevPos = m_CourseStack.at(range-2);
				direction = prevPos.getDirection(cursor);
			}
			else if (m_CourseStack.size() == 1)
			{
				direction = atkUnit->getPosition().getDirection(cursor);
			}
		}
		else if (atkType == UMT_TELEPORT)
		{
			// Ŀ���� ����Ű�� ��ġ �˻�
			HexaPoint cursor;
			bool isHexaGrid = false;
			for (auto hexaPos : m_HexagonPoint)
			{
				ScreenPoint tempPos = HexaPoint(hexaPos).HexaToScreen();
				if (isInHexagon(m_CursoredPoint, tempPos))
				{
					cursor = HexaPoint(hexaPos);
					isHexaGrid = true;
					break;
				}
			}


			if (!m_CourseStack.empty() && m_CourseStack.back() == cursor) return;	// ����Ű�� ��ġ�� ������ ������ OUT!

			if (!unitPos.isAround(cursor, unitAtk)) return;	// �̵����� ����� OUT!

			if (cursor == atkUnit->getPosition())
			{
				// ���� ������ ����Ű�� ���� �ʱ�ȭ
				GameScene::releaseMoveSign();
				m_CourseStack.clear();
				return;
			}

			m_CourseStack.clear();
			m_CourseStack.push_back(cursor);
		}

		releaseMoveSign();
	}

	// ���������� �׷��ݴϴ�
	// �����̵� || �浹�� �з���
	if (atkType == UMT_STRAIGHT || unit != atkUnit)
	{
		assert(direction != HD_NONE);

		// �̵���� ǥ��
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint atkCourse = unitPos.getMovePoint(direction, i);

			// �ٴڿ� ���� �̵� ��ġ�� �׷��ش�
			drawMoveSign(atkCourse, signcolor);

			// �̵���ο� �ٸ� ������ �����ϴ°�?
			if (Unit* crashUnit = getUnitByPos(atkCourse))
			{
				// �׷� �ű⼭���� �ٸ� ������ �̵���θ� �׸����� �Ͻÿ�

				int atkRange;
				if (unit == atkUnit) // ù �浹�̸� ���� ������ ���ݷ� - �浿 ������ ���Ը�ŭ
					atkRange = unitAtk - crashUnit->getWeight();
				else // �ι�° ���Ŀ��� (���� �̵� ��ġ - ������� �׷��� �Ÿ�) ��ŭ
					atkRange = (range - i + 1);

				// ��������
				if (atkRange > 0) drawUnitMove(crashUnit, direction, atkRange);
				break;
			}

		}

	}
	// �پ�ѱ�
	else if (atkType == UMT_JUMP)
	{

		assert(direction != HD_NONE);
		// �̵���� ǥ��
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint atkCourse = unitPos.getMovePoint(direction, i);

			drawMoveSign(atkCourse, signcolor);

			// �̵���ο� �ٸ� ������ �����ϴ°�?
			if (Unit* crashUnit = getUnitByPos(atkCourse))
			{
				// Ȥ�� �� ������... E.N.E.M.Y ��?
				// �ƴ� Ȥ�� ���Ⱑ... range�� ��..?
				if ((crashUnit->getOwner() == UO_ENEMY) || (i == range))
				{
					// �׷���.. �� ��� �츮���� ������... �¶� ���Ĺ����ݴ�?
					// ��.. �ð��� �Ž��� �ö󰡺��ڲٳ�..
					// �׳����� ��.. ������ �ݴ����̶���.. 
					HexaPoint beforePosition = atkCourse.getMovePoint(direction, -1);

					// �ű� Ȥ�� �츮�� ���� �ֽ��ϱ�?
					if (nullptr == getUnitByPos(beforePosition) || range == 1)
					{
						// ����? �Ծ� �浹����������������
						int atkRange = unitAtk - crashUnit->getWeight();
						drawUnitMove(crashUnit, direction, atkRange);
						break;

					}
					else
					{
						// g�� �� ���⼭ �� �ϰ� �ִ� �ŴϤ�;
						// ��¿ �� ���� �� �ڱ����� ���߰ڴ�
						releaseMoveSign();
						if (range - 1 > 0) drawUnitMove(atkUnit, direction, range - 1);
						break;
					}
				}

			}

		}
	}
	else if (atkType == UMT_DASH)
	{
		// ���ÿ� ���ִ´�� �׷��ּ���!
		vector<HexaPoint>::iterator i;
		for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
		{
			drawMoveSign(*i, signcolor);
			// �浹��?
			if (Unit* crashUnit = getUnitByPos(*i))
			{
				int atkRange = unitAtk - crashUnit->getWeight();
				if (atkRange > 0) drawUnitMove(crashUnit, direction, atkRange);

				// �� �ں��� ���� �����
				vector<HexaPoint>::iterator j = ++i;
				for (; j != m_CourseStack.end();)
				{
					j = m_CourseStack.erase(j);
				}
				return;
			}
		}
	}
	else if (atkType == UMT_TELEPORT)
	{
		// �ѱ����� �׷��ݴϴ�
		// �ٸ� ������ ������ �̵��� �� ���� ���� ����������
		if (m_CourseStack.empty()) return;
		HexaPoint movePoint = m_CourseStack.at(0);

		signcolor = (getUnitByPos(movePoint) == nullptr) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;
		drawMoveSign(movePoint, signcolor);
	}



}

// �̵���θ� �׸��� �Լ�
void GameScene::drawMoveSign(HexaPoint point, Color4F signColor)
{
	Hexagon* courseSignHexagon = createHexagon(point.HexaToScreen(), HEXAGON_LENGTH);
	DrawNode* courseSignNode = DrawNode::create();

	courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, signColor, 1, signColor);
	this->addChild(courseSignNode, 99);
	m_CourseSignNode.push_back(courseSignNode);
}


// �׷����� �̵���θ� �ʱ�ȭ�ϴ� �Լ�
void GameScene::releaseMoveSign()
{
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();
}



// Ŭ���� ��ǥ�� Ư�� ������ �ȿ� ����ִ��� Ȯ���ϴ� �Լ�
bool GameScene::isInHexagon(ScreenPoint touch, ScreenPoint anchor)
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
		this->addChild(unitSprite, 100);
	}

	m_GameState = GS_GAME_START;
}


void GameScene::ReadActionQueue(Packet::AttackResult attackResult)
{
	while (!m_UnitActionQueue.empty()) 
		m_UnitActionQueue.pop();
	
	for (int i = 0; i < attackResult.mQueueLength; ++i)
	{
		m_UnitActionQueue.push(attackResult.mUnitActionQueue[i]);
	}

	onUnitAction();
}

void GameScene::ReadFieldBlock(FieldBlock fieldBlock[], int length)
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node, 100);
	for (int i = 0; i < length; i++)
	{
		int x = fieldBlock[i].m_Position.x;
		int y = fieldBlock[i].m_Position.y;
		CCLOG("%d, %d", x, y);
		ScreenPoint point = HexaPoint(x, y).HexaToScreen();

		m_HexagonPoint.push_back(Coord(x, y));	// m_HexagonPoint �� ȭ�鿡 �׷����� ��ǥ(0~x, 0~y)���� ����

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
		//this->addChild(fieldBlock, y);

		if (DRAW_HEXA_NUMBER) drawText(x, y, hexa);	// ���� �ȿ� ������ �ε��� ���� ������ ���ΰ�?

	}
}

void GameScene::onGameStart(Packet::GameStartResult inPacket)
{
	ReadFieldBlock(inPacket.mFieldList, inPacket.mFieldLength);
	ReadUnitData(inPacket.mUnit, inPacket.mUnitLength);
}

// sender �����ϼ�!
void GameScene::onUnitAction(CCNode* sender)
{
	if (!m_UnitActionQueue.empty())
	{
		// i ��° �׼��� ������
		UnitAction action = m_UnitActionQueue.front();
		m_UnitActionQueue.pop();

		// id�� ����Ͽ� i ��° �׼��� ������ ������ ã��
		Unit* unit = getUnitByID(action.mUnitId);

		switch (action.mActionType)
		{
		case UAT_MOVE:{
			HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
			ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

			if (DEBUG_PRINT_PACKET)
				printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

			// ������ �ε����� �̵�
			unit->setPosition(hMovePoint);

			// ��������Ʈ�� ��ǥ�� �̵�
			auto sprite = unit->getSprite();
			CCFiniteTimeAction* actionMove =
				CCMoveTo::create(MOVE_DURATION,
				sMovePoint);
			CCFiniteTimeAction* actionMoveDone =
				CCCallFuncN::create(this,
				callfuncN_selector(GameScene::onUnitAction));
			sprite->runAction(CCSequence::create(actionMove,
				actionMoveDone, NULL));
		}break;
		case UAT_JUMP:{
			HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
			ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

			if (DEBUG_PRINT_PACKET)
				printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

			unit->setPosition(hMovePoint);

			auto sprite = unit->getSprite();
			CCFiniteTimeAction* actionMove =
				CCMoveTo::create(MOVE_DURATION,
				sMovePoint);
			CCFiniteTimeAction* actionMoveDone =
				CCCallFuncN::create(this,
				callfuncN_selector(GameScene::onUnitAction));
			sprite->runAction(CCSequence::create(actionMove,
				actionMoveDone, NULL));
		}break;
		case UAT_DASH:{
			HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
			ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

			if (DEBUG_PRINT_PACKET)
				printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

			// ������ �ε����� �̵�
			unit->setPosition(hMovePoint);

			// ��������Ʈ�� ��ǥ�� �̵�
			auto sprite = unit->getSprite();
			CCFiniteTimeAction* actionMove =
				CCMoveTo::create(MOVE_DURATION,
				sMovePoint);
			CCFiniteTimeAction* actionMoveDone =
				CCCallFuncN::create(this,
				callfuncN_selector(GameScene::onUnitAction));
			sprite->runAction(CCSequence::create(actionMove,
				actionMoveDone, NULL));
		}break;
		case UAT_TELEPORT:{
			HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
			ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

			if (DEBUG_PRINT_PACKET)
				printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

			unit->setPosition(hMovePoint);

			// �ڷ���Ʈ�� ��! �ϰ� �̵�
			auto sprite = unit->getSprite();
			CCFiniteTimeAction* actionMove =
				CCMoveTo::create(0, sMovePoint);
		}break;
		case UAT_COLLISION:{

		}break;
		case UAT_DIE: {
			unit->getSprite()->setVisible(false);
		}
		}
	}
}



