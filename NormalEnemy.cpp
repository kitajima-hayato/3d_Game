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
		{5.0f, -7.0f, 20.0f},
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
	/// モデルの初期化
	model = std::make_unique<Object3D>();
	model->Initialize();
	model->SetModel("Tentativeenemy.obj");
	model->SetTransform(stats.transform);

}

void NormalEnemy::Update()
{
	/// 更新処理
	Move();
	Action();
	model->SetTransform(stats.transform);
	model->Update();
	/// @衝突判定
	//CollisionManager::GetInstance()->AddCollider(model->GetCollider());
	//CollisionManager::GetInstance()->CheckAllCollisions();
	//CollisionManager::GetInstance()->Clear();
}

void NormalEnemy::Draw()
{
	/// 描画処理
	model->Draw();
	//CollisionManager::GetInstance()->Draw();
	//ImGui::Text("Enemy");
	//ImGui::DragFloat3("Translate", &stats.transform.translate.x, 0.1f);
	//ImGui::DragFloat3("Scale", &stats.transform.scale.x, 0.1f);
	//ImGui::DragFloat3("Rotate", &stats.transform.rotate.x, 0.1f);
}

void NormalEnemy::Move()
{

}

void NormalEnemy::Action()
{
}
