#pragma once
#include "engine/math/MyMath.h"
#include <memory>
#include "engine/3d/Object3D.h"
#include "Map.h"
/// プレイヤークラス

/// <summary>
/// 向き
/// </summary>
enum class Direcrion {
	kRight,
	kLeft
};

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionMapInfo {
	// 天井衝突
	bool celling = false;
	// 床衝突
	bool landing = false;
	// 壁衝突
	bool hitWall = false;
	// 移動量
	Vector3 move;
};

/// <summary>
/// コーナー
/// </summary>
enum Corner {
	// 右下
	kRightBottom,
	// 左下
	kLeftBottom,
	// 右上
	kRightTop,
	// 左上
	kLeftTop,
	// 要素数
	kNumCorners
};

/// <summary>
/// 衝突タイプ
/// </summary>
enum class CollisionType {
	kTop,
	kBottom,
	kLeft,
	kRight
};

/// プレイヤーのステータス　
struct PlayerStatus {
	//速度パラメーター
	// 加速度
	float kAcceleration = 0.05f;
	// 速度減衰率
	float kAttenuation = 0.2f;
	// 最大速度
	float kMaxSpeed = 2.0f;

	//ジャンプパラメータ
	// 重力加速度
	float kGravity = 0.05f;
	// 最大落下速度
	float kMaxFallSpeed = 1.0f;
	// ジャンプ初速度
	float kJumpPower = 0.5f;
	// 溜め時間
	float kMaxChargeTime = 0.2f;

	//当たり判定パラメータ
	// 幅
	float kWidth = 0.8f;
	// 高さ
	float kHeight = 0.8f;
	// 当たり判定の余裕
	float kBlank = 2.0f;
	// 微小値
	float kEpsilon = 0.1f;

	//減衰パラメーター
	// 着地時の減衰率
	float kAttenuationLanding = 0.1f;
	// 壁に衝突したときの減衰率
	float kAttenuationWall = 1.0f;

	//振り向きパラメーター
	// 角度の補間タイム
	float kTurnTime = 1.0f;
};

class Player
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	/// プレイヤーの挙動更新統括 / 判定 / 移動 / その他
	void UpdateBehavior();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 右移動処理
	/// @see Move
	/// </summary>
	void MoveRight();

	/// <summary>
	/// 左移動処理
	/// @see Move
	/// </summary>
	void MoveLeft();

	/// <summary>
	/// ImGui表示
	/// </summary>
	void ImGui();

	/// <summary>
	/// マップ衝突チェック
	/// </summary>
	void MapCollision(CollisionMapInfo& collisionInfo);

	/// <summary>
	/// 衝突マップ情報取得
	/// </summary>
	/// <param name="collisionInfo"></param>
	/// <param name="type"></param>
	/// <param name="checkCorners"></param>
	/// <param name="offset"></param>
	/// <param name="mooveCondition"></param>
	void CollisionMapInfoDirection(
		CollisionMapInfo& collisionInfo, 
		CollisionType type, 
		const std::array<Corner, 2>& checkCorners, 
		const Vector3& offset, 
		std::function<bool(const CollisionMapInfo&)>mooveCondition);

	/// <summary>
	/// コーナー位置取得
	/// </summary>
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	/// <summary>
	/// 当たり判定ポイントチェック
	/// </summary>
	/// <param name="posList"></param>
	/// <param name="type"></param>
	/// <param name="collisionInfo"></param>
	bool CheckCollisionPoints(const std::array<Vector3, 2>& posList, CollisionType type, CollisionMapInfo& collisionInfo);


	bool IsHitBlockTable(BlockType type);

	bool IsHitGoalBlockTable(BlockType type);


public:
	void SetDeathHeight(float deathHeight) { deathHeight_ = deathHeight; }

private:	// メンバ変数
	// プレイヤーステータス
	PlayerStatus status_;
	// 速度
	Vector3 velocity_ = {};

	// 死亡判定の高さ / 画面外に出たら死亡判定とする
	float deathHeight_ = -10.0f;

	// プレイヤーのモデル
	std::unique_ptr<Object3D> playerModel_ = nullptr;

	// 死亡フラグ
	bool isDead_ = false;

	// マップ
	Map* map_ = nullptr;

	bool isGoal_ = false;

};

