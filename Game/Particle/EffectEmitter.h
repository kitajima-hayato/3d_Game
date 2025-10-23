#pragma once
#include "EffectManager.h"
#include "engine/math/Mymath.h"
/// <summary>
/// エフェクトエミッタクラス
/// </summary>
class EffectEmitter
{
public:
	// コンストラクタ・デストラクタ
	EffectEmitter();
	~EffectEmitter();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// エフェクト発生
	/// </summary>
	void EmitCylinder();
	void EmitRing();

	// Getter / Setter
	/// <summary>
	/// エミッタのTransform設定・取得
	/// </summary>
	Transform GetTransform() { return emitter.transform; }
	void SetTransform(const Transform& transform) { emitter.transform = transform; }

	void SetEffectName(const std::string& name) { effectName = name; }
private:
	struct Emitter {
		Transform transform;		// エフェクトのTRANSAM
		float lifeTime;				// 発生時間
	};
	// 発生させるエフェクトの情報
	Emitter emitter;

	// エフェクトネーム
	std::string effectName;

};

