#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "engine/math/MyMath.h"
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
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	/// <summary>
	/// レンダーテクスチャの開始
	/// </summary>
	void BeginRender();

	/// <summary>
	/// レンダーテクスチャの終了
	/// </summary>
	void EndRender();

public: // Getter, Setter
	/// <summary>
	/// レンダーテクスチャのリソースを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return srvManager_->GetGPUDescriptorHandle(srvIndex); }
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
	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };
};

