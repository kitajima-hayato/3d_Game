#include "EnemyFactory.h"

std::unique_ptr<EnemyBace> EnemyFactory::CreateEnemy(const std::string& type)
{
	if(type == "NormalEnemy")
	{
		auto enemy = std::make_unique<NormalEnemy>();
		return enemy;
	}
	else if(type == "FlyingEnemy")
	{
		auto enemy = std::make_unique<FlyingEnemy>();
		return enemy;
	}
	return nullptr; // 対応するエネミーがない場合はnullptrを返す
}
