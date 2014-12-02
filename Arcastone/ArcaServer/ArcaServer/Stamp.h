#pragma once

#include "Skill.h"

class Game;
class Stamp : public Skill
{
public:
	Stamp(){}
	~Stamp(){}

	void ActSkill(Game* game, ActionData* skillData);
};