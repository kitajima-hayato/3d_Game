#pragma once
#include "engine/math/MyMath.h"
class CameraController
{

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime);





public:
	// 位置のセット
	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }
	void SetCameraPosition(const Vector3& cameraPosition) { cameraPosition_ = cameraPosition; }
	// 位置の取得
	const Vector3& GetTargetPosition() const { return targetPosition_; }
	const Vector3& GetCameraPosition() const { return cameraPosition_; }
	// フォロー判定の設定
	void SetFollowRange(float minX, float maxX) { minX_ = minX; maxX_ = maxX; };

private:
	// 受け取る対象の位置
	Vector3 targetPosition_;

	// 受け取るカメラの位置
	Vector3 cameraPosition_;

	// デッドゾーン
	Vector2 deadZone_{ 2.0f, 1.0f };

	// 追従速度
	float followSpeed_ = 60.0f;

	// フォロー範囲 / デフォルトは無制限
	float minX_ = -100.0f;
	float maxX_ = 100.0f;

	float leftLimit_ = 10.0f;
	float rightLimit_ = 80.0f;
};

