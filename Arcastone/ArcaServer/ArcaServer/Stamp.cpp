#include "stdafx.h"
#include "Stamp.h"
#include "Game.h"


void Stamp::ActSkill(Game* game, ActionData* skillData)
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

	int beforeCost = game->GetPlayerList()[caster->GetOwner()].GetCurrentCost();
	game->GetPlayerList()[caster->GetOwner()].SetCurrentCost(beforeCost - usingCost);

	caster->UnitPush(game, target, power, direction);

	return;
}