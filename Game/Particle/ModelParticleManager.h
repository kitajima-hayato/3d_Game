#pragma once
#include "MyMath.h"
#include <memory>
#include <list>
#include "engine/3d/Object3D.h"


struct ModelParticle {
	Transform transform;		// 変換情報
	Vector3 velocity;			// 速度
	Vector4 color;				// 色
	Vector3 angularVelocity;	// 角速度
	float lifeTime;				// 寿命
	float currentTime;			// 経過時間
	float gravity;				// 重力影響度
};

/// <summary>
/// モデルパーティクルマネージャークラス
/// </summary>
class ModelParticleManager
{
public:
	/// <summary>
	/// ブロックの破片パーティクルを発生させる
	/// </summary>
	/// <param name="position">発生位置</param>
	/// <param name="color">破片の色</param>
	/// <param name="count">何個出すか</param>
	void EmitBlockDebris(const Vector3& position, const Vector4& color, uint32_t count);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private:
	// パーティクルリスト
	std::list<ModelParticle> particles_;
	// 破片用のモデル
	std::unique_ptr<Object3D>debrisModel_;
};

