#pragma once
#include "ITransitionEffect.h"
#include "Sprite.h"
#include <memory>
#include <vector>
/// <summary>
/// ボール転がりワイプ遷移演出
/// </summary>
class BallWipeTransition :public ITransitionEffect
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BallWipeTransition(bool isStartTransition = false);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BallWipeTransition();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	// ITransitionEffect インターフェース
	void Start() override;
	void Update(float deltaTime) override;
	void Draw() override;
	bool IsHalfway()override;
	bool IsComplete()override;
	float GetProgress()override;

private:
	/// <summary>
	/// ボールの更新
	/// </summary>
	void UpdateBalls(float deltaTime);

	/// <summary>
	/// 帯（道の更新）
	/// </summary>
	void UpdateTrail(float deltaTime);

	/// <summary>
	/// ワイプマスクの更新
	/// </summary>
	void UpdateWipe(float deltaTime);

private:
	// モード設定
	// スタートモード豪華版か
	bool isStartMode_;
	// 演出の総時間
	float totalDuration_;
	// 現在の経過時間
	float currentTime_;

	// タイムライン
	// ボールの出現時間
	float ballAppearTime_;
	// 転がり開始時間
	float rollStartTime_;
	// 転がり終了時間
	float rollEndTime_;
	// ワイプ完了時間
	float wipeCompleteTime_;

	// ボール関連
	struct Ball {

		std::unique_ptr<Sprite>sprite;
		Vector2 position;
		// 回転角度
		float rotation;
		// ボールの半径
		float radius;
		// 上側のボールか
		bool isUpper;
	};
	std::vector<Ball> balls_;

	// 帯（道）関連
	struct Trail
	{
		std::unique_ptr<Sprite>sprite;
		Vector2 position;
		Vector2 size;
		// 上側の帯か
		bool isUpper;
	};
	std::vector<Trail>trails_;

	// ワイプマスク
	std::unique_ptr<Sprite> wipeMask_;
	// ワイプの進行度
	float wipeProgress_;

	// スプライトのファイルパス
	const std::string ballSpriteFilePath_ = "resources/SceneTransition/ball.png";
	const std::string trailSpriteFilePath_ = "resources/SceneTransition/trail.png";


	// 画面のサイズ
	static constexpr float SCREEN_WIDTH = 1280.0f;
	static constexpr float SCREEN_HEIGHT = 720.0f;
};

