#pragma once

#include "Header.h"
#include "Unit.h"

class field
{
public:
	field();
	~field();

	map<Position, GameObject*>*	getGameObject(){ return &m_GameObject; }
	GameObject*					getGameObject(Position key);

private:
	map<Position, GameObject*>	m_GameObject;

};

