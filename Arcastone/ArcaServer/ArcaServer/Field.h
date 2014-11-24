#pragma once
class Field
{
public:
	Field();
	~Field();

	void			InitField(int fieldWidth, int fieldHeight);
	bool			IsInsideOfField(Coord position);

	void			SetFieldType(Coord fieldCoord, FieldBlockType fieldType){ m_FieldBlockList.find(fieldCoord)->second.m_Type = fieldType;}
	void			SetFieldStatus(Coord fieldCoord, FieldBlockStatus fieldStatus){ m_FieldBlockList.find(fieldCoord)->second.m_Status = fieldStatus; }

	void			GetFieldBlockList(OUT FieldBlock fieldBlockArr[]);
	FieldBlock		GetFieldBlock(Coord fieldCoord){ return m_FieldBlockList.find(fieldCoord)->second; }
	int				GetFieldBlockListSize(){ return m_FieldBlockList.size(); }
	int				GetFieldSizeX(){ return m_FieldSizeX; }
	int				GetFieldSizeY(){ return m_FieldSizeY; }

private:
	int				m_FieldSizeX;
	int				m_FieldSizeY;

	typedef std::map<Coord, FieldBlock> FieldBlockList;
	FieldBlockList	m_FieldBlockList;
};

