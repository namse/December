#pragma once
class Field
{
public:
	Field();
	~Field();

	void		InitField(int fieldWidth, int fieldHeight);
	bool		SetObjectOnField(ObjectNumber objectNum, Coord setPos);

private:
	typedef std::map<Coord, FieldBlock*> FieldBlockList;
	FieldBlockList	m_FieldBlockList;
};

