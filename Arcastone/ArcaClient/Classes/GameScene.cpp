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

	m_MaxCosst = 2;

	m_IsCursorMoved = false;

	m_IsCastSkill = false;

	for (int i = 0; i < MAX_HAVE_COST; ++i)
	{
		m_CostLabel[i] = LabelTTF::create("", "Hevetica", 20);

		m_CostLabel[i]->setPosition(Point(120 + i * 28, DISPLAY_SIZEY - 64));

		m_CostLabel[i]->setColor(Color3B(100, 100, 100));

		this->addChild(m_CostLabel[i], 0 + ZORDER_UI);
	}

	m_TurnLabel = LabelTTF::create("Enemy Turn", "Hevetica", 20);

	m_TurnLabel->setPosition(Point(55, DISPLAY_SIZEY - 64));

	m_TurnLabel->setColor(Color3B(100, 100, 100));

	this->addChild(m_TurnLabel, 0 + ZORDER_UI);

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
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bgm.mp3",true);
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
	if (!m_IsMyTurn) return false;

	m_StartPoint = ScreenPoint(touch->getLocation().x, touch->getLocation().y);
	m_CursoredPoint = m_StartPoint;

	// 스킬 사용 범위 선택중이라면 return true
	if (m_IsCastSkill) return true;

	// 커서 안움직임요
	m_IsCursorMoved = false;

	m_SelectedUnit = NON_SELECT_UNIT;

	for (int i = 0; i < m_UnitList.size(); ++i)
	{
		Unit* unit = m_UnitList.at(i);

		// 자신의 유닛인지 확인한다
		if (unit->GetOwner() != UO_ME) continue;

		// 유닛의 좌표인덱스를 화면상 위치로 변환하여
		ScreenPoint screenPoint = m_Field.HexaToScreen(unit->GetPosition());

		if (unit->GetPosition().x == 3 && unit->GetPosition().y == 7)
			int a = 0;

		// 어떤 유닛을 클릭했는지 판정한다.
		if (m_Field.IsInHexagon(m_StartPoint, screenPoint))
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

	// 스킬 범위 선택중이라면 일단 리턴~
	if (m_IsCastSkill) return;

	Unit* unit = GetUnitByID(m_SelectedUnit);
	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;

	// 어딜 터치했는지 찾아서
	HexaPoint touchIndex;
	touchIndex = m_Field.ScreenToHexa((ScreenPoint)touch->getLocation());
	
	HexaPoint cursoredPoint;
	cursoredPoint = m_Field.ScreenToHexa(m_CursoredPoint);

	// 터치한 위치가 (인덱스단위로) 이동했는지 확인하고 이동안했으면 그냥 return;
	if (touchIndex == cursoredPoint) return;

	m_CursoredPoint = ScreenPoint(touch->getLocation());

	// 커서 움직임요
	m_IsCursorMoved = true;

	DrawUnitMove();
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	{
		// 이동할 수 있는 모든 범위 표시 지움
		releaseExpectMoveSign();
		// 그려진 이동 경로 지움
		ReleaseMoveSign();
	}

	if (!m_IsMyTurn) return;

	Unit* unit = GetUnitByID(m_SelectedUnit);

	// 적합한 유닛을 선택하지 않았거나, 유닛이 nullptr 이면 패스
	if (m_SelectedUnit == NON_SELECT_UNIT || unit == nullptr) return;



	// 스킬 장전! 목표설정 상태!
	if (m_IsCastSkill)
	{
		UsingSkill(unit);

		m_IsCastSkill = false;

		return;
	}

	// 커서 안움직이고 유닛만 눌렀다 뗐는데요? && 스킬 준비 상태도 아니에요
	if (m_IsCursorMoved == false && m_IsCastSkill == false)
	{
		// 그럼 스킬을 쓰려는 거구나!
		m_IsCastSkill = true;
		
		// 스킬 쓸 수 있는 범위 그려줄게~
		drawSkillEffect(GetUnitByID(m_SelectedUnit)->GetSkill());

		// 아니 근데 너 스킬 못쓰잖아?
		if (m_CourseStack.empty())
		{
			m_IsCastSkill = false;
		}

		return;
	}

	// 유닛을 드래그 했으므로 공격 패킷 발싸!
	{
		AttackData attackData;

		switch (unit->GetMoveType())
		{
		case UMT_STRAIGHT:{
							  attackData.attackType = UMT_STRAIGHT;
							  if (m_Direction == HD_NONE || m_Range <= 0) return;
		}break;

		case UMT_JUMP:{
						  attackData.attackType = UMT_JUMP;
						  if (m_Direction == HD_NONE || m_Range <= 0) return;
		}break;

		case UMT_DASH:{
						  attackData.attackType = UMT_DASH;
						  m_Range = m_CourseStack.size();
						  for (int i = 0; i < m_CourseStack.size(); ++i)
						  {
							  // 대쉬는 이동 스택을 입력
							  attackData.position[i] = m_CourseStack.at(i).HexaToCoord();
						  }
		}break;

		case UMT_TELEPORT:{
							  attackData.attackType = UMT_TELEPORT;
							  // 텔포는 이동 칸 하나 입력
							  if (m_CourseStack.size() == 1)
							  {
								  // 유닛이 없는 칸만 이동가능
								  if (GetUnitByPos(m_CourseStack.at(0)) == nullptr)
								  {

									  attackData.position[0] = m_CourseStack.at(0).HexaToCoord();
								  }
								  else return;
							  }
							  else return;
		}break;

		default:
			assert(false && "DrawUnitMove -> not defined New attackType !");
		}

		m_CourseStack.clear();

		attackData.id = unit->GetID();
		attackData.direction = m_Direction;
		attackData.range = m_Range;

		m_Range = 0;
		m_Direction = HD_NONE;

		TcpClient::getInstance()->attackRequest(attackData);
	}

	m_SelectedUnit = NON_SELECT_UNIT;
}

void GameScene::UsingSkill(Unit* unit)
{
	SkillData skillData;
	Skill skill = unit->GetSkill();

	skillData.id = unit->GetID();
	skillData.skillType = skill.type;
	skillData.skillRank = skill.rank;

	switch (skill.type)
	{
	case USK_FIREBALL:{
#ifdef _DEBUG
						  printf("Using FireBall!!\n\n");
#endif

						  skillData.position[0] = Coord(-1, -1);

						  // 파이어 볼은 유효한 범위 중 내가 선택한 좌표로 스킬 시전
						  for (int i = 0; i < m_CourseStack.size(); ++i)
						  {
							  if (m_Field.ScreenToHexa(m_CursoredPoint) == m_CourseStack.at(i))
							  {
								  skillData.position[0] = m_CourseStack.at(i).HexaToCoord();
								  break;
							  }
						  }
	}break;

	case USK_STAMP:{
#ifdef _DEBUG
					   printf("Using Stamp!!\n\n");
#endif


	}break;

	default:
		break;
	}


	// 스킬패킷 발사!
	TcpClient::getInstance()->skillRequest(&skillData);
}

void GameScene::drawSkillEffect(Skill skill)
{
	switch (skill.type)
	{
	case USK_FIREBALL:{
						  Unit* pCaster = GetUnitByID(m_SelectedUnit);
						  HexaPoint castPoint = pCaster->GetPosition();

						  for (int d = 0; d < 6; ++d)
						  {
							  for (int l = 1; l < skill.rank + 2; ++l)
							  {
								  HexaPoint effectPoint = castPoint.GetMovePoint(HexaDirection(d + 1), l);
								  if (GetUnitByPos(effectPoint) != nullptr)
								  {
									  // 유닛이 있는 곳에만 사용 가능한 스킬임!
									  drawMoveSign(effectPoint, COLOR_OF_CRASHED);
									  m_CourseStack.push_back(effectPoint);
									  break;
								  }
								  drawMoveSign(effectPoint, COLOR_OF_SKILL);
							  }
						  }
	}break;
	default:
		break;
	}
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
					ScreenPoint realMovePoint = m_Field.HexaToScreen(movePoint);

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

					Hexagon* expectSignHexagon = new Hexagon(m_Field.HexaToScreen(movePoint));
					DrawNode* expectSignNode = DrawNode::create();

					expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
					this->addChild(expectSignNode, 98);
					m_ExpectSignNode.push_back(expectSignNode);
				}
			}

		}break;
		default:
			assert(false && "DrawUnitMove -> not defined New attackType !");
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

// 선택한 유닛의 이동 타입을 확인해서
// 타입에 따라 방향과 거리. 혹은 좌표들을 저장하고
// 화면에 그려주는 함수
void GameScene::DrawUnitMove()
{
	Unit*				attacker = GetUnitByID(m_SelectedUnit);
	UnitMoveType		unitAtkType = attacker->GetMoveType();
	int					unitRange = attacker->GetMoveRange();
	int					unitAtk = attacker->GetAttack();
	HexaPoint			unitPos = attacker->GetPosition();
	HexaPoint			cursoredHexa = m_Field.ScreenToHexa(m_CursoredPoint);
	HexaPoint			startHexa = m_Field.ScreenToHexa(m_StartPoint);
	
	//	#pragma region 마우스의 이동과 공격 방식에 따라 가능한지 체크하고, 가능하면 입력
	switch (unitAtkType)
	{
	case UMT_STRAIGHT:
	case UMT_JUMP:{
					  // TODO
					  Point line[3] = 
					  {
							m_StartPoint,
							m_CursoredPoint,
							m_StartPoint
					  };

					  DrawNode* courseSignNode = DrawNode::create();

					  courseSignNode->drawPolygon(line, 3, Color4F(1.0f, 1.0f, 1.0f, 0.5f), 1, Color4F(0.0f, 0.0f, 0.0f, 0.0f));



					  // 선택한 공격자 유닛의 위치로 커서를 이동했다면 초기화
					  if (cursoredHexa == unitPos)
					  {
						  m_Range = 0;
						  m_Direction = HD_NONE;
						  ReleaseMoveSign();
						  return;
					  }

					  // 직선이동은 공격유닛의 위치와 현재 마우스의 위치로 방향, 사거리 정함
					  {
						  // TODO : 당긴 량에 비례해서 증가하도록 수정
						  m_Range = HexaPoint(startHexa - cursoredHexa).GetRange();
						  m_Direction = cursoredHexa.GetDirection(startHexa);
						  if (m_Range > unitRange) m_Range = unitRange;

						  // 0 칸이나 이상한 방향은 이동할 수 없어..
						  if (m_Range == 0 || m_Direction == HD_NONE)
						  {
							  m_Range = 0;
							  m_Direction = HD_NONE;
							  return;
						  }
					  }

					  // 이 전의 패스는 지우고
					  ReleaseMoveSign();
					  if (unitAtkType == UMT_STRAIGHT)
					  {
						  for (int i = 1; i <= m_Range; ++i)
						  {
							  HexaPoint atkCourse = unitPos.GetMovePoint(m_Direction, i);
							  Unit* crashUnit = GetUnitByPos(atkCourse);

							  // 아 근데, 여기 다른유닛 있어?
							  if (crashUnit == nullptr)
							  {
								  drawMoveSign(atkCourse, COLOR_OF_PLAYER);
							  }
							  else
							  {
								  KnockBackDraw(attacker, crashUnit, m_Direction, unitAtk);
								  return;
							  }
						  }
					  }

					  if (unitAtkType == UMT_JUMP)
					  {
						  // 점프 이동은 사거리 끝만을 공격하지!
						  HexaPoint atkPoint = unitPos.GetMovePoint(m_Direction, m_Range);
						  Unit* crashUnit = GetUnitByPos(atkPoint);

						  // 사거리 끝에 아무도 없으면
						  if (crashUnit == nullptr)
						  {
							  // 무조건 이동할 수 있어!
							  drawMoveSign(atkPoint, COLOR_OF_PLAYER);
							  return;
						  }
						  else // 어 누구 있네?
						  {
							  // 근데 한칸만 움직일거라고?
							  if (m_Range == 1)
							  {
								  // 그럼 그놈 밀어버려 ㅋㅋ
								  KnockBackDraw(attacker, crashUnit, m_Direction, unitAtk);
								  return;
							  }

							  HexaPoint preAtkPoint = unitPos.GetMovePoint(m_Direction, m_Range - 1);
							  Unit* preCrashUnit = GetUnitByPos(preAtkPoint);
							  // 그럼 혹시 전칸에도 누가 있어?
							  if (preCrashUnit != nullptr)
							  {
								  // 에이.. 그럼 못가겠네
								  ReleaseMoveSign();
								  m_CourseStack.clear();
								  drawMoveSign(atkPoint, COLOR_OF_CANTMOVE);
								  drawMoveSign(preAtkPoint, COLOR_OF_CANTMOVE);
								  m_Range = 0;
								  m_Direction = HD_NONE;
								  return;
							  }
							  // 오 없음? ㅋㅋ 그럼 고고
							  else
							  {
								  KnockBackDraw(attacker, crashUnit, m_Direction, unitAtk);
								  return;
							  }
						  }
					  }
	}break;

	case UMT_DASH:{
					  // 맵 밖으론 못가
					  if (!m_Field.IsThisHexaInField(cursoredHexa)) return;

					  // 제자리로도 못가
					  if (cursoredHexa == unitPos)
					  {
						  ReleaseMoveSign();
						  m_CourseStack.clear();
						  return;
					  }

					  if (m_CourseStack.empty())
					  {
						  // 1칸 이내만 공격 가능하지
						  if (!unitPos.isAround(cursoredHexa, 1)) return;
						  m_Direction = unitPos.GetDirection(cursoredHexa);
					  }
					  else
					  {
						  // 가리키는 위치에 변경이 없으면 OUT!
						  if (m_CourseStack.back() == cursoredHexa) return;	

						  // 스택에 저장된 마지막 요소 기준으로 1칸 범위에 없으면 OUT!
						  if (!m_CourseStack.back().isAround(cursoredHexa, 1)) return;

						  // 지금 선택된 좌표가 이미 스택에 저장돼있습니까?
						  vector<HexaPoint>::iterator i;
						  for (i = m_CourseStack.begin(); i != m_CourseStack.end(); ++i)
						  {
							  if (*i == cursoredHexa)
							  {
								  // 저장돼있다면 거기 다음부터 마지막 요소까지 삭제해버리세요
								  vector<HexaPoint>::iterator j;
								  for (j = i; j != m_CourseStack.end();)
								  {
									  j = m_CourseStack.erase(j);
								  }
								  // 덤으로 다 지우고
								  ReleaseMoveSign();
								  // 다 다시그려줘요
								  for (int k = 0; k < m_CourseStack.size(); ++k)
								  {
									  drawMoveSign(m_CourseStack.at(k), COLOR_OF_PLAYER);
								  }
								  break;
							  }
						  }

						  // 아직도 공격코스가 남았어?
						  if (!m_CourseStack.empty())
						  {
							  // 공격 대상이 코스에 있다면, 새로운 코스를 추가할수는 없다..
							  if (GetUnitByPos(m_CourseStack.back()) != nullptr) return;

							  // 이미 사거리만큼 차있는 경우도 새로운 코스를 추가할수는 없어
							  if (m_CourseStack.size() == unitRange) return;

							  m_Direction = m_CourseStack.back().GetDirection(cursoredHexa);
						  }
						  else
						  {
							  m_Direction = unitPos.GetDirection(cursoredHexa);
						  }
					  }


					  // 조건에 부합하므로 푸쉬
					  m_CourseStack.push_back(cursoredHexa);
					  drawMoveSign(cursoredHexa, COLOR_OF_PLAYER);

					  // 충돌한 유닛 밀어줌
					  Unit* crashUnit = GetUnitByPos(cursoredHexa);
					  if (crashUnit != nullptr)
					  {
						  KnockBackDraw(attacker, crashUnit, m_Direction, unitAtk);
					  }


	}break;

	case UMT_TELEPORT:{
						  // 맵 밖으론 못가
						  if (!m_Field.IsThisHexaInField(cursoredHexa)) return;

						  // 제자리로도 못가
						  if (cursoredHexa == unitPos)
						  {
							  ReleaseMoveSign();
							  m_CourseStack.clear();
							  return;
						  }

						  // 선택한 공격자 유닛의 위치로 커서 이동한 경우 무시
						  if (cursoredHexa == unitPos) return;

						  // 이동범위 벗어나면 OUT!
						  if (!unitPos.isAround(cursoredHexa, unitRange)) return;

						  // 조건에 부합하니까 푸시
						  m_CourseStack.clear(); // 혹시 모르니까 다 비우고 푸시
						  m_CourseStack.push_back(cursoredHexa);

						  ReleaseMoveSign();

						  if (m_CourseStack.empty()) return;
						  HexaPoint movePoint = m_CourseStack.at(0);
						  Unit* crashUnit = GetUnitByPos(movePoint);
						  Color4F signColor = COLOR_OF_PLAYER;

						  // 다른 유닛이 존재해 이동할 수 없는 곳은 다른색으로
						  if (crashUnit != nullptr)
						  {
							  m_CourseStack.clear();
							  signColor = COLOR_OF_CANTMOVE;
						  }
						  drawMoveSign(movePoint, signColor);

	}break;
	default:
		assert(false && "DrawUnitMove -> not defined New attackType !");
	}
}

void GameScene::KnockBackDraw(Unit* attacker, Unit* target, HexaDirection direction, int range)
{
	HexaPoint targetPoint = target->GetPosition();
	// 충돌 대상 표시
	drawMoveSign(targetPoint, COLOR_OF_CRASHED);

	if (!DRAW_KNOCKBACK) return;

	int targetWeight = target->GetWeight();
	int realMoveRange = range - targetWeight;
	if (realMoveRange <= 0) return;


	// 가해자는?
	if (attacker->GetOwner() != target->GetOwner())
	{
		// 적팀이면 피해를 입지요~
		// TODO : 피해를 입는다는 것 알려줄 필요 있음
	}

	for (int i = 1; i <= realMoveRange; ++i)
	{
		HexaPoint atkCourse = targetPoint.GetMovePoint(direction, i);
		Unit* crashUnit = GetUnitByPos(atkCourse);

		// 한칸씩 그림
		drawMoveSign(atkCourse, COLOR_OF_ENEMY);

		// 아 근데, 여기 다른 유닛이 있니?
		if (crashUnit != nullptr) // 응
		{
			// 거기서부터 그 유닛의 이동경로를 그리도록 하시오
			// 난 너를 이 방향으로 밀어버리겠다!
			KnockBackDraw(target, crashUnit, direction, realMoveRange);
			return;
		}
	}
}

// 이동경로를 그리는 함수
void GameScene::drawMoveSign(HexaPoint point, Color4F signColor)
{
	Hexagon* courseSignHexagon = new Hexagon(m_Field.HexaToScreen(point));
	DrawNode* courseSignNode = DrawNode::create();

	courseSignNode->drawPolygon(&courseSignHexagon->vertex[0], 6, signColor, 1, signColor);
	this->addChild(courseSignNode, 99);
	m_CourseSignNode.push_back(courseSignNode);
}

// 그려놓은 이동경로를 초기화하는 함수
void GameScene::ReleaseMoveSign()
{
	for (auto node : m_CourseSignNode)
	{
		this->removeChild(node);
	}
	m_CourseSignNode.clear();
}

// 유닛을 찾습니다
Unit* GameScene::GetUnitByID(int unitID)
{
	for (auto unit : m_UnitList)
	{
		if (unitID == unit->GetID())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
}

Unit* GameScene::GetUnitByPos(HexaPoint unitPos)
{
	for (auto unit : m_UnitList)
	{
		if (unitPos == unit->GetPosition())
			return unit;
	}
	return nullptr; // 검색에 실패하면 null return
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
		ScreenPoint unitRealPoint = m_Field.HexaToScreen(unit->GetPosition());

		// 화면상의 좌표에 유닛을 배치
		assert(unitSprite != nullptr);

		unit->setScreenPos(unitRealPoint);
		this->addChild(unitSprite);
	}

	m_GameState = GS_GAME_START;
}


void GameScene::ReadActionQueue(Packet::AttackResult attackResult)
{
	/*
	while (!m_UnitActionQueue.empty()) 
		m_UnitActionQueue.pop();
	*/
	bool unitAction = false;
	if (m_UnitActionQueue.empty())
	{
		// 큐가 비어있다면, 유닛 액션이 돌아가고 있지 않고 있다는 것이므로
		unitAction = true;
	}

	for (int i = 0; i < attackResult.mQueueLength; ++i)
	{
		m_UnitActionQueue.push(attackResult.mUnitActionQueue[i]);
	}
	if (unitAction) OnUnitAction();
}

void GameScene::OnGameStart(Packet::GameStartResult inPacket)
{
	if (USE_SOUND)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_GAMESTART);
	}
	m_Field.Init(this, inPacket.mFieldList, inPacket.mFieldLength, inPacket.mFieldSizeX, inPacket.mFieldSizeY, inPacket.mReverseMap);
	ReadUnitData(inPacket.mUnit, inPacket.mUnitLength);
}

// sender 무시하셈!
void GameScene::OnUnitAction(CCNode* sender)
{
	if (!m_UnitActionQueue.empty())
	{
		// i 번째 액션을 가져옴
		UnitAction action = m_UnitActionQueue.front();
		m_UnitActionQueue.pop();

		// id를 사용하여 i 번째 액션을 적용할 유닛을 찾음
		Unit* unit = GetUnitByID(action.mUnitId);

		PrintUnitAction(action);

		switch (action.mActionType)
		{
		case UAT_MOVE:{
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = m_Field.HexaToScreen(hMovePoint);

						  // 유닛은 인덱스로 이동
						  unit->setPosition(hMovePoint);

						  // 스프라이트는 좌표로 이동
						  auto sprite = unit->GetSprite();
						  CCFiniteTimeAction* actionMove =
							  CCMoveTo::create(MOVE_DURATION,
							  sMovePoint);
						  CCFiniteTimeAction* actionMoveDone =
							  CCCallFuncN::create(this, callfuncN_selector(GameScene::OnUnitAction));

						  sprite->runAction(CCSequence::create(actionMove,
							  actionMoveDone, NULL));

						  if (USE_SOUND)
						  {
							  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_MOVE);
						  }
		}break;
		case UAT_STRAIGHT:{
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = m_Field.HexaToScreen(hMovePoint);

						  // 유닛은 인덱스로 이동
						  unit->setPosition(hMovePoint);

						  // 스프라이트는 좌표로 이동
						  auto sprite = unit->GetSprite();
						  CCFiniteTimeAction* actionMove =
							  CCMoveTo::create(MOVE_DURATION,
							  sMovePoint);
						  CCFiniteTimeAction* actionMoveDone =
							  CCCallFuncN::create(this, callfuncN_selector(GameScene::OnUnitAction));


						  sprite->runAction(CCSequence::create(actionMove,
							  actionMoveDone, NULL));

						  if (USE_SOUND)
						  {
							  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_MOVE);
						  }
		}break;

		case UAT_JUMP:{
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = m_Field.HexaToScreen(hMovePoint);

						  unit->setPosition(hMovePoint);

						  auto sprite = unit->GetSprite();
						  CCFiniteTimeAction* actionMove =
							  CCMoveTo::create(MOVE_DURATION,
							  sMovePoint);
						  CCFiniteTimeAction* actionMoveDone =
							  CCCallFuncN::create(this,
							  callfuncN_selector(GameScene::OnUnitAction));
						  sprite->runAction(CCSequence::create(actionMove,
							  actionMoveDone, NULL));

						  if (USE_SOUND)
						  {
							  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_MOVE);
						  }
		}break;

		case UAT_DASH:{
						  // 적용할 액션의 이동타입이 대쉬면?
						  // 다른 타입의 액션이 들어올때까지 반복
						  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
						  ScreenPoint sMovePoint = m_Field.HexaToScreen(hMovePoint);

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
							  callfuncN_selector(GameScene::OnUnitAction));
						  sprite->runAction(CCSequence::create(actionMove,
							  actionMoveDone, NULL));

						  if (USE_SOUND)
						  {
							  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_MOVE);
						  }
		}break;

		case UAT_TELEPORT:{
							  HexaPoint hMovePoint(action.mMoveData.mFinalX, action.mMoveData.mFinalY);
							  ScreenPoint sMovePoint = m_Field.HexaToScreen(hMovePoint);

							  unit->setPosition(hMovePoint);

							  // 텔레포트는 뿅! 하고 이동
							  auto sprite = unit->GetSprite();
							  CCFiniteTimeAction* actionMove =
								  CCMoveTo::create(0, sMovePoint);
							  sprite->runAction(actionMove);

							  if (USE_SOUND)
							  {
								  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_MOVE);
							  }

		}break;
		case UAT_COLLISION:{
							   // 부딪힌 놈들 체력설정
							   GetUnitByID(action.mUnitId)->SetHP(action.mCollisionData.mMyHP);
							   GetUnitByID(action.mCollisionData.mTarget)->SetHP(action.mCollisionData.mTargetHP);

							   runAction(CCCallFuncN::create(this,
								   callfuncN_selector(GameScene::OnUnitAction)));

							   if (USE_SOUND)
							   {
								   CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_COLLISION);
							   }
		}break;
		case UAT_DIE: {
						  unit->GetSprite()->setVisible(false);
						  unit->SetStatus(UST_DEAD);
						  unit->setPosition(HexaPoint(100, 100));

						  if (USE_SOUND)
						  {
							  CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(ARCA_SOUND_DIE);
						  }
		}
		}
	}
}

void GameScene::HighlightHexagon(ScreenPoint position)
{
	Hexagon* expectSignHexagon = new Hexagon(position);
	DrawNode* expectSignNode = DrawNode::create();

	expectSignNode->drawPolygon(&expectSignHexagon->vertex[0], 6, COLOR_OF_EXPECT, 1, COLOR_OF_EXPECT);
	this->addChild(expectSignNode, 98);
	m_ExpectSignNode.push_back(expectSignNode);
}

void GameScene::PrintUnitAction(UnitAction attackData)
{
	printf("Read Unit Action Queue\n");
	switch (attackData.mActionType)
	{
	case UAT_MOVE:
		printf("MoveType : UAT_MOVE");
		printf("\n");
		goto UnitMove;
	case UAT_STRAIGHT:
		printf("MoveType : UAT_STRAIGHT");
		printf("\n");
		goto UnitMove;
	case UAT_JUMP:
		printf("MoveType : UAT_JUMP");
		printf("\n");
		goto UnitMove;
	case UAT_DASH:
		printf("MoveType : UAT_DASH");
		printf("\n");
		goto UnitMove;
	case UAT_TELEPORT:
		printf("MoveType : UAT_TELEPORT");
		printf("\n");
		goto UnitMove;

	case UAT_COLLISION:
		printf("Action Type : Unit Collision");
		printf("\n");
		goto UnitCollision;

	case UAT_DIE:
		printf("Action Type : Unit Die");
		printf("\n");
		goto UnitDie;
	}

	return;

UnitMove:
	printf("Unit ID : %d\n Move Range : %d\n Move Direction : %d\n Move Point : %d, %d\n",
		(int)attackData.mUnitId,
		attackData.mMoveData.mRange,
		(int)attackData.mMoveData.mDirection,
		(int)attackData.mMoveData.mFinalX,
		(int)attackData.mMoveData.mFinalY);
	printf("\n");

	return;

UnitCollision:
	printf("Attacker ID : %d\n", (int)attackData.mUnitId);
	printf("Target ID : %d\n", (int)attackData.mCollisionData.mTarget);
	printf("Attacker HP : %d\n", attackData.mCollisionData.mMyHP);
	printf("Target HP : %d\n\n", attackData.mCollisionData.mTargetHP);

	return;

UnitDie:
	printf("Die Unit ID : %d\n\n", (int)attackData.mUnitId);

	return;
}

void GameScene::ReadRestCost(Packet::CostRenewalResult recvData)
{
	printf("Rest Cost : %d\nMax Cost : %d\n\n", recvData.mCost, recvData.mMaxCost);

	for (int i = 0; i < MAX_HAVE_COST; ++i)
	{
		m_CostLabel[i]->setString(" ");
	}

	for (int i = 0; i < recvData.mCost; ++i)
	{
		m_CostLabel[i]->setString("O");
	}

	for (int i = recvData.mCost; i < recvData.mMaxCost; ++i)
	{
		m_CostLabel[i]->setString("X");
	}

	m_MaxCosst = recvData.mMaxCost;
}

void GameScene::SetTurn(bool isMyTurn)
{
	m_IsMyTurn = isMyTurn;


	if (isMyTurn)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("myturn.mp3");
		m_TurnLabel->setString("Your Turn");
		for (int i = 0; i < m_MaxCosst; ++i)
		{
			m_CostLabel[i]->setString("O");
		}
	}
	else
		m_TurnLabel->setString("Enemy Turn");
}

void GameScene::GameOver(Packet::GameOverResult recvData)
{
	// 무승부
	if (recvData.mIsDraw == true)
	{
		m_TurnLabel->setString("draw!");
	}
	// 승리
	else if (recvData.mIsMyWin == true)
	{
		m_TurnLabel->setString("you win!");
	}
	// 패배
	else if (recvData.mIsMyWin == false)
	{
		m_TurnLabel->setString("you lose!");
	}

}

void GameScene::ChangeFieldType(Packet::ChangeFieldResult recvData)
{
	FieldBlock changeField = recvData.mFieldBlock;
	m_Field.SetFieldSprite(changeField);
}