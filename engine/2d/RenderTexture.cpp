#include "RenderTexture.h"
#include "engine/bace/DirectXCommon.h"
#include "engine/bace/SrvManager.h"
#include <map>
RenderTexture::RenderTexture()
{
}
RenderTexture::~RenderTexture()
{
}
void RenderTexture::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, 
	DXGI_FORMAT format, const Vector4& clearColor)
{
	/// 引き数を渡す
	this->dxCommon_ = dxCommon;
	this->srvManager_ = srvManager;
	this->clearColor_ = clearColor;


	const Vector4 kRenderTargetClearValue = clearColor;
	textureResource = CreateRenderTextureResource(
		dxCommon_->GetDevice(),width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue
	);

	rtvHeap = dxCommon_->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
	rtvHandle = dxCommon_->GetCPUDescriptorHandle(rtvHeap, dxCommon_->GetDescriptorSizeRTV(), 0);


	dxCommon_->GetDevice()->CreateRenderTargetView(textureResource.Get(), nullptr, rtvHandle);


	srvIndex = srvManager_->Allocate();
	srvManager_->CreateSRVforTexture2D(srvIndex, textureResource.Get(), format, 1);

	currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	CreateGraficsPipeLine();
	CreateVertexBuffer();
	
}

Microsoft::WRL::ComPtr<ID3D12Resource> RenderTexture::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{
	
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clear{};
	clear.Format = format;
	clear.Color[0] = clearColor.x;
	clear.Color[1] = clearColor.y;
	clear.Color[2] = clearColor.z;
	clear.Color[3] = clearColor.w;

	
	HRESULT hr = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clear,
		IID_PPV_ARGS(&textureResource));
	assert(SUCCEEDED(hr));

	return textureResource;
}

void RenderTexture::BeginRender()
{
	if (currentState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			textureResource.Get(),
			currentState,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
		currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	// DSV の設定
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon_->GetDSVHandle();
	dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// ビューポートとシザー設定
	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(resourceDesc.Width);
	viewport.Height = static_cast<float>(resourceDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = static_cast<LONG>(resourceDesc.Width);
	scissorRect.bottom = static_cast<LONG>(resourceDesc.Height);

	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);

	// クリアカラー
	float  clearColor[4] = { clearColor_.x,clearColor_.y,clearColor_.z,clearColor_.w };
	dxCommon_->GetCommandList()->ClearRenderTargetView(
		rtvHandle,
		clearColor,
		0,
		nullptr
	);
}


void RenderTexture::EndRender()
{
	if (currentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			textureResource.Get(),
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


void RenderTexture::Draw() {

	//// SRVヒープを設定（ルートテーブル用）
	//ID3D12DescriptorHeap* heaps[] = { srvManager_->GetSRVHeap().Get()};
	//dxCommon_->GetCommandList()->SetDescriptorHeaps(1, heaps);

	// PSO / RootSignature をセット
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());

	// SRVをピクセルシェーダーにバインド（ルートパラメータ0にSRVテーブル）
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, GetGPUHandle());

	// プリミティブトポロジーを設定（三角形リスト）
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 頂点バッファを使わず3頂点で全画面描画（VSでSV_VertexID使用前提）
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}




void RenderTexture::CreateGraficsPipeLine() {

	// ルートシグネチャの作成
	CreateRootSignatrue();

	// 頂点の位置データを表すセマンティクスを設定
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	// テクスチャ座標データを表すセマンティクスの座標
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	

	inputLayoutDescs.pInputElementDescs = nullptr;
	inputLayoutDescs.NumElements = 0;

	// レンダーターゲットの書き込みマスクを設定。全ての色チャンネルに書き込みを許可
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;


	// カリングモードを設定
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// ポリゴンのフィルモードを設定。ポリゴンの塗り潰し
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//　頂点シェーダーのコンパイル結果を格納するBlob
	vertexShaderBlob = dxCommon_->CompileShader(
		L"resources/shaders/Fullscreen.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	// ピクセルシェーダーのコンパイル結果を格納するBlob
	pixelShaderBlob = dxCommon_->CompileShader(
		L"resources/shaders/Grayscale.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;



	// グラフィックスパイプラインのステートオブジェクトのデスクリプタを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	// インプットレイアウトの設定
	graphicsPipelineStateDesc.InputLayout = { nullptr,0 };
	// シェーダーの設定
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	// ブレンドステートの設定
	graphicsPipelineStateDesc.BlendState = blendDesc;
	// ラスタライザの設定
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// レンダーターゲットの設定
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// プリミティブとポロ時の設定
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));

	assert(SUCCEEDED(hr));
}

void RenderTexture::CreateRootSignatrue()
{
#pragma region RootParameter
	//RootSignature作成
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_NONE;
#pragma region DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

	//RootParamater作成。複数設定できるので配列。今回は結果が１つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParam[1] = {};
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParam[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;

	descriptionRootSignature.pParameters = rootParam;
	descriptionRootSignature.NumParameters = 1;
#pragma endregion



	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].RegisterSpace = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ結果を格納するBlob
	Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob = nullptr;
	// エラーメッセージが発生した場合に格納するBlob
	Microsoft::WRL::ComPtr <ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

}

void RenderTexture::CreateVertexBuffer() {
	TextureVertex vertices[] = {
		{{-1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-1.0f,  3.0f, 0.0f, 1.0f}, {0.0f, -1.0f}},
		{{ 3.0f, -1.0f, 0.0f, 1.0f}, {2.0f, 1.0f}},
	};

	// 頂点バッファリソース作成（UPLOADヒープ）
	vertexBufferResource = dxCommon_->CreateBufferResource(sizeof(vertices));

	// Map してから安全にコピー
	void* mappedPtr = nullptr;
	HRESULT hr = vertexBufferResource->Map(0, nullptr, &mappedPtr);
	assert(SUCCEEDED(hr));
	memcpy(mappedPtr, vertices, sizeof(vertices));
	vertexBufferResource->Unmap(0, nullptr);

	// ビューの設定
	vertexBufferView.BufferLocation = vertexBufferResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertices);
	vertexBufferView.StrideInBytes = sizeof(TextureVertex);
}
