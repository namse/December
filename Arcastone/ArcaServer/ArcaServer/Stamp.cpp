#include "stdafx.h"
#include "Stamp.h"
#include "Game.h"


bool Stamp::ActSkill(Game* game, ActionData* skillData)
{
	if (skillData->position[0].x == -1)
		return false;

	// JsonManager 에서 스킬 데이터 받아오는 방식으로 구현할 것

	Unit* caster = game->GetUnit(skillData->id);

	// TODO : 유닛의 위치를 기준으로, position[0] 이 스킬사용한 위치인지 확인할 것

	Unit* target = game->GetUnitInPosition(skillData->position[0]);

	// 캐스트 유닛에서 스킬 대상위치까지의 방향은?
	HexaDirection direction = GetDirection(caster->GetPos(), target->GetPos());
	int power = skillData->skillRank + 1;
	int usingCost = 1;
	int* currentCost = game->GetCurrentCost();

	// 주소 역참조
	*currentCost = *currentCost - usingCost;

	target->UnitPush(game, power, direction);

	return true;
}