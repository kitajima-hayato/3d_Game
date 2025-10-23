#include "NormalEnemy.h"

void NormalEnemy::Initialize()
{
	/// @仮スタッツ
	stats = {
		// scale
		{{1.0f, 1.0f, 1.0f},
		// Rotate
		{0.0f, 0.0f, 0.0f},
		// Translate
		
		},
		// 生存フラグ
		true,
		// 速度
		{0.1f, 0.1f, 0.0f},
		// 体力
		1,
		// 攻撃力
		1
	};

	/// Object3Dの初期化
	model = std::make_unique<Object3D>();
	model->Initialize();
	/// モデルの設定
	model->SetModel("Tentativeenemy.obj");
	/// トランスフォームの設定
	model->SetTransform(stats.transform);
}

void NormalEnemy::Update()
{
	if (stats.isAlive) {
		/// 挙動処理
		Move();
		/// 攻撃処理
		Action();
		/// モデルの更新
		model->Update();
	}
	
}

void NormalEnemy::Draw()
{
	if (stats.isAlive) {
		/// モデルの描画
		model->Draw();
	}
}

void NormalEnemy::Move()
{
}

void NormalEnemy::Action()
{
}
