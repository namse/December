#pragma once

#include "stdafx.h"

class Game;
class Skill
{
public:
	virtual void ActSkill(Game* game, ActionData* actionData) = 0;

	UnitSkillType type;
	UnitSkillRank rank;
};

