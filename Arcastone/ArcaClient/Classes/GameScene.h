#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "Header.h"
#include "Game.h"
#include "Unit.h"

///# 이것도 코딩 컨벤션 문제.. "{"를 네임 뒤에 붙일 것인가 줄내림 할 것인가는.. 전체적으로 통일성 있게 맞출 것.
enum GameState{
	GS_BEFORE_LOGIN,
	GS_WAIT_GAME,
	GS_GAME_START,
};

///# 전체적으로 게임씬에 대부분의 기능을 다 때려박은 분위기? 나중에 유지 보수하기 힘들어지니까.. 조금씩 리팩토링 할 것

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
	void					onGameStart(Packet::GameStartResult inPacket); ///< 어떤 멤버는 대문자로.. 어떤 멤버는 소문자로 시작하는 것은 ㄴㄴ 팀만의 코딩 컨벤션 맞출것
	void					ReadFieldBlock(FieldBlock fieldBlock[], int length, int mapSizeX, int mapSizeY); ///< 가변 크기 배열 넘기는것은 비추.. 반드시 바운드 체크 할 것. std::array를 쓰거나 포인터를 넘기던가.
	void					ReadUnitData(UnitData unitData[], int length);
	void					SetTurn(bool isMyTurn);
	void					ReadActionQueue(Packet::AttackResult attackResult);  ///< const AttackResult& ... 사용
	void					ReadRestCost(Packet::CostRenewalResult recvData); ////< 마찬가지
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


	LabelTTF*				m_TurnLabel;				///< UI 객체와 게임 콘텐츠 분리.. 전체적으로 하나의 클래스 내에 렌더링이 관련된 녀석들이랑 게임 콘텐츠에 관련된 녀석이 많이 섞여 있음.
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