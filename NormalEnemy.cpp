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
}

void NormalEnemy::Update()
{
}

void NormalEnemy::Draw()
{
}

void NormalEnemy::Move()
{
}

void NormalEnemy::Action()
{
}
