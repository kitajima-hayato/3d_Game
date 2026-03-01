#include "SkyBox.h"
#include <Logger.h>
#include "MakeMatrix.h"
#include "TextureManager.h"
#include "Object3DCommon.h"

using namespace Engine;

void SkyBox::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	this->dxCommon = dxCommon;
	this->srvManager = srvManager;
	this->camera = Object3DCommon::GetInstance()->GetDefaultCamera();
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	modelData.material.textureFilePath = textureFilePath;
	/// パイプラインの生成
	CreatePipeline();
	/// ルートシグネチャの生成
	CreateRootSignature();
	/// グラフィックスパイプラインの設定
	SetGraphicsPipeline();
	/// スカイボックスの頂点生成
	CreateVertexData();
	/// マテリアルの初期化
	InitializeMaterial();
	/// 
	CreateTransformationMatrix();
}

void SkyBox::Update()
{
	// カメラのビュー行列とプロジェクション行列を取得
	const Matrix4x4& viewMatrix = camera->GetViewMatrix();
	const Matrix4x4& projectionMatrix = camera->GetProjectionMatrix();

	// 回転はそのまま、位置だけを無視したビュー行列を作成
	Matrix4x4 viewNoTranslation = viewMatrix;
	viewNoTranslation.m[3][0] = 0.0f;
	viewNoTranslation.m[3][1] = 0.0f;
	viewNoTranslation.m[3][2] = 0.0f;

	// スカイボックス用のワールド行列（スケールのみ、位置も回転もなし）
	Matrix4x4 worldMatrix = MakeScaleMatrix({ 1.0f, 1.0f, 1.0f });

	// WVP行列を構築（視点固定）
	Matrix4x4 viewProj = Multiply(viewNoTranslation, projectionMatrix);
	Matrix4x4 worldViewProjection = Multiply(worldMatrix, viewProj);

	wvpData->WVP = worldViewProjection;
	wvpData->World = worldMatrix;
}



void SkyBox::Draw()
{
	/// コマンド : ルートシグネチャを設定
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	/// コマンド : パイプラインステートオブジェクトを設定
	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	/// コマンド : プリミティブトロポジ(描画形状)を設定
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/// コマンド : VertexBufferViewを設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	/// コマンド : インデックスバッファビューを設定
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	/// コマンド : マテリアルの定数バッファを設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	///
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle =
		TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath);
	/// コマンド : スカイボックスのテクスチャビューを設定
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2,textureHandle);
	/// コマンド : 描画
	//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.indices.size()), 1, 0, 0);
	dxCommon->GetCommandList()->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);
	
}

void SkyBox::CreatePipeline()
{
	// パイプラインの生成
	/// ルートシグネチャの生成
	CreateRootSignature();
	// グラフィックスパイプラインの設定
	SetGraphicsPipeline();
}

void SkyBox::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//D3D12_DESCRIPTOR_RANGE descriptorRangeInstancing[1] = {};
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

	// 1. TransformMatrix
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 1;

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 塗りつぶしモード
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// 4. Shaderをコンパイルする
	vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/SkyBox.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/SkyBox.PS.hlsl",
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

void SkyBox::SetGraphicsPipeline()
{
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
										vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
										pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ.三角形
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

void SkyBox::CreateVertexBufferView() {

	vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファービュー
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 1頂点分のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点数
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// データの書き込み / マップ
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// データの書き込み
	memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

}

void SkyBox::InitializeMaterial()
{
	// リソース生成
	materialResource = dxCommon->CreateBufferResource(sizeof(Material));
	// マテリアルデータに書き込み
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// マテリアルデータの初期化
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
}

void SkyBox::CreateVertexData()
{
	// インデックス（各面：2三角形＝6つのインデックス）
	indices = {
		// 各面ごとに2枚の三角形 (6インデックス)
		0, 1, 2,  2, 1, 3,   // 右面
		4, 5, 6,  6, 5, 7,   // 左面
		8, 9, 10, 10, 9,11,  // 前面
		12,13,14, 14,13,15,  // 後面
		16,17,18, 18,17,19,  // 上面
		20,21,22, 22,21,23   // 下面
	};

	modelData.indices = indices;


	// 右面 (+X) / 描画index [0,1,2] [2,1,3]
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, -1.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {1, 0, 0} });
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, 1.0f, 1.0f},  .texcoord = {1.0f, 1.0f}, .normal = {1, 0, 0} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, -1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {1, 0, 0} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 1.0f, 1.0f},  .texcoord = {0.0f, 1.0f}, .normal = {1, 0, 0} });

	// 左面 (-X) / 描画index [4,5,6] [6,5,7]
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 1.0f, 1.0f},  .texcoord = {1.0f, 0.0f}, .normal = {-1, 0, 0} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, -1.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {-1, 0, 0} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, 1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {-1, 0, 0} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, -1.0f, 1.0f},.texcoord = {0.0f, 1.0f}, .normal = {-1, 0, 0} });

	// 前面 (+Z) / 描画index [8,9,10] [10,9,11]
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 1.0f, 1.0f},  .texcoord = {1.0f, 0.0f}, .normal = {0, 0, 1} });
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, 1.0f, 1.0f},   .texcoord = {1.0f, 1.0f}, .normal = {0, 0, 1} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, 1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0, 0, 1} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 1.0f, 1.0f},  .texcoord = {0.0f, 1.0f}, .normal = {0, 0, 1} });

	// 後面 (-Z) / 描画index [12,13,14] [14,13,15]
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, -1.0f, 1.0f},  .texcoord = {1.0f, 0.0f}, .normal = {0, 0, -1} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, -1.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0, 0, -1} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, -1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0, 0, -1} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, -1.0f, 1.0f},.texcoord = {0.0f, 1.0f}, .normal = {0, 0, -1} });

	// 上面 (+Y) / 描画index [16,17,18] [18,17,19]
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, -1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0, 1, 0} });
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, -1.0f, 1.0f},  .texcoord = {0.0f, 1.0f}, .normal = {0, 1, 0} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 1.0f, 1.0f},  .texcoord = {1.0f, 0.0f}, .normal = {0, 1, 0} });
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, 1.0f, 1.0f},   .texcoord = {1.0f, 1.0f}, .normal = {0, 1, 0} });

	// 下面 (-Y) / 描画index [20,21,22] [22,21,23]
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, 1.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0, -1, 0} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 1.0f, 1.0f},  .texcoord = {0.0f, 1.0f}, .normal = {0, -1, 0} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, -1.0f, 1.0f},.texcoord = {1.0f, 0.0f}, .normal = {0, -1, 0} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, -1.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0, -1, 0} });

	CreateVertexBufferView();
	CreateIndexBufferView();
}


void SkyBox::SetBlendMode(D3D12_BLEND_DESC& desc, BlendMode mode)
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

void SkyBox::CreateTransformationMatrix()
{
	// 変換行列リソースを作成
	wvpResource =Object3DCommon::GetInstance()->GetDxCommon()->
		CreateBufferResource(sizeof(TransformationMatrix));
	// 変換行列リソース
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 変換行列データの初期化
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
}

void SkyBox::CreateIndexBufferView() {

	// インデックスリソース作成
	indexResource = dxCommon->CreateBufferResource(sizeof(uint32_t) * modelData.indices.size());

	// IndexBufferView 設定
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * modelData.indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// マップしてデータ書き込み
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());
}
