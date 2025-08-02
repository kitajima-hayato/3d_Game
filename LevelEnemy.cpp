#include "LevelEnemy.h"

void LevelEnemy::Initialize()
{
	// 初期化処理
	enemyObject = std::make_unique<Object3D>();
	enemyObject->SetModel("EnemySpawn.obj");
	// モデルの設定
	enemyObject->Initialize();

	// Transformの設定
	//enemyObject->SetTransform(transform);

}


void LevelEnemy::Update()
{
	// 更新処理
	enemyObject->Update();
}


void LevelEnemy::Draw()
{
	// 描画処理
	enemyObject->Draw();
}