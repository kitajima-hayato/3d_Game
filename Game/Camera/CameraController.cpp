#include "CameraController.h"

static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
{
	return a + (b - a) * t;
}

void CameraController::Initialize()
{
}


void CameraController::Update(float deltaTime)
{
    // プレイヤーXを制限
    float targetX = targetPosition_.x;

    // --- 左制限 ---
    if (targetX < leftLimit_) {
        targetX = leftLimit_;  // 左固定
    }

    // --- 右制限 ---
    if (targetX > rightLimit_) {
        targetX = rightLimit_; // 右固定
    }

    // -------- dead zone を使ったフォロー部分 --------
    Vector3 desired = cameraPosition_;
    float dx = targetX - cameraPosition_.x;

    if (std::abs(dx) > deadZone_.x) {
        if (dx > 0) {
            desired.x = targetX - deadZone_.x;
        } else {
            desired.x = targetX + deadZone_.x;
        }
    }

    // -------- スムース追従（Lerp） --------
    float t = followSpeed_ * deltaTime;
    if (t > 1.0f) t = 1.0f;

    cameraPosition_ = Lerp(cameraPosition_, desired, t);
}
