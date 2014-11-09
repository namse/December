#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "Header.h"
#include "Game.h"
#include "Unit.h"

enum GameState{
	GS_BEFORE_LOGIN,
	GS_WAIT_GAME,
	GS_GAME_START,
};
class GameScene : public LayerColor
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene*			createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool			init();
    
    // a selector callback
    void					menuCloseCallback(Ref* pSender);

	void					gameLogic(float dt);

	EventListenerTouchOneByOne*	_touchListener;
	void					touchEventInit();
	virtual bool			onTouchBegan(Touch* touch, Event* event);
	virtual void			onTouchMoved(Touch* touch, Event* event);
	virtual void			onTouchCancelled(Touch* touch, Event* unused_event){}
	virtual void			onTouchEnded(Touch* touch, Event *event);
	bool					touchCheck(ScreenPoint touch, ScreenPoint anchor);

	CREATE_FUNC(GameScene);

	Hexagon*				createHexagon(ScreenPoint anchor, int size);
	Unit*					createUnit(UnitType unitType);

	void					drawHexaGrid();
	void					drawText(int i, int j, Hexagon* hexa);
	void					drawUnitMove(Unit* unit, HexaDirection direction = HD_NONE, int range = 0);

	bool					drawToRect(float y);
	bool					drawToHexa(int x, int y);

	Unit*					getUnitByID(int unitID);
	Unit*					getUnitByPos(HexaPoint unitPos);
	HexaDirection			getPointToPointDirection(ScreenPoint point1, ScreenPoint point2);
	int						getPointToPointDistance(ScreenPoint point1, ScreenPoint point2);
	HexaPoint				getPointMoveDirection(HexaPoint start, HexaDirection direction, int range);
	HexaDirection			getInverseDirection(HexaDirection direction);

	// 하위는 네트워크 관련 함수
	void					ReadUnitData(UnitData unitData[], int length);
	// Header 안에서 typedef Packet::GameStartResult::UnitData UnitData 했음 .
	void					SetTurn(bool isMyTurn){ m_IsMyTurn = isMyTurn; }
	void					ReadActionQueue(UnitAction unitActionQueue[], int length);

private:
	Game					m_Game;
	GameState				m_GameState;
	bool					m_IsMyTurn;

	Player					m_Player[2];
	vector<Unit*>			m_UnitList;

	vector<CCDrawNode*>		m_CourseSignNode;
	ScreenPoint				m_StartPoint;
	ScreenPoint				m_CursoredPoint;

	vector<Coord>			m_HexagonPoint;

	vector<Unit*>			m_UnitData;

	int						m_SelectedUnit;
};

#endif // __HELLOWORLD_SCENE_H__
