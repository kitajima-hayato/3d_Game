#pragma once
#include "engine/math/MyMath.h"
#include "engine/3d/Object3D.h"
#include <string>
#include "Game/Particle/EffectEmitter.h"
#include "Game/Collision/Collider.h"
#include "BlockType.h"


struct CollisionMapInfo {
	bool ceiling = false;	// 天井との衝突
	bool landing = false;	// 着地
	bool hitWall = false;	// 壁との接触
	Vector3 move;			// 移動量
};

enum class CollisionType {
	Top,
	Bottom,
	Left,
	Right
};

enum Corner {
	kRightBottom,	// 右下
	kRightTop,		// 右上
	kLeftTop,		// 左上
	kLeftBottom,	// 左下
	kNumCorners		// コーナーの数
};

struct PlayerParameter {
	// Playerのパラメータ構造体
	// 当たり判定
	float kWidth = 1.0f;	// 幅
	float kHeight = 1.0f;	// 高さ
	float blank = 0.01f;		// 余白
	float kEpsilon = 0.1f; // 微小量
	float kAcceleration = 0.1f; // 加速度

};

class Map;
class Player : public Collider
{
public: // 判定
	Type GetType() const override { return Type::Player; }
	AABB GetAABB() const override;
	void OnCollision(Collider* other)override;

	AABB CalcAABBAtPosition(const Vector3& pos);
public: // メソッド

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 移動入力統括処理
	/// </summary>
	void Move();

	/// <summary>
	/// 右への移動入力処理
	/// </summary>
	void MoveRight();

	/// <summary>
	/// 左への移動入力処理
	/// </summary>
	void MoveLeft();

	/// <summary>
	/// ジャンプ入力処理
	/// </summary>
	void Jump();

	/// <summary>
	/// Imguiの描画関数
	/// </summary>
	void DrawImgui();

	/// <summary>
	/// ブロックとの衝突判定
	/// </summary>
	void CheckBlockCollision();

	/// <summary>
	/// マップの衝突判定統括
	/// </summary>
	void MapCollision(CollisionMapInfo& mapInfo);


	/// <summary>
	/// マップの特定方向の衝突判定
	/// </summary>
	void CollisionMapInfoDirection(CollisionMapInfo& mapInfo, CollisionType dir, const std::array<Corner, 2>& CheckCorners, const Vector3& offset, std::function<bool(const CollisionMapInfo&)>moveCondition);

	/// <summary>
	/// 指定したコーナーの位置を取得
	/// </summary>
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	/// <summary>
	/// 指定した2点の間にブロックがあるかどうかを調べる
	/// </summary>
	bool CheackCollisionPoints(const std::array<Vector3, 2>& posList, CollisionType type, CollisionMapInfo& mapInfo);

	/// <summary>
	/// 指定したブロックタイプが当たり判定を持つかどうか
	/// </summary>
	bool IsHitTargetBlockType(BlockType type);

	/// <summary>
	/// 床との衝突判定
	/// </summary>
	void CeilingCollisionMove(const CollisionMapInfo& mapInfo);

	void LandingCollisionMove(const CollisionMapInfo& mapInfo);

	void WallCollisionMove(const CollisionMapInfo& mapInfo);

	/// <summary>
	/// 指定したブロックタイプが乗れるブロックかどうか
	/// </summary>
	bool IsGroundTile(BlockType type);

	/// <summary>
	/// 指定したブロックタイプがゴールブロックかどうか
	/// </summary>
	bool IsGoalTile(BlockType type);

public: // Setter / getter
	/// <summary>
	/// プレイヤーのSRTを設定
	/// </summary>
	void SetTransform(const Transform& transform) { this->transform = transform; }

	/// <summary>
	/// プレイヤーのSRTを取得
	/// </summary>
	const Transform& GetTransform() const { return transform; }

	/// <summary>
	/// プレイヤーの移動速度を設定
	/// </summary>
	void SetMoveSpeed(const Vector3& speed) { velocity = speed; }

	/// <summary>
	/// 判定
	/// </summary>
	void BeginFrameHitReset() { hitThisFrame = false; } // 毎フレーム冒頭で呼ぶ
	bool WasHitThisFrame() const { return hitThisFrame; }

	void DrawHitImgui();

	/// <summary>
	///  生存フラグの取得
	/// </summary>
	bool IsAlive() const { return isAlive; }

	/// <summary>
	/// マップチップデータをプレイヤーにも渡す
	/// </summary>
	void SetMapChipField(Map* mapChipField) { this->mapChipField = mapChipField; }

private:
	/// プレイヤーの生存フラグ
	bool isAlive = true;



	/// プレイヤーのSRT
	Transform transform;

	/// プレイヤーの移動速度
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	/// プレイヤーのダッシュ速度
	Vector3 dashSpeed = { 0.01f, 0.2f, 0.0f };
	/// プレイヤーの落下速度
	Vector3 fallSpeed = { 0.0f, -9.8f, 0.0f };
	
	/// プレイヤーのモデル
	std::unique_ptr<Object3D> playerModel;


	/// ダッシュ入力のカウントフレーム / 右
	float dashExtensionFrameRight = 0.0f;
	uint32_t dashInputRight = 0;

	/// ダッシュ入力のカウントフレーム / 左
	float dashExtensionFrameLeft = 0.0f;
	uint32_t dashInputLeft = 0;


	/// ダッシュ入力時の猶予フレーム
	const float dashInputMaxFrame = 18.0f;

	/*/// 一時的変数
	std::unique_ptr<EffectEmitter>qux;
	Transform emitterTransform;

	std::unique_ptr<EffectEmitter>quux;
	Transform quuxTransform;*/

	/// 判定用のAABB
	AABB aabb;

	/// 60 FPSを想定したデルタタイム
	float deltaTime = 0.016f;
	/// 
	bool hitThisFrame = false;

	/// 前フレームの座標
	Vector3 prevTranslate = { 0.0f, 0.0f, 0.0f };

	/// プレイヤーのパラメータ
	PlayerParameter playerParameter;
	/// Map
	Map* mapChipField = nullptr;
	/// 接地フラグ
	bool onGround = false;

	/// ゴールフラグ
	bool goalFlag = false;
};

