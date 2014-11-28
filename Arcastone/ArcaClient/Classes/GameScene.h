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
	void					InitCostLabel();
	CREATE_FUNC(GameScene);



	void					OnUnitAction(CCNode* sender = NULL);



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

	/* draw 함수 */
	void					DrawExpectUnitMove(Unit* unit);
	void					DrawUnitMove();
	void					DrawMoveSign(HexaPoint point, Color4F signColor);
	void					HighlightHexagon(ScreenPoint position);
	void					releaseExpectMoveSign();
	void					ReleaseMoveSign();
	void					KnockBackDraw(Unit* attacker, Unit* target, HexaDirection direction, int range);
	void					DrawSkillEffect(Skill skill);
	void					DrawCursorSign();
	

	/* 멤버 검색 함수 */
	Unit*					GetUnitByID(int unitID);
	Unit*					GetUnitByPos(HexaPoint unitPos);


	/* 네트워크 관련 함수 */
	void					OnGameStart(Packet::GameStartResult inPacket);
	void					ReadUnitData(UnitData unitData[], int length);
	void					SetTurn(bool isMyTurn);
	void					ReadActionQueue(Packet::AttackResult attackResult);
	void					ReadRestCost(Packet::CostRenewalResult recvData);
	void					UsingSkill(Unit* unit);
	void					GameOver(Packet::GameOverResult recvData);
	void					ChangeFieldType(Packet::ChangeFieldResult recvData);

private:

	void					PrintUnitAction(UnitAction unitAction);

private:
	Game					m_Game;
	GameState				m_GameState;
	bool					m_IsMyTurn;
	bool					m_IsCursorMoved;
	bool					m_IsCastSkill;
	Field					m_Field;

	Player					m_Player[2];

	LabelTTF*				m_TurnLabel;
	LabelTTF*				m_CostLabel[MAX_HAVE_COST];

	ScreenPoint				m_StartPoint;
	ScreenPoint				m_CursoredPoint;
	ScreenPoint				m_SelectedUnitPoint;
	int						m_SelectedUnit;

	HexaDirection			m_Direction;
	int						m_Range;
	int						m_MaxCosst;

	vector<Unit*>			m_UnitList;
	vector<Unit*>			m_UnitData;

	vector<DrawNode*>		m_CourseSignNode;
	vector<DrawNode*>		m_ExpectSignNode;
	vector<HexaPoint>		m_CourseStack;
	queue<UnitAction>		m_UnitActionQueue;

	DrawNode*				m_CursorSignNode;
};

#endif