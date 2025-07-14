#include "RenderTexture.h"
#include "engine/bace/DirectXCommon.h"
#include "engine/bace/SrvManager.h"
void RenderTexture::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	/// 引き数　を渡す
	this->dxCommon_ = dxCommon;
	this->srvManager_ = srvManager;


	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = 1280; // テクスチャの幅
	desc.Height = 720; // テクスチャの高さ
	desc.DepthOrArraySize = 1; // 1枚のテクスチャ
	desc.MipLevels = 1; // ミップマップレベル
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // テクスチャのフォーマット
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clear{};
	clear.Format = desc.Format;
	clear.Color[0] = clearColor_.x;
	clear.Color[1] = clearColor_.y;
	clear.Color[2] = clearColor_.z;
	clear.Color[3] = clearColor_.w;

	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clear,
		IID_PPV_ARGS(&texture));
	assert(SUCCEEDED(hr));

	rtvHeap = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
	rtvHandle = dxCommon_->GetCPUDescriptorHandle(rtvHeap.Get(), dxCommon_->GetDescriptorSizeRTV(), 0);

	dxCommon_->GetDevice()->CreateRenderTargetView(texture.Get(), nullptr, rtvHandle);

	srvIndex = srvManager_->Allocate();
	srvManager_->CreateSRVforTexture2D(srvIndex, texture.Get(), desc.Format, 1);

	currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;



}

void RenderTexture::BeginRender()
{
	if (currentState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texture.Get(),
			currentState,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
		currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon_->GetDSVHandle();
	dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	/// ビューポート / シザー設定
	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = 1280.0f;
	viewport.Height = 720.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 1280;
	scissorRect.bottom = 720;

	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);

	float clearColor[] = { clearColor_.x,clearColor_.y,clearColor_.z,clearColor_.w };
	dxCommon_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

}

void RenderTexture::EndRender()
{
	if (currentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texture.Get(),
			currentState,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
		currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetGPUHandle() const {
	return srvManager_->GetGPUDescriptorHandle(srvIndex);
}
