#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "Header.h"

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
	virtual void			onTouchMoved(Touch* touch, Event* unused_event){}
	virtual void			onTouchCancelled(Touch* touch, Event* unused_event){}
	virtual void			onTouchEnded(Touch* touch, Event *unused_event);

	void					onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {}
	void					onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	Hexagon*				createHexagon(Point anchor, int size);
	void					drawHexagon();

private:
	map<int, CCSprite*>		m_PeerMap;
	map<Point, Hexagon*>	m_HexaMap;

	bool					m_IsClicked;
};

#endif // __HELLOWORLD_SCENE_H__
