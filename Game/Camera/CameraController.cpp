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
    // 向きに応じた「目標オフセット」（理想値）
    float targetOffsetX = 0.0f;
    if (moveDirection > 0) {
        // 右に進むとき：カメラを少し右へ → プレイヤーは画面左寄り
        targetOffsetX = followOffsetX_;
    } else if (moveDirection < 0) {
        // 左に進むとき：カメラを少し左へ → プレイヤーは画面右寄り
        targetOffsetX = -followOffsetX_;
    }
    // moveDirection == 0 のときは 0（中央に戻る）

    // オフセットをなめらかに切り替え
    float ot = offsetLerpSpeed_ * deltaTime;
    if (ot > 1.0f) ot = 1.0f;
    currentOffsetX_ = currentOffsetX_ + (targetOffsetX - currentOffsetX_) * ot;

    // プレイヤー位置 + オフセット = 理想のカメラ中心
    float centerX = targetPosition_.x + currentOffsetX_;

    // カメラ中心をステージ範囲でクランプ
    if (centerX < leftLimit_)  centerX = leftLimit_;
    if (centerX > rightLimit_) centerX = rightLimit_;

    // カメラ位置をスムーズに追従
    float t = followSpeed_ * deltaTime;
    if (t > 1.0f) t = 1.0f;

    Vector3 desired = cameraPosition_;
    desired.x = centerX;

    cameraPosition_ = Lerp(cameraPosition_, desired, t);
}