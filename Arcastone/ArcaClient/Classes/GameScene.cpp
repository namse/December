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
	}break;

	case GS_WAIT_GAME:
	{

	}break;

	case GS_GAME_START:			// GameStartResult 를 받은 상태
	{
		//if (m_IsAction && m_IsMyTurn) TcpClient::getInstance()->TurnEndRequest();
	}break;
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

	m_StartPoint = ScreenPoint(touch->getLocation());
	m_CursoredPoint = ScreenPoint(touch->getLocation());

	m_SelectedUnit = NON_SELECT_UNIT;

	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);

		// 자신의 유닛인지 확인한다
		if (unit->GetOwner() != UO_ME) continue;

		// 자신 유닛의 좌표인덱스를 화면상 위치로 변환하여
		ScreenPoint screenPoint = unit->GetPosition().HexaToScreen();

		// 자신의 유닛을 클릭했는지 판정한다.
		if (isInHexagon(m_StartPoint, screenPoint))
		{
			// 그 유닛을 선택했다는 것을 지정하기 위해 id 를 멤버변수에 저장한다.
			m_SelectedUnit = unit->GetID();

			// 유닛이 이동가능한 헥사곤을 하이라이트한다
			DrawExpectUnitMove(unit);
			return true;
		}
	}

	return false;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	if (!m_IsMyTurn) return;

	Unit* unit = getUnitByID(m_SelectedUnit);
	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;

	// 어딜 터치했는지 찾아서
	HexaPoint touchIndex;
	touchIndex = ScreenToHexa(ScreenPoint(touch->getLocation().x, touch->getLocation().y));
	
	HexaPoint cursoredPoint;
	cursoredPoint = ScreenToHexa(m_CursoredPoint);

	// 터치한 위치가 (인덱스단위로) 이동했는지 확인하고 이동안했으면 그냥 return;
	if (touchIndex == cursoredPoint) return;

	m_CursoredPoint = ScreenPoint(touch->getLocation());

	// TODO : 요거 이렇게 가면 너무 자주 지우고 그리고 하니까 어떻게 좀 해주세요
	drawUnitMove(unit, HD_NONE, 0, true);
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	{
		// 이동할 수 있는 모든 범위 표시 지움
		releaseExpectMoveSign();
		// 그려진 이동 경로 지움
		releaseMoveSign();
	}

	if (!m_IsMyTurn) return;

	Unit* unit = getUnitByID(m_SelectedUnit);
	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;

	HexaDirection direction = HD_NONE;
	int distance = 0;

	AttackData attackData;

	switch (unit->GetMoveType())
	{
	case UMT_STRAIGHT:
	case UMT_JUMP:{
		// 스트레이트와 점프는 방향과 거리를 입력
		direction = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).GetDirection();
		distance = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).GetRange();

		if (distance > unit->GetMoveRange()) distance = unit->GetMoveRange();

		// 공격을 취소하는 경우
		if (distance == 0)
			return;
	}break;

	case UMT_DASH:{
		for (int i = 0; i < m_CourseStack.size(); ++i)
		{
			// 대쉬는 이동 스택을 입력
			attackData.position[i] = m_CourseStack.at(i).HexaToCoord();
		}
	}break;

	case UMT_TELEPORT:{
		// 텔포는 이동 칸 하나 입력
		attackData.position[0] = m_CourseStack.at(0).HexaToCoord();
	}break;

	default:
		assert(false && "drawUnitMove -> not defined New attackType !");
	}

	m_CourseStack.clear();

	attackData.direction = direction;
	attackData.Range = distance;

	TcpClient::getInstance()->attackRequest(attackData);
	m_SelectedUnit = NON_SELECT_UNIT;
	
}

void GameScene::DrawExpectUnitMove(Unit* unit)
{
	int unitMoveRange = unit->GetMoveRange();
	HexaPoint unitPos = unit->GetPosition();

	switch (unit->GetMoveType())
	{
		case UMT_STRAIGHT:
		case UMT_JUMP:
		{
			for (int i = 1; i <= 6; ++i)
			{
				HexaDirection direction = (HexaDirection)i;
				for (int j = 1; j <= unitMoveRange; ++j)
				{
					HexaPoint movePoint = unitPos.GetMovePoint(direction, j);
					ScreenPoint realMovePoint = movePoint.HexaToScreen();

					HighlightHexagon(realMovePoint);
				}
			}
		}break;

		case UMT_DASH:
		case UMT_TELEPORT:
		{
			for (int x = -unitMoveRange; x <= unitMoveRange; ++x)
			{
				for (int y = -unitMoveRange; y <= unitMoveRange; ++y)
				{
					if (abs(x + y) > unitMoveRange) continue;

					HexaPoint movePoint(unitPos.x + x, unitPos.y + y);

					Hexagon* expectSignHexagon = createHexagon(movePoint.HexaToScreen(), HEXAGON_LENGTH);
					DrawNode* expectSignNode = DrawNode::create();

					expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
					this->addChild(expectSignNode, 98);
					m_ExpectSignNode.push_back(expectSignNode);
				}
			}

		}break;
		default:
			assert(false && "drawUnitMove -> not defined New attackType !");
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

// 재귀함수로 그려준다. 마지막 인자는 마우스를 이동해서 호출한건지 확인
void GameScene::drawUnitMove(Unit* unit, HexaDirection direction, int range, bool isFirstCall)
{
	// 인자로 들어온 unit 은 어떻게 이동하는지 현재 확인할 유닛
	Unit*			atkUnit = getUnitByID(m_SelectedUnit);
	// atkUnit 은 처음에 이동한 유닛임

	UnitMoveType	atkType = atkUnit->GetMoveType();

	HexaPoint		unitPos = unit->GetPosition();
	int				unitAtk = unit->GetAttack();
	Color4F			signcolor = (unit == atkUnit) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;

	// 유닛 패스를 처음부터 그리는 경우
	#pragma region If First Call
	if (isFirstCall)
	{
		// 처음부터 다시 그려줌
		releaseMoveSign();

		switch (atkType)
		{
		case UMT_STRAIGHT:
		case UMT_JUMP:{

						  direction = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).GetDirection();
						  range = ((ScreenPoint)(m_StartPoint - m_CursoredPoint)).GetRange();
						  if (range > unit->GetMoveRange()) range = unit->GetMoveRange();
		}break;

		case UMT_DASH:{
						  // 커서가 가리키는 위치 검색
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

						  if (!isHexaGrid) return;	// 헥사그리드 범위가 아니면 OUT!

						  if (cursor == atkUnit->GetPosition())
						  {
							  // 공격 유닛을 가리키면 스택 초기화
							  releaseMoveSign();
							  m_CourseStack.clear();
							  return;
						  }

						  // 스택이 비었나요?
						  if (m_CourseStack.empty())
						  {
							  // 공격 유닛 기준으로 1칸 범위에 없으면 OUT!
							  if (!atkUnit->GetPosition().isAround(cursor, 1)) return;
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

							  // 스택에 저장된 마지막 요소 기준으로 1칸 범위에 없으면 OUT!
							  if (!m_CourseStack.back().isAround(cursor, 1)) return;

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

							  if (m_CourseStack.size() == unitAtk)
							  {
								  return;	// 설마 스택이 꽉 찼어요? OUT!
							  }

						  }

						  m_CourseStack.push_back(cursor);
						  range = m_CourseStack.size();
						  if (range >= 2)
						  {
							  // 뒤에서 두번째에 있는 요소와 비교
							  HexaPoint prevPos = m_CourseStack.at(range - 2);
							  direction = prevPos.GetDirection(cursor);
						  }
						  else if (m_CourseStack.size() == 1)
						  {
							  direction = atkUnit->GetPosition().GetDirection(cursor);
						  }
		}break;

		case UMT_TELEPORT:{
							  // 커서가 가리키는 위치 검색
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

							  // 가리키는 위치에 변경이 없으면 OUT!
							  if (!m_CourseStack.empty() && m_CourseStack.back() == cursor) return;

							  // 이동범위 벗어나면 OUT!
							  if (!unitPos.isAround(cursor, unitAtk)) return;

							  if (cursor == atkUnit->GetPosition())
							  {
								  // 공격 유닛을 가리키면 스택 초기화
								  releaseMoveSign();
								  m_CourseStack.clear();
								  return;
							  }

							  releaseMoveSign();
							  m_CourseStack.clear();
							  m_CourseStack.push_back(cursor);
		}break;
		default:
			assert(false && "drawUnitMove -> not defined New attackType !");
		}
	}
#pragma endregion

	// 본격적으로 유닛이 이동할 수 있는 곳들을 그려줍니다
	#pragma region Attack Unit Move Draw
	switch (atkType)
	{
	case UMT_STRAIGHT:{
		// 이동경로 표시
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint atkCourse = unitPos.GetMovePoint(direction, i);

			// 바닥에 예상 이동 위치를 그려준다
			drawMoveSign(atkCourse, signcolor);

			// 이동경로에 다른 유닛이 존재하는가?
			if (Unit* crashUnit = getUnitByPos(atkCourse))
			{
				// 그럼 거기서부터 다른 유닛의 이동경로를 그리도록 하시오
				// 첫 충돌이면 현재 유닛의 공격력 - 충동 유닛의 무게만큼
				int atkRange = unitAtk - crashUnit->GetWeight();

				// 움직이자
				if (atkRange > 0) drawUnitMove(crashUnit, direction, atkRange, false);
					break;
			}
		}
	}break;

	case UMT_JUMP:{
		// 이동경로 표시
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint atkCourse = unitPos.GetMovePoint(direction, i);

			drawMoveSign(atkCourse, signcolor);

			// 이동경로에 다른 유닛이 존재하는가?
			if (Unit* crashUnit = getUnitByPos(atkCourse))
			{
				// 혹시 네 주인이... E.N.E.M.Y 니?
				// 아님 혹시 여기가... range의 끝..?
				if ((crashUnit->GetOwner() == UO_ENEMY) || (i == range))
				{
					// 그런데.. 이 길목에 우리편이 있으면... 걔랑 겹쳐버리잖니?
					// 자.. 시간을 거슬러 올라가보자꾸나..
					// 그나저나 얘.. 방향은 반대쪽이란다.. 
					HexaPoint beforePosition = atkCourse.GetMovePoint(direction, -1);

					// 거기 혹시 우리집 유닛 있습니까?
					if (nullptr == getUnitByPos(beforePosition) || range == 1)
					{
						// 없음? 님아 충돌여ㅋㅋㅋㅋㅋㅋㅋ
						int atkRange = unitAtk - crashUnit->GetWeight();
						drawUnitMove(crashUnit, direction, atkRange, false);
						break;
					}
					else
					{
						// g헐 얘 여기서 뭘 하고 있는 거니ㅠ;
						// 어쩔 수 없지 얘 뒤까지만 가야겠다
						releaseMoveSign();
						if (range - 1 > 0) drawUnitMove(atkUnit, direction, range - 1, false);
						break;
					}
				}
			}
		}
	}break;

	case UMT_DASH:{
		// 스택에 들어가있는대로 그려주세요!
		vector<HexaPoint>::iterator i;
		for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
		{
			drawMoveSign(*i, signcolor);
			// 충돌함?
			if (Unit* crashUnit = getUnitByPos(*i))
			{
				int atkRange = unitAtk - crashUnit->GetWeight();
				if (atkRange > 0) drawUnitMove(crashUnit, direction, atkRange, false);

				// 이 뒤부터 스택 비워줌
				vector<HexaPoint>::iterator j = ++i;
				for (; j != m_CourseStack.end();)
				{
					j = m_CourseStack.erase(j);
				}
				return;
			}
		}
	}break;

	case UMT_TELEPORT:{
		// 다른 유닛이 존재해 이동할 수 없는 곳은 빨간색으로
		if (m_CourseStack.empty()) return;
		HexaPoint movePoint = m_CourseStack.at(0);

		signcolor = (getUnitByPos(movePoint) == nullptr) ? COLOR_OF_PLAYER : COLOR_OF_ENEMY;
		drawMoveSign(movePoint, signcolor);
	}break;

	default:
		assert(false && "drawUnitMove -> not defined New attackType2 !");
	}
#pragma endregion

	// 공격에 의해 날라가는 유닛을 그려줌
	if (unit != atkUnit)
	{
		// 이동경로 표시
		for (int i = 1; i <= range; ++i)
		{
			HexaPoint atkCourse = unitPos.GetMovePoint(direction, i);

			// 바닥에 예상 이동 위치를 그려준다
			drawMoveSign(atkCourse, signcolor);

			// 이동경로에 다른 유닛이 존재하는가?
			if (Unit* crashUnit = getUnitByPos(atkCourse))
			{
				// 그럼 거기서부터 다른 유닛의 이동경로를 그리도록 하시오
				// 두번째 이후에는 (예상 이동 위치 - 현재까지 그려준 거리) 만큼
				int atkRange = (range - i + 1);

				// 움직이자
				if (atkRange > 0) drawUnitMove(crashUnit, direction, atkRange, false);
					break;
			}
		}
	}
}

// 이동경로를 그리는 함수
void GameScene::drawMoveSign(HexaPoint point, Color4F signColor)
{
	Hexagon* courseSignHexagon = createHexagon(point.HexaToScreen(), HEXAGON_LENGTH);
	DrawNode* courseSignNode = DrawNode::create();

	courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, signColor, 1, signColor);
	this->addChild(courseSignNode, 99);
	m_CourseSignNode.push_back(courseSignNode);
}

// 그려놓은 이동경로를 초기화하는 함수
void GameScene::releaseMoveSign()
{
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();
}

// 클릭한 좌표가 특정 육각형 안에 들어있는지 확인하는 함수
bool GameScene::isInHexagon(ScreenPoint touch, ScreenPoint anchor)
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
		if (unitID == unit->GetID())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
}

Unit* GameScene::getUnitByPos(HexaPoint unitPos)
{
	for (auto unit : m_UnitList)
	{
		if (unitPos == unit->GetPosition())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
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
		Sprite* unitSprite = unit->GetSprite();
		ScreenPoint unitRealPoint = unit->GetPosition().HexaToScreen();

		// 화면상의 좌표에 유닛을 배치
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

		m_HexagonPoint.push_back(HexaPoint(x, y));	// m_HexagonPoint 에 화면에 그려지는 좌표(0~x, 0~y)들을 저장

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

		float scale = HEXAGON_LENGTH * 2 / fieldBlock->getContentSize().width;
		fieldBlock->setScale(scale);
		fieldBlock->setAnchorPoint(Vec2(0.5f, 0.67f));
		fieldBlock->setPosition(point);
		this->addChild(fieldBlock, y);

		if (DRAW_HEXA_NUMBER) drawText(x, y, hexa);	// 헥사곤 안에 정수형 인덱스 값을 보여줄 것인가?

	}
}

void GameScene::onGameStart(Packet::GameStartResult inPacket)
{
	ReadFieldBlock(inPacket.mFieldList, inPacket.mFieldLength);
	ReadUnitData(inPacket.mUnit, inPacket.mUnitLength);
}

// sender 무시하셈!
void GameScene::onUnitAction(CCNode* sender)
{
	if (!m_UnitActionQueue.empty())
	{
		// i 번째 액션을 가져옴
		UnitAction action = m_UnitActionQueue.front();
		m_UnitActionQueue.pop();

		// id를 사용하여 i 번째 액션을 적용할 유닛을 찾음
		Unit* unit = getUnitByID(action.mUnitId);

		if (DEBUG_PRINT_PACKET)
			printf("Read Ation from Server\n");

		switch (action.mActionType)
		{
		case UAT_MOVE:{
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

						  if (DEBUG_PRINT_PACKET)
							  printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

						  // 유닛은 인덱스로 이동
						  unit->setPosition(hMovePoint);

						  // 스프라이트는 좌표로 이동
						  auto sprite = unit->GetSprite();
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

						  auto sprite = unit->GetSprite();
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
						  // 적용할 액션의 이동타입이 대쉬면?
						  // 다른 타입의 액션이 들어올때까지 반복
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = hMovePoint.HexaToScreen();

						  if (DEBUG_PRINT_PACKET)
							  printf("Move To(%d, %d)\n", hMovePoint.x, hMovePoint.y);

						  // 유닛은 인덱스로 이동
						  unit->setPosition(hMovePoint);

						  // 스프라이트는 좌표로 이동
						  auto sprite = unit->GetSprite();
						  CCFiniteTimeAction* actionMove =
							  CCMoveTo::create(MOVE_DURATION,
							  sMovePoint);
						  // TODO : CCBezier

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

							  // 텔레포트는 뿅! 하고 이동
							  auto sprite = unit->GetSprite();
							  CCFiniteTimeAction* actionMove =
								  CCMoveTo::create(0, sMovePoint);

		}break;
		case UAT_COLLISION:{

		}break;
		case UAT_DIE: {
						  unit->GetSprite()->setVisible(false);
		}
		}
	}
}

void GameScene::HighlightHexagon(ScreenPoint position)
{
	Hexagon* expectSignHexagon = createHexagon(position, HEXAGON_LENGTH);
	DrawNode* expectSignNode = DrawNode::create();

	expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
	this->addChild(expectSignNode, 98);
	m_ExpectSignNode.push_back(expectSignNode);
}

HexaPoint GameScene::ScreenToHexa(ScreenPoint point)
{
	HexaPoint retPoint(-1, -1);

	for (int i = 0; i < m_HexagonPoint.size(); ++i)
	{
		if (isInHexagon(point, m_HexagonPoint.at(i).HexaToScreen()))
		{
			retPoint = m_HexagonPoint.at(i);
			break;
		}
	}
	
	return retPoint;
}