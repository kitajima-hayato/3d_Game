// DeathEffect_ShakeAndZoom.cpp
#include "DeathEffect_ShakeAndZoom.h"
// #include "CameraManager.h"  // ← あなたのカメラ管理のヘッダに合わせて

void DeathEffect_ShakeAndZoom::Start(const Vector3& focusPos)
{
    focusPos_ = focusPos;
    t_ = 0;
    finished_ = false;

    // --- カメラ初期値の取得（★ここをあなたのAPIに置き換え） ---
    // 例:
    // auto* cam = CameraManager::Get()->GetMainCamera();
    // baseFovDeg_   = cam->GetFovDeg();
    // baseDistance_ = cam->GetDistance();
    // baseTarget_   = cam->GetTarget();
    // basePos_      = cam->GetPosition();

    // フォーカス点をプレイヤー位置へ
    // cam->SetTarget(focusPos_);

    // シェイクオフセット初期化（あるなら）
    // CameraManager::Get()->SetScreenShakeOffset({0,0});
}

void DeathEffect_ShakeAndZoom::Update()
{
    if (finished_) return;

    // 進行
    ++t_;
    int total = std::max(shakeFrames_, zoomFrames_);

    // --- シェイク（前半で減衰） ---
    if (t_ <= shakeFrames_) {
        float k = EaseOut(Clamp01((float)t_ / (float)shakeFrames_));
        float amp = 0.18f * (1.0f - k); // 初期強め→減衰
        float dx = Rand(-1.f, 1.f) * amp;
        float dy = Rand(-1.f, 1.f) * amp;

        // 画面揺れ（★ここをあなたのAPIに置き換え）
        // CameraManager::Get()->SetScreenShakeOffset({dx, dy});
    } else {
        // CameraManager::Get()->SetScreenShakeOffset({0,0});
    }

    // --- ズーム（FOV or 距離） ---
    float zt = EaseIn(Clamp01((float)t_ / (float)zoomFrames_));

    // FOV方式
    if (targetFovDeg_ > 0.0f) {
        float fov = Lerp(baseFovDeg_, targetFovDeg_, zt);
        // cam->SetFovDeg(fov); // ★置き換え
    }
    // 距離方式（FOVが無い場合はこちらだけ使う）
    if (targetDistance_ >= 0.0f) {
        float dist = Lerp(baseDistance_, targetDistance_, zt);
        // cam->SetDistance(dist); // ★置き換え
        // cam->SetTarget(focusPos_); // 念のためターゲット固定
    }

    // （必要なら）プレイヤー位置へ滑らかにターゲット補正
    // Vector3 tgt = LerpVec(baseTarget_, focusPos_, EaseOut(Clamp01((float)t_/(float)zoomFrames_)));
    // cam->SetTarget(tgt);

    if (t_ >= total) {
        // 後片付け
        Stop();
        finished_ = true;
        if (onComplete_) onComplete_();
    }
}

void DeathEffect_ShakeAndZoom::Stop()
{
    // CameraManager::Get()->SetScreenShakeOffset({0,0});

    // ズームを元に戻す必要があればここで（今回はSceneTransitionへ繋ぐので維持でもOK）
    // cam->SetFovDeg(baseFovDeg_);
    // cam->SetDistance(baseDistance_);
}
