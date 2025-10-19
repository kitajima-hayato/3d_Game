#pragma once
#include "DirectXCommon.h"
/// <summary>
/// SRV管理クラス
/// </summary>
/// <remarks>
/// SRVの生成、管理を行う
/// シングルトンクラス
/// </remarks>
class SrvManager
{
public:
	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// アロケーター
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();
	/// <summary>
	/// SRV生成(テクスチャ2D用)
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="Format"></param>
	/// <param name="MipLevels"></param>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	/// <summary>
	/// SRV生成(バッファ用)
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="numElements"></param>
	/// <param name="structureByteStrude"></param>
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStrude);
	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();
	/// <summary>
	/// SRVセットコマンド
	/// </summary>
	/// <param name="RootParameterIndex"></param>
	/// <param name="srvIndex"></param>
	void SetGraphicsDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
	
	/// <summary>
	/// 最大数に達しているか
	/// </summary>
	/// <returns></returns>
	bool IsAllocate();
public:// Getter,Setter
	/// <summary>
	/// CPUディスクリプタハンドルの取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	/// <summary>
	/// GPUディスクリプタハンドルの取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	/// <summary>
	/// SRVヒープの取得
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRVHeap()const;
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

