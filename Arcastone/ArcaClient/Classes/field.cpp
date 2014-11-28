#include "Field.h"


Field::Field()
{
}


Field::~Field()
{
}

void Field::Init(Layer* sceneLayer, FieldBlock fieldBlock[], int length, int mapWidth, int mapHeight, bool ReverseMap)
{
	m_FieldGrid = DrawNode::create();
	m_MapWidth = mapWidth;
	m_MapHeight = mapHeight;
	m_ReverseMap = ReverseMap;

	for (int i = 0; i < length; i++)
	{
		HexaPoint hexaPoint(fieldBlock[i].position);
		ScreenPoint anchor = HexaToScreen(hexaPoint);

		m_HexaPointList.push_back(hexaPoint);	// m_HexaPointList 에 화면에 그려지는 좌표들을 저장

		// 그리드 생성
		Hexagon* hexa = new Hexagon(anchor);
		m_FieldGrid->drawPolygon(&hexa->vertex[0], 6, ccc4f(0.0f, 0.0f, 0.0f, 0.0f), 1, COLOR_OF_GRID);
		
		// 블록 스프라이트 생성
		auto blockSprite = Sprite::create("sprite_none.png");

		// 맵 반전일 경우 x,y좌표가 낮을수록 z-index가 높음 
		int blockSpriteZidx;
		if (m_ReverseMap)
			blockSpriteZidx = (mapHeight - hexaPoint.y) * 2 + (mapWidth - hexaPoint.x);
		else
			blockSpriteZidx = hexaPoint.y*2 + hexaPoint.x;
		
		sceneLayer->addChild(blockSprite, blockSpriteZidx);
		m_FieldSpriteMap.insert(FieldSpriteMap::value_type(hexaPoint, blockSprite));

		SetFieldSprite(fieldBlock[i]);

		if (DRAW_HEXA_NUMBER)
		{
			// 인덱스 라벨 생성
			auto indexLabel = CreateText(hexaPoint);
			sceneLayer->addChild(indexLabel, 999);
		}
	}
	sceneLayer->addChild(m_FieldGrid, 100);
}

HexaPoint Field::ScreenToHexa(ScreenPoint point)
{
	// TODO : 헥사그리드 내에 있을 때만 검사하도록 수정
	// 입력한 점이 주어진 범위를 벗어나면 -1, -1 을 리턴
	/// 
	HexaPoint retPoint(-1, -1);

	// -맵크기의 3배 ~ 맵크기의 3배까지 검사한다
	for (int x = -m_MapWidth * 3; x < m_MapWidth * 3; ++x)
	{
		for (int y = -m_MapHeight * 3; y < m_MapHeight * 3; ++y)
		{
			// 그 임의의 점들 중에서 입력한 ScreenPoint를 포함하는 육각형의 중점을 찾는다
			ScreenPoint anchor = HexaToScreen(HexaPoint(x, y));
			if (IsInHexagon(point, anchor))
			{
				return HexaPoint(x, y);
			}
		}
	}

	return retPoint;
}

ScreenPoint Field::HexaToScreen(HexaPoint point)
{
	ScreenPoint retPoint;

	int xMidPoint = point.x - (m_MapWidth - 1)*0.5;
	int yMidPoint = point.y - (m_MapHeight - 1)*0.5;

	// MAP_START 를 중앙에 위치하도록 그려주기 위한 수식들
	if (m_ReverseMap)
	{
		retPoint.x = MAP_XSTART - HEXAGON_LENGTH * 1.5 * (xMidPoint);
		retPoint.y = MAP_YSTART + HEXAGON_LENGTH * 2 * sin(RADIANS_60) * (yMidPoint + xMidPoint * 0.5);
	}
	else
	{
		retPoint.x = MAP_XSTART + HEXAGON_LENGTH * 1.5 * (xMidPoint);
		retPoint.y = MAP_YSTART - HEXAGON_LENGTH * 2 * sin(RADIANS_60) * (yMidPoint + xMidPoint * 0.5);
	}

	return retPoint;
}

void Field::SetFieldSprite(FieldBlock fieldBlockData)
{
	HexaPoint anchor(fieldBlockData.position);
	Sprite* fieldBlock = m_FieldSpriteMap.find(anchor)->second;
	char imgNameBuf[20];
	FieldBlockType fieldType = fieldBlockData.type;
	FieldBlockStatus fieldStatus = fieldBlockData.status;

	switch (fieldStatus)
	{
	case FBS_NONE:
		break;
	case FBS_NORMAL:
	{
		switch (fieldType)
		{
		case FBT_NORMAL:
		{
			//strcpy(imgNameBuf, "block_water.png");
			char* imgName = "block_grass";
			sprintf_s(imgNameBuf, "%s%d.png", imgName, rand() % 6 + 1);
		}break;
		default:
			break;
		}
	}
		break;
	case FBS_HOLE:
		strcpy(imgNameBuf,"sprite_none.png");
		break;
	default:
		break;
	}
	
	fieldBlock->setTexture(imgNameBuf);
	float scale = HEXAGON_LENGTH * 2 / fieldBlock->getContentSize().width;
	fieldBlock->setScale(scale);
	fieldBlock->setAnchorPoint(Vec2(0.5f, 0.65f));
	ScreenPoint screenPoint = HexaToScreen(anchor);
	fieldBlock->setPosition(screenPoint);

	return;
}

LabelTTF* Field::CreateText(HexaPoint point)
{
	char szBuf[20];
	LabelTTF* vLabel;

	if (DRAW_HEXA_POSITION)
	{
		ScreenPoint screenPoint = HexaToScreen(point);
		sprintf_s(szBuf,_countof(szBuf),"%d,%d", (int)screenPoint.x, (int)screenPoint.y);
	}
	else
	{
		sprintf_s(szBuf, _countof(szBuf), "%d,%d", (int)point.x, (int)point.y);
	}

	vLabel = LabelTTF::create(szBuf, "Hevetica", 20);

	ScreenPoint screenPoint = HexaToScreen(point);
	vLabel->setPosition(screenPoint);
	vLabel->setColor(Color3B(0, 0, 0));

	return vLabel;
}

bool Field::IsInHexagon(ScreenPoint touch, ScreenPoint anchor)
{
	float msin = sin(RADIANS_60);

	// 클릭한 좌표가 육각형의 x 범위에 속하는지 확인
	if (touch.y > anchor.y - HEXAGON_LENGTH * msin &&
		touch.y < anchor.y + HEXAGON_LENGTH * msin)
	{

		float yHowFarToAnchor = abs(touch.y - anchor.y);

		// 육각형의 중심에서 y 거리로 멀어질수록 x 판정이 좁아지는 것을 구현하는 코드
		float yTuningValue = yHowFarToAnchor / (HEXAGON_LENGTH * msin) * 0.5;

		if (touch.x > anchor.x - HEXAGON_LENGTH * (1 - yTuningValue) &&
			touch.x < anchor.x + HEXAGON_LENGTH * (1 - yTuningValue))
			return true;
	}
	return false;
}

