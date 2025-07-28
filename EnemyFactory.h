#pragma once
#include "NormalEnemy.h"
#include <memory>

class EnemyFactory
{
public:
	/// <summary>
	/// エネミー発生処理
	/// </summary>
	static std::unique_ptr<EnemyBace> CreateEnemy(const std::string& type);



};

