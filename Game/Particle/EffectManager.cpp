#include "EffectManager.h"
#include <Logger.h>
#include "MakeMatrix.h"
#include <TextureManager.h>
#include "WinAPI.h"
#include <numbers>

EffectManager* EffectManager::instance = nullptr;

EffectManager* EffectManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new EffectManager();
	}
	return instance;
}

void EffectManager::DeleteInstance()
{
	delete instance;
	instance = nullptr;
}

void EffectManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera)
{
	// 各種取得
	this->dxCommon = dxCommon;
	this->srvManager = srvManager;
	this->camera = camera;
	// ブレンドモードの設定
	blendMode = BlendMode::kBlendModeAdd;

	// パイプラインの生成
	CreatePipeline();

	// 頂点データの初期化
	CreateRingVertex();
	CreateCylinderVertex();

	// マテリアルの初期化
	InitializeMaterial();
}

void EffectManager::Update()
{
	// 行列の更新
	UpdateMatrix();
	
	uvOffset.x += uvScrollSpeed.x * kDeltaTime;
	uvOffset.y += uvScrollSpeed.y * kDeltaTime;

	// wrap (0〜1の範囲に保つ)
	uvOffset.x = std::fmod(uvOffset.x, 5.0f);
	uvOffset.y = std::fmod(uvOffset.y, 5.0f);
	materialData->uvTransform = MakeTranslateMatrix({ uvOffset.x, uvOffset.y, 0.0f });

	// 更新した行列をGPUに送る
	UpdateEffectInstanceData();
}



void EffectManager::CreateEffectGroup(const std::string& name, const std::string textureFilrPath)
{
	// 登録済みの名前か確認
	if (effectGroups.contains(name)) {
		// 登録されていたらリターン
		Logger::Log("すでに登録されているエフェクトグループ名です");
		return;
	}
	// 新たな空のエフェクトグループを作成してコンテナに登録
	effectGroups[name] = EffectGroup();

	// マテリアルデータの作成
	EffectGroup& effectGroup = effectGroups[name];

	// 新たなエフェクトのマテリアルデータを作成
	effectGroup.materialData.textureFilePath = textureFilrPath;
	// テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(effectGroup.materialData.textureFilePath);
	// マテリアルデータにテクスチャのインデックスを設定
	effectGroup.materialData.textureIndex = TextureManager::GetInstance()->GetSrvIndex(effectGroup.materialData.textureFilePath);
	// インスタンシング用のリソースを作成
	effectGroup.instancingResource = dxCommon->CreateBufferResource(sizeof(ParticleForGPU) * kMaxEffectCount);
	effectGroup.srvIndex = srvManager->Allocate();
	// 書き込む為のアドレスを取得
	effectGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&effectGroup.instancingData));

	srvManager->CreateSRVforStructuredBuffer(effectGroup.srvIndex, effectGroup.instancingResource.Get(), kMaxEffectCount, sizeof(ParticleForGPU));

	// インスタンス数を初期化
	effectGroup.kNumInstance = 0;
}

void EffectManager::DeleteEffectGroup(const std::string& name)
{
	auto it = effectGroups.find(name);
	if (it != effectGroups.end()) {
		// Unmapでリークしないようにする
		if (it->second.instancingResource) {
			it->second.instancingResource->Unmap(0, nullptr);
			it->second.instancingData = nullptr; // ポインタの無効化
			it->second.instancingResource.Reset(); // ComPtrの解放
		}

		// SRVの解放も（もし必要であれば）
		// srvManager->Release(it->second.srvIndex); // 必要なら

		// Mapから削除
		effectGroups.erase(it);
	}
}

void EffectManager::EmitCylinder(const std::string& name, const Transform& transform, uint32_t count)
{
	// 登録済みのエフェクトグループ名か確認
	auto it = effectGroups.find(name);
	// エフェクトグループが存在することを確認
	assert(it != effectGroups.end());
	// 指定されたエフェクトグループにエフェクトを追加
	EffectGroup& group = it->second;
	for (uint32_t i = 0; i < count; ++i) {
		EffectInstance effect;
		effect.transform = transform;
		effect.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		effect.lifeTime = 1.0f;
		effect.currentTime = 0.0f;
		group.cylinderEffects.push_back(effect);
	}
}

void EffectManager::EmitRing(const std::string& name, const Transform& transform, uint32_t count)
{
	// 登録済みのエフェクトグループ名か確認
	auto it = effectGroups.find(name);
	// エフェクトグループが存在することを確認
	assert(it != effectGroups.end());
	// 指定されたエフェクトグループにエフェクトを追加
	EffectGroup& group = it->second;
	for (uint32_t i = 0; i < count; ++i) {
		EffectInstance effect;
		effect.transform = transform;
		effect.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		effect.lifeTime = 1.0f;
		effect.currentTime = 0.0f;
		group.ringEffects.push_back(effect);
	}
}




void EffectManager::CreateRingVertex()
{
	std::vector<VertexData> vertices;
	const uint32_t kRingDivide = 32;
	const float kOuterRadius = 1.0f;
	const float kInnerRadius = 0.2f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	for (uint32_t i = 0; i < kRingDivide; ++i) {
		float angle0 = i * radianPerDivide;
		float angle1 = (i + 1) * radianPerDivide;

		float sin0 = std::sin(angle0);
		float cos0 = std::cos(angle0);
		float sin1 = std::sin(angle1);
		float cos1 = std::cos(angle1);

		// 外側・内側の各点
		Vector4 outer0 = { sin0 * kOuterRadius, cos0 * kOuterRadius, 0.0f, 1.0f };
		Vector4 outer1 = { sin1 * kOuterRadius, cos1 * kOuterRadius, 0.0f, 1.0f };
		Vector4 inner0 = { sin0 * kInnerRadius, cos0 * kInnerRadius, 0.0f, 1.0f };
		Vector4 inner1 = { sin1 * kInnerRadius, cos1 * kInnerRadius, 0.0f, 1.0f };

		Vector2 uvOuter0 = { float(i) / kRingDivide, 0.0f };
		Vector2 uvOuter1 = { float(i + 1) / kRingDivide, 0.0f };
		Vector2 uvInner0 = { float(i) / kRingDivide, 1.0f };
		Vector2 uvInner1 = { float(i + 1) / kRingDivide, 1.0f };

		Vector3 normal = { 0.0f, 0.0f, 1.0f };

		// 三角形 1（outer0, outer1, inner0）
		vertices.push_back({ outer0, uvOuter0, normal });
		vertices.push_back({ outer1, uvOuter1, normal });
		vertices.push_back({ inner0, uvInner0, normal });

		// 三角形 2（inner0, outer1, inner1）
		vertices.push_back({ inner0, uvInner0, normal });
		vertices.push_back({ outer1, uvOuter1, normal });
		vertices.push_back({ inner1, uvInner1, normal });
	}

	// 頂点バッファの作成
	size_t vertexSize = sizeof(VertexData) * vertices.size();

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = vertexSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));

	VertexData* mapData = nullptr;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	memcpy(mapData, vertices.data(), vertexSize);
	vertexBuffer->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = static_cast<UINT>(vertexSize);
	vbView.StrideInBytes = sizeof(VertexData);

	ringVertexBuffer = vertexBuffer;
	ringVertexBufferView = vbView;
	ringVertexCount = static_cast<uint32_t>(vertices.size());
}

void EffectManager::CreateCylinderVertex()
{
	std::vector<VertexData> vertices;
	const uint32_t kCylinderDivide = 32;
	const float kTopRadius = 1.0f;
	const float kBottomRadius = 1.0f;
	const float kHeight = 3.0f;
	const float halfHeight = kHeight * 0.5f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);

	for (uint32_t i = 0; i < kCylinderDivide; ++i) {
		float angle0 = i * radianPerDivide;
		float angle1 = (i + 1) * radianPerDivide;

		float sin0 = std::sin(angle0);
		float cos0 = std::cos(angle0);
		float sin1 = std::sin(angle1);
		float cos1 = std::cos(angle1);

		float u0 = float(i) / float(kCylinderDivide);
		float u1 = float(i + 1) / float(kCylinderDivide);

		Vector4 top0 = { sin0 * kTopRadius, +halfHeight, cos0 * kTopRadius, 1.0f };
		Vector4 top1 = { sin1 * kTopRadius, +halfHeight, cos1 * kTopRadius, 1.0f };
		Vector4 bottom0 = { sin0 * kBottomRadius, -halfHeight, cos0 * kBottomRadius, 1.0f };
		Vector4 bottom1 = { sin1 * kBottomRadius, -halfHeight, cos1 * kBottomRadius, 1.0f };

		Vector2 uvTop0 = { u0, 0.0f };
		Vector2 uvTop1 = { u1, 0.0f };
		Vector2 uvBottom0 = { u0, 1.0f };
		Vector2 uvBottom1 = { u1, 1.0f };

		Vector3 normal0 = Vector3{ sin0, 0.0f, cos0 }.Normalized();
		Vector3 normal1 = Vector3{ sin1, 0.0f, cos1 }.Normalized();

		vertices.push_back({ top0,    uvTop0,    normal0 });
		vertices.push_back({ top1,    uvTop1,    normal1 });
		vertices.push_back({ bottom0, uvBottom0, normal0 });

		vertices.push_back({ top1,    uvTop1,    normal1 });
		vertices.push_back({ bottom1, uvBottom1, normal1 });
		vertices.push_back({ bottom0, uvBottom0, normal0 });
	}

	// 頂点バッファの作成
	size_t vertexSize = sizeof(VertexData) * vertices.size();

	Microsoft::WRL::ComPtr<ID3D12Resource>vertexBuffer;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = vertexSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));
	assert(SUCCEEDED(hr));

	VertexData* mapData = nullptr;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	memcpy(mapData, vertices.data(), vertexSize);
	vertexBuffer->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = static_cast<UINT>(vertexSize);
	vbView.StrideInBytes = sizeof(VertexData);

	cylinderVertexBuffer = vertexBuffer;
	cylinderVertexBufferView = vbView;
	cylinderVertexCount = static_cast<uint32_t>(vertices.size());
}

void EffectManager::DrawRing()
{
	// コマンド : ルートシグネチャを設定
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	// コマンド : パイプラインステートオブジェクトを設定
	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	// コマンド : プリミティブトポロジ(描画形状)を設定
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// コマンド : Ring用のVertexBufferViewを設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &ringVertexBufferView);
	// コマンド : マテリアルリソースを設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	for (auto& [name, effectGroup] : effectGroups) {
		// Skip: リングエフェクトが1つもない場合
		if (effectGroup.ringEffects.empty()) {
			continue;
		}

		// インスタンシングデータの更新
		srvManager->SetGraphicsDescriptorTable(1, effectGroup.srvIndex);

		// テクスチャSRV
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = srvManager->GetGPUDescriptorHandle(effectGroup.materialData.textureIndex);
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle);

		// 頂点データの描画（正しいインスタンス数を指定）
		dxCommon->GetCommandList()->DrawInstanced(ringVertexCount, 1, 0, 0);
	}
}


void EffectManager::DrawCylinder()
{
	// コマンド : ルートシグネチャを設定
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	// コマンド : パイプラインステートオブジェクトを設定
	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	// コマンド : プリミティブトロポジ(描画形状)を設定
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// コマンド : Cylinder用のVertexBufferViewを設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &cylinderVertexBufferView);
	// コマンド : マテリアルリソースを設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	for (auto& [name, effectGroup] : effectGroups) {
		if (effectGroup.cylinderEffects.empty()) {
			continue;
		}
		// インスタンシングデータの更新
		srvManager->SetGraphicsDescriptorTable(1, effectGroup.srvIndex);
		
		// テクスチャSRV index = 0(仮)
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = srvManager->GetGPUDescriptorHandle(effectGroup.materialData.textureIndex);
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle);
		// 頂点データの描画
		dxCommon->GetCommandList()->DrawInstanced(cylinderVertexCount, 1, 0, 0);
	}


}

#pragma region 内部処理 / 初期化
void EffectManager::CreatePipeline()
{
	// ルートシグネチャの作成
	CreateRootSignature();
	// グラフィックスパイプラインの設定
	SetGraphicsPipeline();
}

void EffectManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeInstancing[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// 1. RootSignatureの作成
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるので配列。
	// 0.Material
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// 1. パーティクルのRootSignatureの作成
	descriptorRangeInstancing[0].BaseShaderRegister = 0;
	descriptorRangeInstancing[0].NumDescriptors = 1;
	descriptorRangeInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// 1.TransformMatrix
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeInstancing);
	// 2.Texture
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	// Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));



	// 2. InputLayoutの設定
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// 3. BlendDtateの設定
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;

	SetBlendMode(blendDesc, blendMode);
	currentBlendMode = BlendMode::kBlendModeAdd;  // 現在のブレンドモード
	// α値のブレンド
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasterrizerStateの設定
	// カリングなし
	rasterrizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 塗りつぶしモード
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// 4. Shaderをコンパイルする
	vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Particle.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Particle.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void EffectManager::SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode)
{
	switch (mode)
	{
	case kBlendModeAdd:
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultiply:
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_DEST_COLOR;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		break;
	case kBlendModeScreen:
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
		break;
	default:
		assert(false);
		break;
	}
}

void EffectManager::SetGraphicsPipeline()
{
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
										vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
										pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	HRESULT hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

}

void EffectManager::InitializeMaterial()
{
	materialResource = dxCommon->CreateBufferResource(sizeof(Material));
	// マテリアルデータに書き込み
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// マテリアルデータの初期化
	materialData->color = Vector4(0.3f, 1.0f, 0.7f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
}

#pragma endregion

#pragma region 内部処理 / 更新
void EffectManager::UpdateMatrix()
{
	// カメラ行列の取得
	cameraMatrix = camera->GetWorldMatrix();
	// ビュー行列の取得
	viewMatrix = camera->GetViewMatrix();
	// ワールド日行列の取得
	backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
}

void EffectManager::UpdateEffectInstanceData()
{
	for (auto& [name, group] : effectGroups) {
		group.kNumInstance = 0;

		for (const auto& effect : group.cylinderEffects) {
			if (group.kNumInstance >= kMaxEffectCount) break;

			Matrix4x4 world = MakeAffineMatrix(effect.transform.scale, effect.transform.rotate, effect.transform.translate);
			Matrix4x4 wvp = Multiply(world, camera->GetViewProjectionMatrix());

			group.instancingData[group.kNumInstance].WVP = wvp;
			group.instancingData[group.kNumInstance].World = world;
			group.instancingData[group.kNumInstance].color = effect.color;

			++group.kNumInstance;
		}
		for (const auto& effect : group.ringEffects) {
			if (group.kNumInstance >= kMaxEffectCount) break;

			Matrix4x4 world = MakeAffineMatrix(effect.transform.scale, effect.transform.rotate, effect.transform.translate);
			Matrix4x4 wvp = Multiply(world, camera->GetViewProjectionMatrix());

			group.instancingData[group.kNumInstance].WVP = wvp;
			group.instancingData[group.kNumInstance].World = world;
			group.instancingData[group.kNumInstance].color = effect.color;

			++group.kNumInstance;
		}
	}
	
		
	
}



#pragma endregion