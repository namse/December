#include "stdafx.h"
#include "FireBall.h"
#include "Game.h"


void FireBall::ActSkill(Game* game, ActionData* skillData)
{
	if (skillData->position[0].x == -1)
		return;

	// JsonManager ���� ��ų ������ �޾ƿ��� ������� ������ ��

	Unit* caster = game->GetUnit(skillData->id);
	
	// TODO : ������ ��ġ�� ��������, position[0] �� ��ų����� ��ġ���� Ȯ���� ��

	Unit* target = game->GetUnitInPosition(skillData->position[0]);
	
	// ĳ��Ʈ ���ֿ��� ��ų �����ġ������ ������?
	HexaDirection direction = GetDirection(caster->GetPos(), target->GetPos());
	int power = skillData->skillRank + 1;
	int usingCost = 1;

	*(game->GetPlayerList()[caster->GetOwner()].GetCurrentCost()) -= usingCost;

	caster->UnitPush(game, target, power, direction);
}