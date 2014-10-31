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
			FieldBlock* fieldBlock = new FieldBlock();
			fieldBlock->objectNum = 0;
			fieldBlock->position.x = posX;
			fieldBlock->position.y = posY;
			m_FieldBlockList.insert(FieldBlockList::value_type(fieldBlock->position,fieldBlock));
		}
	}
}

bool Field::SetObjectOnField(ObjectNumber objectNum, Coord setPos)
{
	FieldBlockList::iterator blockIter = m_FieldBlockList.find(setPos);
	if (m_FieldBlockList.end() == blockIter)
	{
		return false;
	}

	FieldBlock* fieldBlock = blockIter->second;
	fieldBlock->objectNum = objectNum;

	return true;
}

