#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "MyMath.h"
#include "Game/Camera/Camera.h"
/// <summary>
/// エフェクト管理クラス
/// </summary>
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

    /// <summary>
	/// パイプラインの作成 / 初期化処理内部
    /// </summary>
    void CreatePipeline();
    /// <summary>
	/// ルートシグネチャの作成 / パイプライン生成内部
    /// </summary>
    void CreateRootSignature();
    /// <summary>
	/// グラフィックスパイプラインの設定 / パイプライン生成内部
    /// </summary>
    void SetGraphicsPipeline();
    /// <summary>
	/// ブレンドモードの設定 / パイプライン生成内部
    /// </summary>
    /// <param name="desc"></param>
    /// <param name="mode"></param>
    void SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode);

    /// <summary>
	/// 頂点バッファビューの作成 / 初期化処理内部
    /// </summary>
    void CreateVertexBufferView();
    /// <summary>
	/// マテリアルの初期化 / 初期化処理内部
    /// </summary>
    void InitializeMaterial();

    /// <summary>
	/// 行列の更新 / 更新処理内部
    /// </summary>
    void UpdateMatrix();

    /// <summary>
	/// エフェクトインスタンスデータの更新 / 更新処理内部
    /// </summary>
    void UpdateEffectInstanceData();


public:
    /// <summary>
	/// 初期化
    /// </summary>
    /// <param name="dxCommon"></param>
    /// <param name="srvManager"></param>
    /// <param name="camera"></param>
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera);
    
    /// <summary>
	/// 更新
    /// </summary>
    void Update();
    
    /// <summary>
	/// 描画
    /// </summary>
    void DrawRing();
    void DrawCylinder();


    /// <summary>
	/// エフェクトグループの作成
    /// </summary>
    /// <param name="name"></param>
    /// <param name="textureFilrPath"></param>
    void CreateEffectGroup(const std::string& name, const std::string textureFilrPath);
    /// <summary>
	/// エフェクトグループの削除
    /// </summary>
    /// <param name="name"></param>
    void DeleteEffectGroup(const std::string& name);
    /// <summary>
	/// シリンダーエフェクトの発生
    /// </summary>
    /// <param name="name"></param>
    /// <param name="transform"></param>
    /// <param name="count"></param>
    void EmitCylinder(const std::string& name, const Transform& transform, uint32_t count);
    void EmitRing(const std::string& name, const Transform& transform, uint32_t count);


    // エフェクトの生成
    //void CreateRing(const Transform& transform);
    //void CreateCylinder(const Transform& transform);
    //void CreatePrimitive(const Transform& transform);

  
private:
    // エフェクト用
    struct EffectGroup {
        MaterialData materialData;
        uint32_t srvIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource;
        UINT kNumInstance;
        ParticleForGPU* instancingData;

		std::list <EffectInstance> ringEffects;
		std::list <EffectInstance> cylinderEffects;
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


