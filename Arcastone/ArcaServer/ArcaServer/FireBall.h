#pragma once

#include "Skill.h"

class Game;
class FireBall : public Skill
{
public:
	FireBall(){}
	~FireBall(){}

	void ActSkill(Game* game, ActionData* skillData);
};