#pragma once
#include "EffectManager.h"
#include "engine/math/Mymath.h"
class EffectEmitter
{
public:
	EffectEmitter();
	~EffectEmitter();

	// 更新
	void Update();
	// 描画
	void Emit();

	// Getter / Setter
	Transform GetTransform(const Transform& transform) { return transform; }
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

