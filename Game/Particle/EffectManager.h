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

    // パイプラインの生成 / 初期化処理内部
    void CreatePipeline();
    // ルートシグネチャの作成 / パイプライン生成内部
    void CreateRootSignature();
    // グラフィックスパイプラインの設定 / パイプライン生成内部
    void SetGraphicsPipeline();
    // ブレンドモードの設定 / パイプライン生成内部
    void SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode);

    // バッファービューの作成 / 初期化処理内部
    void CreateVertexBufferView();
    // マテリアルの初期化 / 初期化処理内部
    void InitializeMaterial();

    // 行列更新 / 更新内部処理
    void UpdateMatrix();

    // 更新した行列をGPUに送る
    void UpdateEffectInstanceData();


public:
    // 初期化
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera);
    
    // 更新処理
    void Update();
    
    // 描画処理
    void DrawRing();
    void DrawCylinder();


    // エフェクトグループの作成
    void CreateEffectGroup(const std::string& name, const std::string textureFilrPath);
    // エフェクトグループの削除
    void DeleteEffectGroup(const std::string& name);
    // エフェクトの発生
    void EmitCylinder(const std::string& name, const Transform& transform, uint32_t count);
    void EmitRing(const std::string& name, const Transform& transform, uint32_t count);


    // エフェクトの生成
    void CreateRing(const Transform& transform);
    void CreateCylinder(const Transform& transform);
    void CreatePrimitive(const Transform& transform);

    template <typename Container>
    inline void UpdateEffectList(Container& effects, float deltaTime);
private:
    // エフェクト用
    struct EffectGroup {
        MaterialData materialData;
        std::list<EffectInstance> effects;
        uint32_t srvIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource;
        UINT kNumInstance;
        ParticleForGPU* instancingData;
    };

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
    std::list<EffectInstance> ringEffects;
    std::list<EffectInstance> cylinderEffects;

    // 頂点生成
    void CreateRingVertex();
    void CreateCylinderVertex();

   

    // 行列
    Matrix4x4 cameraMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 backToFrontMatrix;
    Matrix4x4 billboardMatrix;

    // 定数
    const float kDeltaTime = 1.0f / 60.0f;

    // UV座標変更
    Vector2 uvOffset = { 0.0f, 0.0f };
    Vector2 uvScrollSpeed = { 0.1f, 0.0f };  // 水平方向に流す場合

private:    // 内部で処理で使用している変数
    /// CreateRootSignature
    // ルートシグネチャ
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    // ルートパラメーター
    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    // サンプラー
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3];
    // インプットレイアウト
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

    // blendmode
    BlendMode blendMode;
    BlendMode currentBlendMode;
    D3D12_BLEND_DESC blendDesc{};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    D3D12_RASTERIZER_DESC rasterrizerDesc{};

    // シェーダーバイナリ
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

    /// SetGraphicsPipeLine
    // パイプライン
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

    /// InitializeMaterial
    // マテリアルリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
    // マテリアルデータ
    Material* materialData = nullptr;

    /// CreateEffectGroup
    // エフェクトグループ
    std::unordered_map<std::string, EffectGroup> effectGroups;
    // エフェクトの発生上限
    static const int kMaxEffectCount = 1000;

};


template<typename Container>
inline void EffectManager::UpdateEffectList(Container& effects, float deltaTime)
{
    for (auto it = effects.begin(); it != effects.end(); )
    {
        it->currentTime += deltaTime;
        if (it->currentTime >= it->lifeTime) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
}
