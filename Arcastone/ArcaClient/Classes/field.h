#pragma once

#include "Header.h"

class Field
{
public:
	Field();
	~Field();

	void					Init(Layer* sceneLayer, FieldBlock fieldBlock[], int length, int mapWidth, int mapHeight, bool ReverseMap);
	void					InitSpriteResource();

	HexaPoint				ScreenToHexa(ScreenPoint point);
	ScreenPoint				HexaToScreen(HexaPoint point);
	bool					IsThisHexaInField(HexaPoint point);
	bool					IsInHexagon(ScreenPoint touch, ScreenPoint anchor);

	void					SetFieldSprite(FieldBlock fieldBlockData);


private:
	LabelTTF*				CreateText(HexaPoint point);

private:
	int						m_MapWidth;
	int						m_MapHeight;
	bool					m_ReverseMap;

	typedef std::vector<HexaPoint> HexaPointList;
	typedef std::map<HexaPoint, Sprite*> FieldSpriteMap;
	HexaPointList			m_HexaPointList;
	FieldSpriteMap			m_FieldSpriteMap;
	DrawNode*				m_FieldGrid;

	typedef std::map<FieldBlockType, Animation*> AnimationList;
	CCSpriteFrameCache*		m_SpriteFrame;
	AnimationList			m_AnimationList;
};

