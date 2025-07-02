#pragma once
#include "engine/math/MyMath.h"
#include "engine/3d/Object3D.h"
#include <string>
#include "Game/Particle/EffectEmitter.h"
class Player
{

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

public: // Setter / getter
	/// <summary>
	/// プレイヤーのSRTを設定
	/// </summary>
	/// <param name="transform">変形情報</param>
	void SetTransform(const Transform& transform) { this->transform = transform; }
	/// <summary>
	/// プレイヤーのSRTを取得
	/// </summary>
	/// <returns>プレイヤーのSRT</returns>
	const Transform& GetTransform() const { return transform; }

	/// <summary>
	/// プレイヤーの移動速度を設定
	/// </summary>
	void SetMoveSpeed(const Vector3& speed) { moveSpeed = speed; }

private:
	/// プレイヤーのSRT
	Transform transform;

	/// プレイヤーの移動速度
	Vector3 moveSpeed = { 0.1f, 0.1f, 0.0f };
	/// プレイヤーのダッシュ速度
	Vector3 dashSpeed = { 0.01f, 0.2f, 0.0f };
	/// プレイヤーの落下速度
	Vector3 fallSpeed = { 0.0f, -9.8f, 0.0f };
	/// プレイヤーの移動方向
	Vector3 moveDirection = { 0.0f, 0.0f, 0.0f };


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

	/// 一時的変数
	std::unique_ptr<EffectEmitter>qux;
	Transform emitterTransform;

	std::unique_ptr<EffectEmitter>quux;
	Transform quuxTransform;
	
};

