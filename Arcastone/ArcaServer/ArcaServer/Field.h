#pragma once
class Field
{
public:
	Field();
	~Field();

	void		InitField(int fieldWidth, int fieldHeight);
	void		SetUnitOnField(UnitNumber unitId);

private:
	int			m_FieldWidth;
	int			m_FieldHeight;
	int**		m_FieldArr;
};

