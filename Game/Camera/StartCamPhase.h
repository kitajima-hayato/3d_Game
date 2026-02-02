#pragma once
#include "MyMath.h"
#include "Game/Camera/Camera.h"
#include <algorithm>
class Camera;
class StartCamPhase
{
public:
	// スタート演出
	enum class Phase {
		None,
		MoveToLeft,   // 左端へ移動（初期位置から）
		PanToRight,   // 左→右へパン
		Hold,         // 少し止める（任意）
		ReturnToStart // プレイヤー開始地点へ戻る
	};

public:
	void Bind(Camera* camera, Transform* cameraTransform);

	void Start();

	void Update(float dt);

	void Skip();

	bool IsRunning()const;

	Phase GetCurrentPhase() const { return phase_; }

	void DrawImgui();


private:
	static inline float EaseOutCubic(float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return 1.0f - std::pow(1.0f - t, 3.0f);
	}
	static inline float EaseOutBack(float t, float s = 1.70158f) {
		t = std::clamp(t, 0.0f, 1.0f);
		return 1.0f + (s + 1.0f) * std::pow(t - 1.0f, 3.0f) + s * std::pow(t - 1.0f, 2.0f);
	}
	static inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
		return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
	}
private:
	// 
	Camera* camera_ = nullptr;
	Transform* cameraTransform_;

	Phase phase_ = Phase::None;
	float timer_ = 0.0f;

	// 開始地点
	Vector3 startPos_ = {};

	/// 横パン演出
	// 目標位置
	Vector3 camTargetPos_ = { 8.0f,3.5f,-20.0f };
	// 左端へ行く時間
	float introMoveDur_ = 0.6f;
	// 左から右パン時間
	float introPanDur_ = 1.6f;
	// 右端での停止時間
	float introHoldDur_ = 1.0f;
	// 開始地点へ戻る時間
	float introReturnDur_ = 3.0f;

	// 左端と右端
	float introLeftX_ = 8.0f;
	float introRightX_ = 92.0f;
	// 背景が崩壊しない高さ
	float introFixedY_ = 3.5f;
	// 固定Z座標 / 通常距離
	float introFixedZ_ = -20.0f;
};

