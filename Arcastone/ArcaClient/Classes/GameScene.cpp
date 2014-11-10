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
    if ( !LayerColor::initWithColor(ccc4(255,255,255,225)))
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
	//if (!m_IsMyTurn) return false;			// 자신의 턴인 경우에만 마우스 입력을 받는다.

	m_StartPoint.x = touch->getLocation().x;
	m_StartPoint.y = touch->getLocation().y;

	m_SelectedUnit = NON_SELECT_UNIT;

	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);

		// 자신의 유닛인지 확인한다
		if (unit->getOwner() != UO_ME) continue;

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
	//if (!m_IsMyTurn) return;

	// 마우스가 (인덱스단위로) 이동했는지 확인하고 이동안했으면 그냥 return;
	HexaPoint touchIndex(touch->getLocation().x, touch->getLocation().y);
	if (HexaPoint(m_CursoredPoint.ScreenToCoord()) == touchIndex) return;
	m_CursoredPoint = ScreenPoint(touch->getLocation());

	Unit* unit = getUnitByID(m_SelectedUnit);
	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;

	drawUnitMove(unit);
	// TODO : 요거 이렇게 가면 너무 자주 지우고 그리고 하니까 어떻게 좀 해주세요
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	//if (!m_IsMyTurn) return;

	GameScene::releaseMoveSign();
	m_CourseStack.clear();

	// 클릭한 상태로 턴이 넘어갔을 경우 마우스 떼도 이 코드 안돌아가니 주의
	Unit* unit = getUnitByID(m_SelectedUnit);

	HexaDirection direction = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).getDirection();
	int distance = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).getRange();

	if (distance > unit->getMoveRange()) distance = unit->getMoveRange();

	// 공격을 취소하는 경우
	if (distance == 0)
		return;

	if (DEBUG_PRINT_PACKET)
		printf("%f\n", direction);


	TcpClient::getInstance()->attackRequest(m_SelectedUnit, distance, direction);
	m_SelectedUnit = NON_SELECT_UNIT;
}


void GameScene::drawUnitMove(Unit* unit, HexaDirection direction, int range)
{
	Unit* atkUnit = getUnitByID(m_SelectedUnit);
	UnitMoveType atkType = atkUnit->getMoveType();
	HexaPoint unitPos = unit->getPosition();
	Color4F signcolor = (unit == atkUnit) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;

	// 공격이 향하는 위치 찾기
	if (unit == atkUnit && direction == HD_NONE && range == 0)
	{
		// 공격 타입이 STARIGHT거나 JUMP이면
		if ((atkType == UMT_STRAIGHT) || (atkType == UMT_JUMP))
		{
			direction = ((ScreenPoint)(m_StartPoint-m_CursoredPoint)).getDirection();
			range = ((ScreenPoint)(m_StartPoint-m_CursoredPoint)).getRange();
			if (range > unit->getMoveRange()) range = unit->getMoveRange();
			
		}
		// 공격 타입이 DASH이면
		else if (atkType == UMT_DASH)
		{
			// 커서가 가리키는 위치 검색
			HexaPoint cursor;
			bool isHexaGrid = false;
			for (auto hexaPos : m_HexagonPoint)
			{
				ScreenPoint tempPos = HexaPoint(hexaPos).HexaToScreen();
				if (touchCheck(m_CursoredPoint,tempPos))
				{
					cursor = HexaPoint(hexaPos);
					isHexaGrid = true;
					break;
				}
			}

			if (!isHexaGrid) return;	// 헥사그리드 범위가 아니면 OUT!
			if (cursor == atkUnit->getPosition())
			{
				// 공격 유닛을 가리키면 스택 초기화
				GameScene::releaseMoveSign();
				m_CourseStack.clear();
				return;
			}

			// 스택이 비었나요?
			if (m_CourseStack.size() == 0)
			{
				if (!atkUnit->getPosition().isAround(cursor, 1))
				{
					return;	// 공격 유닛 기준으로 1칸 범위에 없으면 OUT!
				}

			}
			else
			{
				if (m_CourseStack.back() == cursor) return;	// 가리키는 위치에 변경이 없으면 OUT!

				if (getUnitByPos(m_CourseStack.back()) != nullptr)
				{
					// 마지막 위치에서 충돌했으면
					if (m_CourseStack.size() == 1)	return;
					m_CourseStack.pop_back();
				}

				if (!m_CourseStack.back().isAround(cursor, 1))
				{
					return;	// 스택에 저장된 마지막 요소 기준으로 1칸 범위에 없으면 OUT!
				}

				// 지금 선택된 좌표가 이미 스택에 저장돼있습니까?
				vector<HexaPoint>::iterator i;
				for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
				{
					if (*i == cursor)
					{
						// 저장돼있다면 거기부터 마지막 요소까지 삭제해버리세요
						vector<HexaPoint>::iterator j;
						for (j = i; j != m_CourseStack.end();)
						{
							j = m_CourseStack.erase(j);
						}
						break;
					}
				}

				if (m_CourseStack.size() == atkUnit->getAttack())
				{
					return;	// 설마 스택이 꽉 찼어요? OUT!
				}

			}

			m_CourseStack.push_back(cursor);
			range = m_CourseStack.size();
			if (range >= 2)
			{
				// 뒤에서 두번째에 있는 요소와 비교
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
			// TODO : 아.. todo 주석 달기도 귀찮다... ((삶의 의지를 잃은 것 같다))
		}

		releaseMoveSign();
	}

	// 본격적으로 그려줍니다
	// 직선이동 || 충돌후 밀려남
	if (atkType == UMT_STRAIGHT || unit != atkUnit)
	{
		assert(direction != HD_NONE);

		// 이동경로 표시
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint attackCourse = unitPos.getMovePoint(direction, i);

			// 바닥에 예상 이동 위치를 그려준다
			drawMoveSign(attackCourse, signcolor);

			// 이동경로에 다른 유닛이 존재하는가?
			if (Unit* crashUnit = getUnitByPos(attackCourse))
			{
				// 그럼 거기서부터 다른 유닛의 이동경로를 그리도록 하시오

				int attackRange;
				if (unit == atkUnit) // 첫 충돌이면 현재 유닛의 공격력 - 충동 유닛의 무게만큼
					attackRange = unit->getAttack() - crashUnit->getWeight();
				else // 두번째 이후에는 (예상 이동 위치 - 현재까지 그려준 거리) 만큼
					attackRange = (range - i + 1);

				// 움직이자
				if (attackRange > 0) drawUnitMove(crashUnit, direction, attackRange);
				break;
			}

		}

	}
	// 뛰어넘기
	else if (atkType == UMT_JUMP)
	{

		assert(direction != HD_NONE);
		// 이동경로 표시
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint attackCourse = unitPos.getMovePoint(direction, i);

			drawMoveSign(attackCourse, signcolor);

			// 이동경로에 다른 유닛이 존재하는가?
			if (Unit* crashUnit = getUnitByPos(attackCourse))
			{
				// 혹시 네 주인이... E.N.E.M.Y 니?
				// 아님 혹시 여기가... range의 끝..?
				if ((crashUnit->getOwner() == UO_ENEMY) || (i == range))
				{
					// 그런데.. 이 길목에 우리편이 있으면... 걔랑 겹쳐버리잖니?
					// 자.. 시간을 거슬러 올라가보자꾸나..
					// 그나저나 얘.. 방향은 반대쪽이란다.. 
					HexaPoint beforePosition = attackCourse.getMovePoint(direction, -1);

					// 거기 혹시 우리집 유닛 있습니까?
					if (nullptr == getUnitByPos(beforePosition) || range == 1)
					{
						// 없음? 님아 충돌여ㅋㅋㅋㅋㅋㅋㅋ
						int attackRange = unit->getAttack() - crashUnit->getWeight();
						drawUnitMove(crashUnit, direction, attackRange);
						break;

					}
					else
					{
						// g헐 얘 여기서 뭘 하고 있는 거니ㅠ;
						// 어쩔 수 없지 얘 뒤까지만 가야겠다
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
		// 스택에 들어가있는대로 그려주세요!
		vector<HexaPoint>::iterator i;
		for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
		{
			drawMoveSign(*i, signcolor);
			// 충돌함?
			if (Unit* crashUnit = getUnitByPos(*i))
			{
				int attackRange = unit->getAttack() - crashUnit->getWeight();
				if (attackRange > 0) drawUnitMove(crashUnit, direction, attackRange);

				// 이 뒤부터 스택 비워줌
				vector<HexaPoint>::iterator j = ++i;
				for (; j != m_CourseStack.end();)
				{
					j = m_CourseStack.erase(j);
				}
				return;
			}
		}
	}



}

// 예상좌표를 그리는 함수
void GameScene::drawMoveSign(HexaPoint point, Color4F signColor)
{
	Hexagon* courseSignHexagon = createHexagon(point.HexaToScreen(), HEXAGON_LENGTH);
	DrawNode* courseSignNode = DrawNode::create();

	courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, signColor, 1, signColor);
	this->addChild(courseSignNode, 99);
	m_CourseSignNode.push_back(courseSignNode);
}


// 그려놓은 예상좌표를 초기화하는 함수
void GameScene::releaseMoveSign()
{
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();
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
Unit* GameScene::getUnitByID(int unitID)
{
	for (auto unit : m_UnitList)
	{
		if (unitID == unit->getID())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
}

Unit* GameScene::getUnitByPos(HexaPoint unitPos)
{
	for (auto unit : m_UnitList)
	{
		if (unitPos == unit->getPosition())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
}

void GameScene::drawHexaGrid()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node, 100);

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
			node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, COLOR_OF_GRID);

			// 임시로 블록 이미지 그려줌
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
			this->addChild(fieldBlock, j);

			if (DRAW_HEXA_NUMBER) drawText(i, j, hexa);	// 헥사곤 안에 정수형 인덱스 값을 보여줄 것인가?
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
		assert(unitSprite != nullptr);
		unitSprite->setPosition(Point(unitRealPoint.x, unitRealPoint.y));
		this->addChild(unitSprite, 100);
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
