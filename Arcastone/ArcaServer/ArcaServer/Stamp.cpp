#include "stdafx.h"
#include "Stamp.h"
#include "Game.h"


bool Stamp::ActSkill(Game* game, ActionData* skillData)
{
	if (skillData->position[0].x == -1)
		return false;

	// JsonManager ���� ��ų ������ �޾ƿ��� ������� ������ ��

	Unit* caster = game->GetUnit(skillData->id);

	// TODO : ������ ��ġ�� ��������, position[0] �� ��ų����� ��ġ���� Ȯ���� ��

	Unit* target = game->GetUnitInPosition(skillData->position[0]);

	// ĳ��Ʈ ���ֿ��� ��ų �����ġ������ ������?
	HexaDirection direction = GetDirection(caster->GetPos(), target->GetPos());
	int power = skillData->skillRank + 1;
	int usingCost = 1;
	int* currentCost = game->GetCurrentCost();

	// �ּ� ������
	*currentCost = *currentCost - usingCost;

	target->UnitPush(game, power, direction);

	return true;
}