#pragma once
#include "DirectXCommon.h"

class ModelCommon
{
public:
    // インスタンス取得
    static ModelCommon* GetInstance();

    // 初期化
    void Initialize(DirectXCommon* dxCommon);

    // 共通描画設定
    void DrawSettingCommon();

    // DirectXCommon の取得
    DirectXCommon* GetDxCommon() const { return dxCommon_; }

    ModelCommon() = default;
    ~ModelCommon() = default;

private:

    // コピー・代入禁止
    ModelCommon(const ModelCommon&) = delete;
    ModelCommon& operator=(const ModelCommon&) = delete;

    DirectXCommon* dxCommon_ = nullptr;
};
