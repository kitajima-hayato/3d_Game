#include "RenderTexture.h"

void RenderTexture::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{
	// 引数渡し
	this->dxCommon_ = dxCommon;
	this->srvManager_ = srvManager;

	this->width = width;
	this->height = height;
	this->format = format;
	this->clearColor = clearColor;

	// 
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = this->width;
	desc.Height = this->height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	//
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	//
	D3D12_CLEAR_VALUE clear{};
	clear.Format = this->format;
	clear.Color[0] = this->clearColor.x;
	clear.Color[1] = this->clearColor.y;
	clear.Color[2] = this->clearColor.z;
	clear.Color[3] = this->clearColor.w;

	//
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clear,
		IID_PPV_ARGS(&texture_));
	assert(SUCCEEDED(hr));

	//
	rtvHeap_ = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
	rtvHandle = dxCommon_->GetCPUDescriptorHandle(rtvHeap_.Get(), dxCommon_->GetDescriptorSizeRTV(), 0);

	dxCommon_->GetDevice()->CreateRenderTargetView(texture_.Get(), nullptr, rtvHandle);

	srvIndex = srvManager_->Allocate();
	srvManager_->CreateSRVforTexture2D(srvIndex, texture_.Get(), format, 1);

	currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
}

void RenderTexture::BeginRender()
{
	//
	if (currentState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texture_.Get(),
			currentState,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
		currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	//
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon_->GetCPUDescriptorHandle(rtvHeap_,dxCommon_->GetDescriptorSizeRTV(),);
	dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle_, FALSE, &dsvHandle);

	// ビューポートとシザー設定
	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = static_cast<LONG>(width);
	scissorRect.bottom = static_cast<LONG>(height);

}

void RenderTexture::EndRender()
{
}
