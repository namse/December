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
	int GetFieldSizeX(){ return m_FieldSizeX; }
	int GetFieldSizeY(){ return m_FieldSizeY; }

private:
	int m_FieldBlockListSize;
	int m_FieldSizeX;
	int m_FieldSizeY;
	FieldBlock	m_FieldBlockList[MAX_FIELD_BLOCK];
};

