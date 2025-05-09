#include "Object3D.h"
#include "Object3DCommon.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "MakeMatrix.h"
#include "ModelManager.h"
#include "TextureManager.h"
using namespace std;
void Object3D::Initialize(Object3DCommon* obj3dCommon)
{
	this->object3DCommon = obj3dCommon;
	this->camera = obj3dCommon->GetDefaultCamera();

	CreateTransformationMatrixData();
	CreateDirectionalLightResource();

	// Transformの初期化
	transform = { { 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f  } };

	// テクスチャのロード（初回のみでOK）
	TextureManager::GetInstance()->LoadTexture("resources/gradationLine.png");

	// GPUハンドルの取得
	textureHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("resources/gradationLine.png");
}

void Object3D::Update()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjection;
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjection = Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjection = worldMatrix;
	}
	wvpData->WVP = worldViewProjection;
	wvpData->World = worldMatrix;
}



void Object3D::Draw() {
	object3DCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	object3DCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	object3DCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle_);

	if (vertexBuffer && indexBuffer && indexCount > 0) {
		// 動的メッシュ（リング）の描画
		object3DCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
		object3DCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&ibView);
		object3DCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(indexCount, 1, 0,0, 0);
	} else if (model) {
		// 通常のモデル描画
		model->Draw();
	}
}




void Object3D::CreateRingMesh(uint32_t kRingDivide, float kOuterRadius, float innerRadius) {
	std::vector<RingVertex> vertices;
	std::vector<uint16_t> indices;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	for (uint32_t i = 0; i < kRingDivide; ++i) {
		float sin = std::sin(i * radianPerDivide);
		float cos = std::cos(i * radianPerDivide);
		float sinNext = std::sin((i + 1) * radianPerDivide);
		float cosNext = std::cos((i + 1) * radianPerDivide);
		float u = float(i) / float(kRingDivide);
		float uNext = float(i + 1) / float(kRingDivide);
		// 外周1
		vertices.push_back({ { cos * kOuterRadius, -sin * kOuterRadius, 0.0f, 1.0f }, { u,0.0f }, { 0.0f,0.0f,1.0f } });
		// 外周2
		vertices.push_back({ { cosNext * kOuterRadius, -sinNext * kOuterRadius, 0.0f, 1.0f }, { uNext,0.0f }, { 0.0f,0.0f,1.0f } });
		// 内周1
		vertices.push_back({ { cos * innerRadius, -sin * innerRadius, 0.0f, 1.0f }, { u, 1.0f }, { 0.0f, 0.0f, 1.0f } });
		// 内周2
		vertices.push_back({ { cosNext * innerRadius, -sinNext * innerRadius, 0.0f, 1.0f }, { uNext, 1.0f }, { 0.0f, 0.0f, 1.0f } });

		uint16_t base = i * 4;
		// 1つ目の三角形
		indices.push_back(base + 0);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
		// 2つ目の三角形
		indices.push_back(base + 2);
		indices.push_back(base + 1);
		indices.push_back(base + 3);
	}
	indexCount = static_cast<uint32_t>(indices.size());

	// 頂点バッファ作成
	size_t vbSize = sizeof(RingVertex) * vertices.size();
	vertexBuffer = object3DCommon->GetDxCommon()->CreateBufferResource(vbSize);
	void* vbMap = null;
	vertexBuffer->Map(0, nullptr, &vbMap);
	memcpy(vbMap, vertices.data(), vbSize);
	vertexBuffer->Unmap(0, nullptr);

	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = static_cast<UINT>(vbSize);
	vbView.StrideInBytes = sizeof(RingVertex);

	// インデックスバッファ作成
	size_t ibSize = sizeof(uint16_t) * indices.size();
	indexBuffer = object3DCommon->GetDxCommon()->CreateBufferResource(ibSize);
	void* ibMap = nullptr;
	indexBuffer->Map(0, nullptr, &ibMap);
	memcpy(ibMap, indices.data(), ibSize);
	indexBuffer->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView.SizeInBytes = static_cast<UINT>(ibSize);
	ibView.Format = DXGI_FORMAT_R16_UINT;
}

void Object3D::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);
}





void Object3D::CreateTransformationMatrixData()
{
	// 変換行列リソースを作成
	wvpResource = object3DCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// 変換行列リソ
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 変換行列データの初期化
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();

}

void Object3D::CreateDirectionalLightResource()
{
	// 平行光源リソースを作成
	directionalLightResource = object3DCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	// 平行光源リソースにデータを書き込むためのアドレスを取得してdirectionalLightDataに割り当てる
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	// 平行光源データの初期化 / デフォルト値
	// ライトの色
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// ライトの向き
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	// 輝度
	directionalLightData->intensity = 1.0f;
}

