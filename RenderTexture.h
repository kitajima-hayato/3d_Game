#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "MyMath.h"
#include <cassert>
class RenderTexture
{
public:
	// 初期化処理
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);
	
	// レンダーテクスチャ先
	void BeginRender();
	// レンダーテクスチャ後
	void EndRender();

	// GPUのハンドル取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return srvManager_->GetGPUDescriptorHandle(srvIndex); }
private:
	DirectXCommon* dxCommon_;
	SrvManager* srvManager_;

	Microsoft::WRL::ComPtr<ID3D12Resource>texture_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>rtvHeap_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
	uint32_t srvIndex = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	Vector4 clearColor = { 0.0f,0.0f,0.0f,1.0f };
	uint32_t width = 0;
	uint32_t height = 0;
	D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;



};

