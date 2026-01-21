#pragma once
#include <memory>

/// エネミー全てのファクトリークラス
#include "EnemyBase.h"
#include "NormalEnemy.h"
#include "FlyingEnemy.h"
class EnemyFactory
{
public:
	/// <summary>
	///  エネミーを生成するファクトリーメソッド
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	static std::unique_ptr<EnemyBase> CreateEnemy(const std::string& type);

};

