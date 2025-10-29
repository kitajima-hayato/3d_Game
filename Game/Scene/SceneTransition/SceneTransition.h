#pragma once
#include <memory>
#include "engine/3d/Object3D.h"
#include "engine/2d/Sprite.h"
#include <random>


class SceneTransition
{
	public:
	enum class Type {
		kNone,
		kFadeIn,
		kFadeOut,
	};

	enum class Phase {
		In, Out,Done
	};

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="division"></param>
	void Initialize(uint32_t division,uint32_t totalFrame);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// Imgui
	/// </summary>
	void Imgui();
	
	/// <summary>
	/// ランダムな実数値を生成
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	float RandomRange(float min, float max);

	/// <summary>
	/// フェーズをDoneに設定
	/// </summary>
	bool IsFinished() const { return phase_ == Phase::Done; }

	
private:
	// スプライトデータ
	SpriteData spriteDate;
	// スプライト
	std::unique_ptr<Sprite> sprite;
	// スプライト配列
	std::vector<Sprite> sprites;
	// スプライト分割数
	uint32_t spriteCount;
	
	// 進行管理
	Phase phase_ = Phase::In;

	uint32_t frame_ = 0;

	uint32_t totalFrames_ = 120;

	uint32_t inFrames_ = 60;
	uint32_t outFrames_ = 60;

	float baseY_ = 720.0f;
	float travelY_ = 1080.0f;



	std::string textureFilePath = "resources/Black.png";

};

