#include "ModelParticleManager.h"
#include <random>
#include "Object3D.h"

// シングルトンインスタンスの初期化

ModelParticleManager& ModelParticleManager::GetInstance()
{
	static ModelParticleManager instance;
	return instance;
}


void ModelParticleManager::Initialize()
{
	// 破片用のモデルを読み込む 
	modelName_ = "breakBlock.obj";

}

void ModelParticleManager::EmitBlockDebris(const Vector3& position, const Vector4& color, uint32_t count)
{
	std::random_device rnd;
	std::mt19937 randomEngine(rnd());
	std::uniform_real_distribution<float> distSpeed(1.0f, 3.0f);
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> distRotation(-5.0f, 5.0f);

	for (uint32_t i = 0; i < count; ++i) {
		ModelParticle particle;

		particle.modelObject = std::make_unique<Object3D>();
		particle.modelObject->Initialize();
		particle.modelObject->SetModel(modelName_);


		float angle = distAngle(randomEngine);
		float speed = distSpeed(randomEngine);

		// 放射状に飛び散る
		particle.velocity = {
			std::cos(angle) * speed,
			distSpeed(randomEngine),  // 上向き
			std::sin(angle) * speed
		};

		// 3D回転速度（ランダム）
		particle.angularVelocity = {
			distRotation(randomEngine),
			distRotation(randomEngine),
			distRotation(randomEngine)
		};

		Transform transform;
		transform.scale = { 0.2f, 0.2f, 0.2f };
		transform.rotate = { 0.0f, 0.0f, 0.0f };
		transform.translate = position;
		particle.modelObject->SetTransform(transform);

		particle.lifeTime = 3.0f;
		particle.currentTime = 0.0f;
		particle.gravity = -15.0f;

		particles_.push_back(std::move(particle));
	}
}

void ModelParticleManager::Update()
{
	const float kDeltaTime = 1.0f / 60.0f;

	for (auto it = particles_.begin(); it != particles_.end();) {
		if (it->currentTime >= it->lifeTime) {
			it = particles_.erase(it);
			continue;
		}

		Transform transform = it->modelObject->GetTransform();

		// 位置更新
		transform.translate += it->velocity * kDeltaTime;

		// 重力を適用
		it->velocity.y += it->gravity * kDeltaTime;

		// 3D回転を適用
		transform.rotate.x += it->angularVelocity.x * kDeltaTime;
		transform.rotate.y += it->angularVelocity.y * kDeltaTime;
		transform.rotate.z += it->angularVelocity.z * kDeltaTime;

		it->modelObject->SetTransform(transform);

		it->modelObject->Update();

		it->currentTime += kDeltaTime;
		++it;
	}
}

void ModelParticleManager::Draw()
{
	// 全てのパーティクルを描画
	for (const auto& particle : particles_) {
		if (particle.modelObject) {
			particle.modelObject->Draw();
		}
	}
}
