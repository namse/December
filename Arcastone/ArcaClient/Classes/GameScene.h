#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "Header.h"
#include "Unit.h"

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

	CREATE_FUNC(GameScene);

	void					touchEventInit();
	EventListenerTouchOneByOne*	_touchListener;
	virtual bool			onTouchBegan(Touch* touch, Event* event);
	virtual void			onTouchMoved(Touch* touch, Event* unused_event);
	virtual void			onTouchCancelled(Touch* touch, Event* unused_event){}
	virtual void			onTouchEnded(Touch* touch, Event *unused_event);

	bool					touchCheck(Point touch, Point anchor);

	Hexagon*				createHexagon(Point anchor, int size);
	void					drawHexagon();
	void					drawText(int i, int j, Hexagon* hexa);

	Point					pointConversion(Point point);
	bool					drawToRect(float y);
	bool					drawToHexa(int x, int y);

	void					ReadUnitData(UnitData* unitData, int length);
	// Header 안에서 typedef Packet::GameStartResult::UnitData UnitData 했음 .

private:
	map<int, CCSprite*>		m_PeerMap;
	
	int						m_Length;

	vector<Point>			m_HexagonPoint;

	vector<Point>			m_PointPath;
	int						m_PointPathIndex;
	bool					m_IsClicked;
};

#endif // __HELLOWORLD_SCENE_H__
