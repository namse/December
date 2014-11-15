#pragma once

#include "Header.h"
#include "Unit.h"

class Group
{
public:
	Group();
	~Group();

private:
	vector<Unit*> m_Unit;

};

