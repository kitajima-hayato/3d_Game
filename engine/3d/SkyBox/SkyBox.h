#pragma once
#include "engine/math/MyMath.h"
#include "engine/base/DirectXCommon.h"
#include "engine/base/SrvManager.h"
#include "Game/Camera/camera.h"
/// <summary>
/// スカイボックスクラス
/// </summary>
class SkyBox
{
private:
	/// <summary>
	/// パイプラインの生成
	/// </summary>
	void CreatePipeline();

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの設定
	/// </summary>
	void SetGraphicsPipeline();

	/// <summary>
	/// バッファービューの作成
	/// </summary>
	void CreateVertexBufferView();
	/// <summary>
	/// スカイボックスの頂点生成
	/// </summary>
	void CreateVertexData();

	/// <summary>
	/// マテリアルの初期化
	/// </summary>
	void InitializeMaterial();

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="desc">設定対象のD3D12_BLEND_DESC</param>
	/// <param name="mode">使用するブレンドモード</param>
	void SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode);

	/// <summary>
	/// 行列データ生成
	/// </summary>
	void CreateTransformationMatrix();

	/// <summary>
	/// インデックスデータの生成
	/// </summary>
	void CreateIndexBufferView();
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(Engine::DirectXCommon* dxCommon, SrvManager* srvManager);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
private:
	///  モデルデータ
	ModelData modelData;


	// Initialize
	/// DxCommonのポインタ
	Engine::DirectXCommon* dxCommon = nullptr;
	/// SRVマネージャーのポインタ
	SrvManager* srvManager = nullptr;

	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	/// ルートパラメーター
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	/// サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	/// 入力レイアウトの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3];
	/// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	/// 入力レイアウト
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	/// ブレンド
	D3D12_BLEND_DESC blendDesc{};
	BlendMode blendMode = BlendMode::kBlendModeAdd;
	BlendMode currentBlendMode = BlendMode::kBlendModeAdd;  // 現在のブレンドモード

	/// ラスタライザー
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	/// シェーダーバイナリ
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;

	/// 深度ステンシル
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	/// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	// CreateVertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	/// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	/// 頂点データ
	VertexData* vertexData = nullptr;

	// InitializeMaterial
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	/// マテリアルデータ
	Material* materialData = nullptr;

	// CreateTransformationMatrix
	// バッファリソース / 座標変換行列リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* wvpData = nullptr;

	/// 頂点データのインデックス 
	std::vector<uint32_t> indices;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	/// カメラ引数
	Camera* camera = nullptr;
public:

	/// <summary>
	/// スカイボックスのテクスチャ
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> skyBoxTexture = nullptr;
	/// <summary>
	/// スカイボックスのテクスチャビュー
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE skyBoxTextureView = {};
};

