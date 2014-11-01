#pragma once

#include "Header.h"
#include "Unit.h"

class field
{
public:
	field();
	~field();

	map<Position, Unit*>*		getGameObject(){ return &m_GameObject; }
	Unit*						getGameObject(Position key);

private:
	map<Position, Unit*>		m_GameObject;

};

