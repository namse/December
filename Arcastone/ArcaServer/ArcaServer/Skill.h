#pragma once

#include "stdafx.h"

class Game;
class Skill
{
public:
	virtual bool ActSkill(Game* game, AttackData* attackData) = 0;

	UnitSkillType type;
	UnitSkillRank rank;
};

