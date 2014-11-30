#pragma once

#include "Skill.h"

class Game;
class FireBall : public Skill
{
public:
	FireBall(){}
	~FireBall(){}

	bool ActSkill(Game* game, AttackData* skillData);
};