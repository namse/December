#pragma once

#include "stdafx.h"

class Game;
class Skill
{
public:
	virtual bool ActSkill(Game* game, ActionData* actionData) = 0;

	UnitSkillType type;
	UnitSkillRank rank;
};

