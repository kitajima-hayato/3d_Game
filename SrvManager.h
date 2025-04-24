#pragma once
#include "DirectXCommon.h"
#include "MyMath.h"
class SrvManager
{
public:
	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// アロケーター
	uint32_t Allocate();
	// SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV生成(バッファ用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStrude);
	// 描画前処理
	void PreDraw();
	// SRVセットコマンド
	void SetGraphicsDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
	// 最大数を超えているか
	bool IsAllocate();

	// RenderTexture
	Microsoft::WRL::ComPtr<ID3D12Resource>
		CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, Vector4& clearColor);
public:	// 
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
private:
	// 絶対にnewしない
	DirectXCommon* dxCommon;
	// SRVのディスクリプタサイズ
	uint32_t descriptorSize;
	// SRVのディスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap;
	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

};

