#pragma once
#include "ParticleManager.h"
using namespace std;
/// <summary>
/// パーティクルエミッタクラス
/// </summary>
class ParticleEmitter
{
public:
    /// <summary>
	/// コンストラクタ・デストラクタ
    /// </summary>
    ParticleEmitter();
    ~ParticleEmitter();
    /// <summary>
	/// 更新
    /// </summary>
    void Update();
    /// <summary>
	/// パーティクル発生
    /// </summary>
    void Emit();

	/// <summary>
	/// 破壊可能ブロックのパーティクル発生
	/// </summary>
	void BreakEmit();

	// Getter / Setter
	/// <summary>
	/// エミッタのTransform設定・取得
	/// </summary>
	/// <param name="transform"></param>
    void SetTransform(const Transform& transform) { emitter.transform = transform; }
	Transform GetTransform()const { return emitter.transform; }

	/// <summary>
	/// Particleの発生位置設定・取得
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Vector3& translate) { emitter.transform.translate = translate; }
	Vector3 GetTranslate()const { return emitter.transform.translate; }

	/// <summary>
	/// Particleの回転設定・取得
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const Vector3& rotate) { emitter.transform.rotate = rotate; }
	Vector3 GetRotate()const { return emitter.transform.rotate; }

	/// <summary>
	/// Particleのスケール設定・取得
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { emitter.transform.scale = scale; }
	Vector3 GetScale()const { return emitter.transform.scale; }

	/// <summary>
	/// 発生させるパーティクルの数設定・取得
	/// </summary>
	/// <param name="name"></param>
	void SetParticleName(const string& name) { particleName = name; }

private:
    // パーティクル構造体
    struct Emitter {
        Transform transform;	// エミッタのTransform
        uint32_t count;			// 生成するパーティクルの数
        float frequency;		// 生成する頻度
        float frequencyTime;	// 生成する時間
    };
    
    // カレントタイム
    float currentTime;
    // フレームごとの時間間隔（例: 60FPSの場合）
	const float kDeltaTime = 1.0f / 60.0f;
    // パーティクル発生間隔
    const float emitInterval = 1.0f;
    
	// 描画上限数 / インスタンス数
    const uint32_t kNumMaxInstance = 100;
    // パーティクルのリスト
	std::list<Particle> particles;
	// Emitter
	Emitter emitter;

    // パーティクルネーム
    string particleName;

private:
    Vector3 test;
};
