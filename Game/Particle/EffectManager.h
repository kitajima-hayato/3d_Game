#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "MyMath.h"
#include "Game/Camera/Camera.h"

class EffectManager
{
public:
    // シングルトン
    static EffectManager* GetInstance();
    static void DeleteInstance();

private:
    static EffectManager* instance;
    EffectManager() = default;
    ~EffectManager() = default;
    EffectManager(EffectManager&) = delete;
    EffectManager& operator=(EffectManager&) = delete;

public:
    // 初期化
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera);
    
    // 更新処理
    void Update();
    
    // 描画処理
    void Draw();

    // エフェクトの生成
    void CreateRingEffect(const Vector3& position);
    void CreateCylinderEffect(const Vector3& position);

private:
    DirectXCommon* dxCommon;
    SrvManager* srvManager;
    Camera* camera;

    // リングエフェクト用の頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> ringVertexBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW ringVertexBufferView{};
    uint32_t ringVertexCount = 0;

    // シリンダーエフェクト用の頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> cylinderVertexBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW cylinderVertexBufferView{};
    uint32_t cylinderVertexCount = 0;

    // エフェクトのリスト
    std::list<RingEffect> ringEffects;
    std::list<Particle> cylinderEffects;

    // 頂点生成
    void CreateRingVertex();
    void CreateCylinderVertex();

    // 描画処理
    void DrawRing();
    void DrawCylinder();

    // 行列
    Matrix4x4 cameraMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 backToFrontMatrix;
    Matrix4x4 billboardMatrix;

    // 定数
    const float kDeltaTime = 1.0f / 60.0f;
};
