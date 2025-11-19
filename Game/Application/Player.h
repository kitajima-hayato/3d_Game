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
	float kMaxSpeed = 0.1f;

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
	float kEpsilon = 0.05f;

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
	/// <summary>
	/// プレイヤーの挙動更新統括 / 判定 / 移動 / その他
	/// </summary>
	void UpdateBehavior();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <sumary>
	/// ジャンプ処理
	/// </sumary>
	void Jump();

	/// <summary>
	/// ImGui表示
	/// </summary>
	void ImGui();

	/// <summary>
	/// マップ衝突チェック
	/// </summary>
	void MapCollision(CollisionMapInfo& collisionInfo);

	/// <sumary>
	/// 天井衝突チェック
	///　天井に衝突したら移動量を調整
	///　</sumary>
	void CellingCollisionMove(CollisionMapInfo& collisionInfo);

	/// <sumary>
	/// 床衝突チェック
	/// 床に衝突したら移動量を調整
	/// </sumary>
	void LandingCollisionMove(CollisionMapInfo& collisionInfo);

	/// <summary>
	/// 壁衝突チェック
	/// </summary>
	/// <param name="collisionInfo"></param>
	void WallCollisionMove(CollisionMapInfo& collisionInfo);

	/// <summary>
	/// 衝突マップ情報取得
	/// </summary>
	/// <param name="info"></param>
	void PlayerCollisionMove(const CollisionMapInfo& info);


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


	/// <summary>
	/// 当たり判定ブロックテーブル
	/// </summary>
	/// <param name="type">指定のブロックは判定を取るか</param>
	/// <returns>当たっているかいないか</returns>
	bool IsHitBlockTable(BlockType type);

	/// <summary>
	/// ゴールブロック当たり判定テーブル
	/// </summary>
	/// <param name="type">ゴール判定のあるブロック</param>
	/// <returns>ゴール判定のあるブロックかどうか</returns>
	bool IsHitGoalBlockTable(BlockType type);

	/// <summary>
	/// デバッグ用プレイヤー死亡時リセット
	/// </summary>
	void DebugPlayerReset();


public:	/// Setter / Getter
	// 死亡判定の高さを設定
	void SetDeathHeight(float deathHeight) { deathHeight_ = deathHeight; }

	// マップのセット
	void SetMap(Map* map) { map_ = map; }
private:	// メンバ変数
	// プレイヤーステータス
	PlayerStatus status_;
	// 速度
	Vector3 velocity_ = {};

	// 死亡判定の高さ / 画面外に出たら死亡判定とする
	float deathHeight_;

	// プレイヤーのモデル
	std::unique_ptr<Object3D> playerModel_ = nullptr;

	// 死亡フラグ
	bool isDead_ = false;

	// マップ
	Map* map_ = nullptr;
	// ゴールフラグ
	bool isGoal_ = false;

	// ジャンプ
	bool onGround_ = true;



};

