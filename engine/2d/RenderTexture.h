#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "engine/math/MyMath.h"
#include "engine/bace/Logger.h"
#include "engine/bace/DirectXCommon.h"
#include "engine/bace/SrvManager.h"

class DirectXCommon;
class SrvManager;

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
	/// </summary>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

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

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device>device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);
private:
	void CreateGraficsPipeLine();
	void CreateRootSignatrue();
	void CreateVertexBuffer();

public: // Getter, Setter
	/// <summary>
	/// レンダーテクスチャのリソースを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
private: // メンバ変数
	/// DirectXCommonポインタ
	DirectXCommon* dxCommon_ = nullptr;
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

