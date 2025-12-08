#include "EnemyFactory.h"
#include <Logger.h>

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
	Logger::Log("\nEnemy generation failed\nNo matching data found\n		or\nSpelling error\n\n");
	return nullptr; // 対応するエネミーがない場合はnullptrを返す
}
