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
	void EmitCylinder();
	void EmitRing();

	// Getter / Setter
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

