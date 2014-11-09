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
		int max = (posX % 2) ? posY + fieldHeight-1 : posY + fieldHeight;
		
		for (posY; posY < max; ++posY)
		{
			FieldBlock fieldBlock;
			fieldBlock.m_Status = FBS_NONE;
			fieldBlock.m_Type = FBT_NONE;
			m_FieldBlockList.insert(FieldBlockList::value_type(Coord(posX, posY), fieldBlock));
		}
	}
}