#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "MyMath.h"
#include <random>
#include "Game/Camera/Camera.h"

// パーティクルマネージャークラス
// シングルトンクラス
class ParticleManager
{
public:
	
	// パーティクル構造体
	struct ParticleGroup {		// パーティクルグループ // 使用するテクスチャごとにパーティクルグループとしてまとめる
		MaterialData materialData;			// マテリアルデータ					
		std::list<Particle> particles;		// パーティクルのリスト		
		uint32_t srvIndex;					// インスタンシングデータ用のSRVインデックス	
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;	// インスタンシングデータ用のリソース
		UINT kNumInstance;					// インスタンス数
		ParticleForGPU* instancingData;		// インスタンシングデータを書き込むためのポインタ
	};

	// インスタンスの取得
	static ParticleManager* GetInstance();
	static void DeleteInstance();
private:
	static ParticleManager* instance;
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;



	// ランダムエンジンの初期化 / 初期化処理内部
	void InitializeRandomEngine();
	// パイプラインの生成 / 初期化処理内部
	void CreatePipeline();
	// ルートシグネチャの作成 / パイプライン生成内部
	void CreateRootSignature();
	// グラフィックスパイプラインの設定 / パイプライン生成内部
	void SetGraphicsPipeline();
	// ブレンドモードの設定 / パイプライン生成内部
	void SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode);
	// 頂点データの初期化(座標) / 初期化処理内部
	void InitializeVertexData();
	// バッファービューの作成 / 初期化処理内部
	void CreateVertexBufferView();
	// マテリアルの初期化 / 初期化処理内部
	void InitializeMaterial();


public:
	// パーティクルの初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera);
	

	// パーティクルグループの作成
	void CreateParticleGroup(const std::string& name, const std::string textureFilrPath);

	// 更新処理
	void Update();
	// 行列更新 / 更新処理内部
	void UpdateMatrix();
	//  パーティクル更新 / 更新処理内部
	void UpdateParticle();
	// 当たり判定 / 更新処理内部 / パーティクル更新
	bool IsCollision(const AABB& aabb, const Vector3& point);

	// 描画処理
	void Draw();

	// パーティクルグループの削除
	void DeleteParticleGroup(const std::string& name);

	// パーティクルの発生
	void Emit(const std::string& name, const Vector3& position, uint32_t count);
	// エフェクトの発生
	void EffectEmit(const std::string& name, const Vector3& position, uint32_t count);

	
	// 通常パーティクル
	Particle MakeParticle(std::mt19937& randomEngine, const Vector3& position);

	// PrimitiveParticle
	Particle MakePrimitiveEffect(std::mt19937& randomEngine, const Vector3& translate);
	// リングエフェクト
	Particle MakeRingEffect(const Vector3& position);
	// Cylinderエフェクト
	Particle MakeCylinderEffect(const Vector3& position);

	// Ringの頂点生成
	void CreateRingVertex();
	// Ringエフェクトの描画処理
	void DrawRing();

	// Cylinder
	void CreateCylinderVertex();
	// CylindeEffectの描画
	void DrawCylinder();

	// 表示パーティクル / エフェクトの値調整 / 
	void DrawImgui();


private:
	DirectXCommon* dxCommon;
	SrvManager* srvManager;
	VertexData* vertexData = nullptr;
	Camera* camera = nullptr;

	// ランダムエンジン
	std::mt19937 randomEngine;

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
	// パイプラインステート

	// インプットレイアウト
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	// ブレンド
	BlendMode blendMode;
	BlendMode currentBlendMode;
	D3D12_BLEND_DESC blendDesc{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc;
	// シェーダーバイナリ
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

	// パーティクルの発生上限
	static const int kMaxParticle = 1000;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;

	// パーティクルデータ
	// パーティクルのモデルデータ
	ModelData modelData;
	// バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// マテリアルデータ
	Material* materialData = nullptr;
	// パーティクルグループ / グループ名をキーにしてパーティクルグループを管理
	std::unordered_map<std::string, ParticleGroup> particleGroups;

	// カメラ行列
	Matrix4x4 cameraMatrix;
	// ワールドビュープロジェクション行列
	Matrix4x4 viewMatrix;
	// プロジェクション行列
	Matrix4x4 projectionMatrix;
	// ビューポート行列
	Matrix4x4 backToFrontMatrix;
	// ビルボード行列
	Matrix4x4 billboardMatrix;
	bool useBillboard = false;

	// パーティクルのリスト
	std::list<Particle>particles;
	// 加速度フィールド
	AccelerationField accelerationField;
	// Δtを定義６０fos固定
	const float kDeltaTime = 1.0f / 60.0f;

	// リングエフェクト
	Microsoft::WRL::ComPtr<ID3D12Resource> ringVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW ringVertexBufferView{};
	uint32_t ringVertexCount = 0;

	// UV座標変更
	Vector2 uvOffset = { 0.0f, 0.0f };  
	Vector2 uvScrollSpeed = { 0.1f, 0.0f };  // 水平方向に流す場合


};

