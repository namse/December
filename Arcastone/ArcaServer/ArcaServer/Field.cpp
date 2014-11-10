#include "stdafx.h"
#include "Field.h"


Field::Field()
{

}


Field::~Field()
{	
}

void Field::InitField(int fieldWidth, int fieldHeight)
{
	// TODO : 맵의 형태 불러와서 그거대로 FieldBlock 찎어내기
	// 일단은 육각형 형태로 잘라냄..

	int i = 0;

	for (int x = 0; x < MAP_FIELD_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_FIELD_HEIGHT; ++y)
		{
			if (x + y >= MAP_FIELD_WIDTH / 2 &&
				x + y <= MAP_FIELD_WIDTH / 2 + MAP_FIELD_HEIGHT - 1)
			{
				m_FieldBlockList[i].m_Position = Coord(x, y);
				m_FieldBlockList[i].m_Status = FBS_NONE;
				m_FieldBlockList[i].m_Type = FBT_NONE;
				i++;
			}
		}
	}
	m_FieldBlockListSize = i;
}

bool Field::isInsideOfField(Coord position)
{
	for (int i = 0; i < m_FieldBlockListSize; ++i)
	{
		if (m_FieldBlockList[i].m_Position == position)
			return true;
	}
	return false;
}