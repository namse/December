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
	for (int posX = 0; posX < fieldWidth; ++posX)
	{
		int posY = 0 - (posX + 1) / 2;
		int max = (posX % 2) ? posY + fieldHeight : posY + fieldHeight - 1;
		
		for (posY; posY < max; ++posY)
		{
			FieldBlock fieldBlock;
			fieldBlock.m_Position = Coord(posX, posY);
			fieldBlock.m_Status = FBS_NONE;
			fieldBlock.m_Type = FBT_NONE;
			m_FieldBlockList.insert(FieldBlockList::value_type(fieldBlock.m_Position, fieldBlock));
		}
	}
}