#include "stdafx.h"
#include "Field.h"


Field::Field() : m_FieldWidth(0), m_FieldHeight(0)
{

}


Field::~Field()
{
}

void Field::InitField(int fieldWidth, int fieldHeight)
{
	m_FieldWidth = fieldWidth;
	m_FieldHeight = fieldHeight;

	// 다차원 배열 초기화
	m_FieldArr = new int*[fieldHeight];

	for (int i = 0; i < fieldHeight; ++i)
	{
		m_FieldArr[i] = new int[fieldWidth];
	}

}

