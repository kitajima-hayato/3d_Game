#include "EnemyFactory.h"

std::unique_ptr<EnemyBace> EnemyFactory::CreateEnemy(const std::string& type)
{
	if (type == "Normal") {
		return std::make_unique<NormalEnemy>();
	}

	return nullptr; // 未知のエネミータイプ
}
