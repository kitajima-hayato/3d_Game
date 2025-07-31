#pragma once
#include "engine/math/MyMath.h"

struct Stats {
	/// 座標
	Transform transform;
	/// 生存フラグ
	bool isAlive = true;
	/// 速度
	Vector3 velocity;
	/// 体力
	uint32_t health;
	/// 攻撃力
	int32_t attackPower;
};


class EnemyBace
{

public:
	/// <summary>
	/// Enemy クラスの仮想デストラクタ。派生クラスのオブジェクトが正しく破棄されることを保証
	/// </summary>
	virtual ~EnemyBace() = default;

	/// <summary>
	/// エネミーの初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// エネミーの更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// エネミーの描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// エネミーの動作処理
	/// </summary>
	virtual void Move() = 0;

	/// <summary>
	/// エネミーの攻撃処理
	/// </summary>
	virtual void Action() = 0;

protected:
	Stats stats; 
};

