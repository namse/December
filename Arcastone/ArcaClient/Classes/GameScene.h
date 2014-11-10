#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

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
	CREATE_FUNC(GameScene);

    // a selector callback
    void					menuCloseCallback(Ref* pSender);

	void					gameLogic(float dt);

	/* touch 함수 */
	EventListenerTouchOneByOne*	_touchListener;
	void					touchEventInit();
	virtual bool			onTouchBegan(Touch* touch, Event* event);
	virtual void			onTouchMoved(Touch* touch, Event* event);
	virtual void			onTouchCancelled(Touch* touch, Event* unused_event){}
	virtual void			onTouchEnded(Touch* touch, Event *event);
	bool					isInHexagon(ScreenPoint touch, ScreenPoint anchor);

	/* draw 함수 */
	Hexagon*				createHexagon(ScreenPoint anchor, int size);
	void					drawText(int i, int j, Hexagon* hexa);

	void					drawUnitMove(Unit* unit, HexaDirection direction = HD_NONE, int range = 0);
	void					drawMoveSign(HexaPoint point, Color4F signColor);
	void					releaseMoveSign();
	
	

	/* 멤버 검색 함수 */
	Unit*					getUnitByID(int unitID);
	Unit*					getUnitByPos(HexaPoint unitPos);


	/* 네트워크 관련 함수 */
	void					onGameStart(Packet::GameStartResult inPacket);
	void					ReadFieldBlock(FieldBlock fieldBlock[], int length);
	void					ReadUnitData(UnitData unitData[], int length);
	void					SetTurn(bool isMyTurn){ m_IsMyTurn = isMyTurn; }
	void					ReadActionQueue(UnitAction unitActionQueue[], int length);

private:
	Game					m_Game;
	GameState				m_GameState;
	bool					m_IsMyTurn;

	Player					m_Player[2];
	vector<Unit*>			m_UnitList;

	ScreenPoint				m_StartPoint;
	ScreenPoint				m_CursoredPoint;
	int						m_SelectedUnit;
	vector<DrawNode*>		m_CourseSignNode;
	vector<HexaPoint>		m_CourseStack;

	vector<Coord>			m_HexagonPoint;

	vector<Unit*>			m_UnitData;

};

#endif