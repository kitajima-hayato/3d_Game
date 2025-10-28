// DeathEffect_ShakeAndZoom.h
#pragma once
#include <functional>
#include <random>
#include "engine/math/MyMath.h"

class DeathEffect_ShakeAndZoom {
public:
    // shakeFrames: 揺れるフレーム数, zoomFrames: ズームにかけるフレーム数
    // targetFovDeg: 目標FOV（FOVが無ければ -1 を渡し、distance方式を使う）
    // targetDistance: 目標距離（distanceが無ければ <0 を渡し、FOV方式を使う）
    DeathEffect_ShakeAndZoom(int shakeFrames, int zoomFrames,
        float targetFovDeg = 35.0f,
        float targetDistance = -1.0f)
        : shakeFrames_(shakeFrames), zoomFrames_(zoomFrames),
        targetFovDeg_(targetFovDeg), targetDistance_(targetDistance) {
    }

    void Start(const Vector3& focusPos);
    void Update();
    void Stop(); // 早期終了や後片付け
    bool IsFinished() const { return finished_; }
    void SetOnComplete(std::function<void()> cb) { onComplete_ = std::move(cb); }

private:
    // === ユーティリティ ===
    static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
    static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
    static inline float EaseOut(float t) { return 1.0f - (1.0f - t) * (1.0f - t); } // シェイク減衰
    static inline float EaseIn(float t) { return t * t * t; }                          // ズーム加速

    float Rand(float a, float b) {
        static std::mt19937 mt{ std::random_device{}() };
        std::uniform_real_distribution<float> d(a, b); return d(mt);
    }

private:
    // 入力
    int   shakeFrames_ = 18;
    int   zoomFrames_ = 24;
    float targetFovDeg_ = 35.0f; // FOV方式
    float targetDistance_ = -1.0f; // 距離方式

    // 状態
    int   t_ = 0;
    bool  finished_ = false;
    Vector3 focusPos_{};

    // カメラ初期値の保存
    float  baseFovDeg_ = 60.0f; // ← ここは実際の初期FOVを Start() 時に取得
    float  baseDistance_ = 5.0f;  // ← ここも Start() で取得（距離ドリーを使う場合）
    Vector3 baseTarget_{};
    Vector3 basePos_{};

    std::function<void()> onComplete_;
};
