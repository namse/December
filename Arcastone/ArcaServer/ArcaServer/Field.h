#pragma once
class Field
{
public:
	Field();
	~Field();

	void		InitField(int fieldWidth, int fieldHeight);
	bool isInsideOfField(Coord position){
		if (m_FieldBlockList.find(position) != m_FieldBlockList.end())
			return true;
		return false;
	}

private:
	typedef std::map<Coord, FieldBlock> FieldBlockList;
	FieldBlockList	m_FieldBlockList;
};

