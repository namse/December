#pragma once
class Field
{
public:
	Field();
	~Field();

	void		InitField(int fieldWidth, int fieldHeight);
	bool isInsideOfField(Coord position);

	FieldBlock GetFieldBlock(int i){ return m_FieldBlockList[i]; }
	int GetFieldBlockListSize(){ return m_FieldBlockListSize; }

private:
	int m_FieldBlockListSize;
	FieldBlock	m_FieldBlockList[MAX_FIELD_BLOCK];
};

