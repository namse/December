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

	void					DrawExpectUnitMove(Unit* unit);
	void					drawUnitMove();
	void					drawMoveSign(HexaPoint point, Color4F signColor);
	void					HighlightHexagon(ScreenPoint position);
	void					releaseExpectMoveSign();
	void					releaseMoveSign();
	void					KnockBackDraw(Unit* attacker, Unit* target, HexaDirection direction, int range);
	
	void					onUnitAction(CCNode* sender = NULL);
	void					drawSkillEffect(Skill skill);
	

	/* 멤버 검색 함수 */
	Unit*					getUnitByID(int unitID);
	Unit*					getUnitByPos(HexaPoint unitPos);


	/* 네트워크 관련 함수 */
	void					onGameStart(Packet::GameStartResult inPacket);
	void					ReadFieldBlock(FieldBlock fieldBlock[], int length, int mapSizeX, int mapSizeY);
	void					ReadUnitData(UnitData unitData[], int length);
	void					SetTurn(bool isMyTurn);
	void					ReadActionQueue(Packet::AttackResult attackResult);
	void					ReadRestCost(Packet::CostRenewalResult recvData);
	void					UsingSkill(Unit* unit);

	void					gameOver(Packet::GameOverResult recvData);

private:
	HexaPoint				ScreenToHexa(ScreenPoint point);

	void					PrintUnitAction(UnitAction unitAction);

private:
	Game					m_Game;
	GameState				m_GameState;
	bool					m_IsMyTurn;
	bool					m_IsCursorMoved;
	bool					m_IsCastSkill;

	Player					m_Player[2];


	LabelTTF*				m_TurnLabel;
	LabelTTF*				m_CostLabel[MAX_HAVE_COST];

	ScreenPoint				m_StartPoint;
	ScreenPoint				m_CursoredPoint;
	int						m_SelectedUnit;

	HexaDirection			m_Direction;
	int						m_Range;
	int						m_MaxCosst;

	vector<HexaPoint>		m_HexagonPoint;

	vector<Unit*>			m_UnitList;
	vector<Unit*>			m_UnitData;

	vector<DrawNode*>		m_CourseSignNode;
	vector<DrawNode*>		m_ExpectSignNode;
	vector<HexaPoint>		m_CourseStack;
	queue<UnitAction>		m_UnitActionQueue;
};

#endif