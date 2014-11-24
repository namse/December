#pragma once
class Field
{
public:
	Field();
	~Field();

	void			InitField(int fieldWidth, int fieldHeight);
	bool			IsInsideOfField(Coord position);


	// TODO : 요 함수 두개 에러처리
	void			SetFieldType(Coord fieldCoord, FieldBlockType fieldType){ m_FieldBlockList.at(fieldCoord).m_Type = fieldType;}
	void			SetFieldStatus(Coord fieldCoord, FieldBlockStatus fieldStatus){ m_FieldBlockList.at(fieldCoord).m_Status = fieldStatus; }

	Coord			GetRandomBlock();
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

