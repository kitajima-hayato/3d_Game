#include "ModelParticleManager.h"
#include <random>

void ModelParticleManager::EmitBlockDebris(const Vector3& position, const Vector4& color, uint32_t count)
{
    std::random_device rnd;
    std::mt19937 randomEngine(rnd());
    std::uniform_real_distribution<float> distSpeed(3.0f, 8.0f);
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distRotation(-5.0f, 5.0f);

    for (uint32_t i = 0; i < count; ++i) {
        ModelParticle particle;

        float angle = distAngle(randomEngine);
        float speed = distSpeed(randomEngine);

        // 放射状に飛び散る
        particle.velocity = {
            std::cos(angle) * speed,
            distSpeed(randomEngine),  // 上向き
            std::sin(angle) * speed
        };

        // ⭐ 3D回転速度（ランダム）
        particle.angularVelocity = {
            distRotation(randomEngine),
            distRotation(randomEngine),
            distRotation(randomEngine)
        };

        particle.transform.scale = { 0.2f, 0.2f, 0.2f };  // 小さな立方体
        particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
        particle.transform.translate = position;

        particle.color = color;
        particle.lifeTime = 2.0f;
        particle.currentTime = 0.0f;
        particle.gravity = -9.8f;  // ⭐ 重力

        particles_.push_back(particle);
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

        // 位置更新
        it->transform.translate += it->velocity * kDeltaTime;

        // ⭐ 重力を適用
        it->velocity.y += it->gravity * kDeltaTime;

        // ⭐ 3D回転を適用
        it->transform.rotate.x += it->angularVelocity.x * kDeltaTime;
        it->transform.rotate.y += it->angularVelocity.y * kDeltaTime;
        it->transform.rotate.z += it->angularVelocity.z * kDeltaTime;

        it->currentTime += kDeltaTime;
        ++it;
    }
}

void ModelParticleManager::Draw()
{
     for (const auto& particle : particles_) {
        // モデルを描画
        debrisModel_->SetTransform(particle.transform);
        
        debrisModel_->Draw();
    }
}
