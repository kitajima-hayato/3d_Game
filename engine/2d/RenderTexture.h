#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "engine/math/MyMath.h"
#include "engine/base/Logger.h"
#include "engine/base/SrvManager.h"

namespace Engine {
	class DirectXCommon;
}
class SrvManager;
/// <summary>
/// レンダーテクスチャクラス
/// </summary>
class RenderTexture
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderTexture();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RenderTexture();

	/// <summary>
	/// 初期化
	/// RenderTarget リソースを生成し、RTV / SRV を作成する。また、描画に必要なパイプラインと頂点バッファの初期化も行う。
	/// </summary>
	/// <param name="dxCommon">DirectX12 のデバイス・コマンド管理を行う共通クラス</param>
	/// <param name="srvManager">RV のインデックス管理および SRV 作成を行うマネージャ</param>
	/// <param name="width">レンダーテクスチャの横幅（ピクセル）</param>
	/// <param name="height">レンダーテクスチャの縦幅（ピクセル）</param>
	/// <param name="format">SRV として使用する際のテクスチャフォーマット</param>
	/// <param name="clearColor">レンダーターゲットをクリアする際のクリアカラー</param>
	void Initialize(Engine::DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	/// <summary>
	/// レンダーテクスチャの開始
	/// </summary>
	void BeginRender();

	/// <summary>
	/// レンダーテクスチャの終了
	/// </summary>
	void EndRender();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	/// <summary>
	///RenderTarget テクスチャリソースを生成
	/// 指定されたサイズ・フォーマットで 2D テクスチャを作成し、RenderTarget として使用可能な状態で初期化する
	/// </summary>
	/// <param name="device">DirectX12 のデバイス</param>
	/// <param name="width">テクスチャの横幅（ピクセル）</param>
	/// <param name="height">テクスチャの縦幅（ピクセル）</param>
	/// <param name="format">フォーマット</param>
	/// <param name="clearColor">使用するクリアカラー</param>
	/// <returns>作成された RenderTarget 用 ID3D12Resource</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device>device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

private:
	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void CreateGraphicsPipeLine();
	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();
	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	void CreateVertexBuffer();

public: // Getter, Setter
	/// <summary>
	/// レンダーテクスチャのリソースを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
private: // メンバ変数
	/// DirectXCommonポインタ
	Engine::DirectXCommon* dxCommon_ = nullptr;
	/// SrvManagerポインタ
	SrvManager* srvManager_ = nullptr;

	/// レンダーテクスチャリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> texture;

	/// RTVヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	/// RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	/// SRVインデックス
	uint32_t srvIndex = 0;
	/// カレントステート
	D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	/// クリアカラー
	Vector4 clearColor_;

private:
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescs{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	/// ブレンドデスク
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 頂点シェーダーのコンパイル結果を格納するBlob
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	// ピクセルシェーダーのコンパイル結果を格納するBlob
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	// パイプラインステートの作成
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// VertexBufferResource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferResource;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	D3D12_RESOURCE_DESC resourceDesc{};

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

};

