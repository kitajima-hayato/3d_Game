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
	this->dxCommon = dxCommon;
	this->srvManager = srvManager;
	this->camera = camera;

	// 頂点データの初期化
	CreateRingVertex();
	CreateCylinderVertex();
}

void EffectManager::Update()
{
	// カメラ行列の更新
	cameraMatrix = camera->GetWorldMatrix();
	viewMatrix = camera->GetViewMatrix();
	backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// リングエフェクトの更新
	for (auto it = ringEffects.begin(); it != ringEffects.end();)
	{
		it->currentTime += kDeltaTime;
		if (it->currentTime >= it->lifeTime)
		{
			it = ringEffects.erase(it);
		}
		else
		{
			++it;
		}
	}

	// シリンダーエフェクトの更新
	for (auto it = cylinderEffects.begin(); it != cylinderEffects.end();)
	{
		it->currentTime += kDeltaTime;
		if (it->currentTime >= it->lifeTime)
		{
			it = cylinderEffects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void EffectManager::Draw()
{
	DrawRing();
	DrawCylinder();
}

void EffectManager::CreateRingEffect(const Vector3& position)
{
	RingEffect effect;
	effect.transform.scale = { 1.0f, 1.0f, 1.0f };
	effect.transform.rotate = { 0.0f, 0.0f, 0.0f };
	effect.transform.translate = position;
	effect.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	effect.lifeTime = 1.0f;
	effect.currentTime = 0.0f;
	effect.isActive = true;

	ringEffects.push_back(effect);
}

void EffectManager::CreateCylinderEffect(const Vector3& position)
{
	Particle effect;
	effect.transform.scale = { 1.0f, 1.0f, 1.0f };
	effect.transform.rotate = { 0.0f, 0.0f, 0.0f };
	effect.transform.translate = position;
	effect.velocity = { 0.0f, 0.0f, 0.0f };
	effect.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	effect.lifeTime = 1.0f;
	effect.currentTime = 0.0f;

	cylinderEffects.push_back(effect);
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

		Vector4 outer0 = { sin0 * kOuterRadius, cos0 * kOuterRadius, 0.0f, 1.0f };
		Vector4 outer1 = { sin1 * kOuterRadius, cos1 * kOuterRadius, 0.0f, 1.0f };
		Vector4 inner0 = { sin0 * kInnerRadius, cos0 * kInnerRadius, 0.0f, 1.0f };
		Vector4 inner1 = { sin1 * kInnerRadius, cos1 * kInnerRadius, 0.0f, 1.0f };

		Vector2 uvOuter0 = { float(i) / kRingDivide, 0.0f };
		Vector2 uvOuter1 = { float(i + 1) / kRingDivide, 0.0f };
		Vector2 uvInner0 = { float(i) / kRingDivide, 1.0f };
		Vector2 uvInner1 = { float(i + 1) / kRingDivide, 1.0f };

		Vector3 normal = { 0.0f, 0.0f, 1.0f };

		vertices.push_back({ outer0, uvOuter0, normal });
		vertices.push_back({ outer1, uvOuter1, normal });
		vertices.push_back({ inner0, uvInner0, normal });

		vertices.push_back({ inner0, uvInner0, normal });
		vertices.push_back({ outer1, uvOuter1, normal });
		vertices.push_back({ inner1, uvInner1, normal });
	}

	size_t vertexSize = sizeof(VertexData) * vertices.size();
	ringVertexBuffer = dxCommon->CreateBufferResource(vertexSize);
	ringVertexBufferView.BufferLocation = ringVertexBuffer->GetGPUVirtualAddress();
	ringVertexBufferView.SizeInBytes = UINT(vertexSize);
	ringVertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	ringVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	memcpy(vertexData, vertices.data(), vertexSize);
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

	size_t vertexSize = sizeof(VertexData) * vertices.size();
	cylinderVertexBuffer = dxCommon->CreateBufferResource(vertexSize);
	cylinderVertexBufferView.BufferLocation = cylinderVertexBuffer->GetGPUVirtualAddress();
	cylinderVertexBufferView.SizeInBytes = UINT(vertexSize);
	cylinderVertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	cylinderVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	memcpy(vertexData, vertices.data(), vertexSize);
	cylinderVertexCount = static_cast<uint32_t>(vertices.size());
}

void EffectManager::DrawRing()
{
	// リングエフェクトの描画処理を実装
	// TODO: パイプラインステートの設定や描画コマンドの発行
}

void EffectManager::DrawCylinder()
{
	// シリンダーエフェクトの描画処理を実装
	// TODO: パイプラインステートの設定や描画コマンドの発行
}
