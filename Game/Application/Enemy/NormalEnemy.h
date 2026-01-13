#pragma once
#include "EnemyBase.h"
/// 基底クラス<BaceEnemy>を継承
class NormalEnemy :
	public EnemyBase
{
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 動作処理
	/// </summary>
	void Move() override;
	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Action() override;



};

