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
    if ( !LayerColor::initWithColor(ccc4(255,255,255,255)))
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	TcpClient::getInstance()->loginRequest();
	// 서버님 로그인 하게 해줘요 !!

	// TODO : 서버님 유닛 배치( 맵의 상태 ) 주세요 !!

	// TODO : 서버님 누구 차례인지 알려줘요 !!

	touchEventInit();

	Game::getInstance();

	drawHexagon();
    
	this->schedule(schedule_selector(GameScene::gameLogic), 0.0f);
	// 게임 로직 실행

	this->setKeyboardEnabled(true);

    return true;
}

void GameScene::gameLogic(float dt)
{
	
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	static int id = 1;

	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		break;

	default:
		break;
	}
}

void GameScene::touchEventInit()
{
	m_IsClicked = false;

	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(true);

	_touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);

	EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();

	dispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	Point touchPoint = touch->getLocation();
	m_IsClicked = true;

	return true;
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{

	m_IsClicked = false;
}

Hexagon* GameScene::createHexagon(Point anchor, int size)
{
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

void GameScene::drawHexagon()
{
	CCDrawNode* node = CCDrawNode::create();
	this->addChild(node);

	float x = HEXAGON_LENGTH * 1.5;
	float y = HEXAGON_LENGTH * 2 * sin(RADIANS_60);

	Point point;

	for (int i = 0; i < MAP_SCALEX; ++i)
	{
		for (int j = 0; j < MAP_SCALEY + MAP_SCALEX / 2; ++j)
		{
			if (i + j >= MAP_SCALEX / 2 && i + j <= MAP_SCALEX + MAP_SCALEY - 2)
			{
				point.x = MAP_XSTART + x * i;
				point.y = MAP_YSTART + y * (j + i*0.5 - MAP_SCALEY / 2);

				if (point.y <= MAP_YSTART + y * (MAP_SCALEX / 2 - MAP_SCALEY / 2 - 1) ||
					point.y >= MAP_YSTART + y * (MAP_SCALEX / 2 + MAP_SCALEY / 2))
					continue;
				// 사각형이 되도록 육각형의 상,하 모서리를 지우기 위한 if - continue 문 임 .

				Hexagon* hexa = createHexagon(point, HEXAGON_LENGTH);
				node->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.8f, 0.0f, 0.8f, 0.0f), 1, ccc4f(0.8f, 0.0f, 0.0f, 0.8f));
			}
		}
	}
}