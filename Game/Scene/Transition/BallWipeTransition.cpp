#include "BallWipeTransition.h"
#include <numbers>

// カラフルな色パレット（ポップで明るい色）
const std::vector<Vector4> BallWipeTransition::COLOR_PALETTE = {
	{ 1.0f, 0.3f, 0.3f, 1.0f },  // 赤
	{ 1.0f, 0.6f, 0.2f, 1.0f },  // オレンジ
	{ 1.0f, 0.9f, 0.2f, 1.0f },  // 黄色
	{ 0.4f, 0.9f, 0.4f, 1.0f },  // 緑
	{ 0.3f, 0.7f, 1.0f, 1.0f },  // 水色
	{ 0.6f, 0.4f, 1.0f, 1.0f },  // 紫
	{ 1.0f, 0.4f, 0.8f, 1.0f },  // ピンク
};

BallWipeTransition::BallWipeTransition(bool isStartTransition)
	: isStartMode_(isStartTransition)
	, totalDuration_(0.0f)
	, currentTime_(0.0f)
	, currentPhase_(Phase::BallAppear)
	, coverAlpha_(0.0f)
	, stampColorIndex_(0)
{
	// モードに応じてタイムラインを設定
	if (isStartMode_) {
		// 豪華版のタイムライン
		totalDuration_ = 1.0f;
		ballAppearTime_ = 0.0f;
		rollStartTime_ = 0.1f;
		rollEndTime_ = 0.5f;
		// 画面を完全に覆う
		coverCompleteTime_ = 0.55f;
		// 覆いを解除開始
		uncoverStartTime_ = 0.65f;
		stampInterval_ = 20.0f;
		stampGrowDuration_ = 0.9f;
		stampShrinkDuration_ = 0.3f;
	} else {
		// シンプル版のタイムライン
		totalDuration_ = 0.5f;
		ballAppearTime_ = 0.0f;
		rollStartTime_ = 0.05f;
		rollEndTime_ = 0.2f;
		coverCompleteTime_ = 0.25f;
		uncoverStartTime_ = 0.3f;
		stampInterval_ = 35.0f;
		stampGrowDuration_ = 0.6f;
		stampShrinkDuration_ = 0.15f;
	}
}



void BallWipeTransition::Initialize()
{
	currentTime_ = 0.0f;
	currentPhase_ = Phase::BallAppear;
	coverAlpha_ = 0.0f;
	stampColorIndex_ = 0;

	// ボールの初期化(２つ)
	balls_.clear();
	balls_.resize(2);

	// 上側のボール
	balls_[0].sprite = std::make_unique<Sprite>();
	// 使用するスプライトの指定
	balls_[0].sprite->Initialize(ballSpriteFilePath_);
	balls_[0].radius = BALL_RADIUS;
	balls_[0].isUpper = true;
	balls_[0].position = { -BALL_RADIUS * 2,UPPER_BALL_Y };
	balls_[0].rotation = 0.0f;
	balls_[0].lastStampX = -BALL_RADIUS * 2;
	balls_[0].sprite->SetSize({ BALL_RADIUS * 2, BALL_RADIUS * 2 });
	// 中心を基準に回転させる
	balls_[0].sprite->SetAnchorPoint({ 0.5f,0.5f });

	// 下側のボール
	balls_[1].sprite = std::make_unique<Sprite>();
	// 使用するスプライトの指定
	balls_[1].sprite->Initialize(ballSpriteFilePath_);
	balls_[1].radius = BALL_RADIUS;
	balls_[1].isUpper = false;
	balls_[1].position = { -BALL_RADIUS * 2,LOWER_BALL_Y };
	balls_[1].rotation = 0.0f;
	balls_[1].lastStampX = -BALL_RADIUS * 2;
	balls_[1].sprite->SetSize({ BALL_RADIUS * 2, BALL_RADIUS * 2 });
	// 中心を基準に回転させる
	balls_[1].sprite->SetAnchorPoint({ 0.5f,0.5f });

	// 帯（道）の初期化
	stamps_.clear();


	//// カバースプライト（画面全体を覆う）
	//coverSprite_ = std::make_unique<Sprite>();
	//coverSprite_->Initialize(trailSpriteFilePath_);
	//coverSprite_->SetPosition({ 0.0f,0.0f });
	//coverSprite_->SetSize({ SCREEN_WIDTH,SCREEN_HEIGHT });
	//coverSprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f });

}

void BallWipeTransition::Start()
{
	Initialize();
}

void BallWipeTransition::Update(float deltaTime)
{
	// 遷移時間の更新
	currentTime_ += deltaTime;

	if (currentPhase_ == Phase::Covered && currentTime_ >= uncoverStartTime_) {
		for (auto& stamp : stamps_) {
			stamp.sizeBeforeShrink = stamp.currentSize; // ★現在のサイズを保存
		}
		currentPhase_ = Phase::Uncovering;
	} else if (currentPhase_ == Phase::BallAppear && currentTime_ >= rollStartTime_) {
		currentPhase_ = Phase::Rolling;
	} else if (currentPhase_ == Phase::Rolling && currentTime_ >= rollEndTime_) {
		currentPhase_ = Phase::Covering;
	} else if (currentPhase_ == Phase::Covering && currentTime_ >= coverCompleteTime_) {
		currentPhase_ = Phase::Covered;
	} else if (currentPhase_ == Phase::Uncovering && currentTime_ >= totalDuration_) {
		currentPhase_ = Phase::Complete;
	}

	// 各フェーズの更新
	switch (currentPhase_)
	{
	case BallWipeTransition::Phase::BallAppear:
	case BallWipeTransition::Phase::Rolling:
		UpdateBalls(deltaTime);
		UpdateStamps(deltaTime);
		break;
	case BallWipeTransition::Phase::Covering:
	case BallWipeTransition::Phase::Covered:
	case BallWipeTransition::Phase::Uncovering:
		UpdateStamps(deltaTime);
		break;
	}
}

void BallWipeTransition::UpdateBalls(float deltaTime)
{
	// 転がりフェーズの進行度
	float rollDuration = rollEndTime_ - rollStartTime_;
	float rollTime = (std::max)(0.0f, currentTime_ - rollStartTime_);
	float rollProgress = (std::min)(1.0f, rollTime / rollDuration);

	// ボールを両面端まで移動
	float targetX = SCREEN_WIDTH + BALL_RADIUS * 2;

	for (auto& ball : balls_) {
		// 位置の更新
		float startX = -BALL_RADIUS * 2; // スタート位置（画面外左）
		float newX = startX + (targetX - startX) * rollProgress;
		float distance = newX - ball.position.x;
		ball.position.x = newX;

		// 回転の更新 / 移動距離で連動
		ball.rotation += distance / ball.radius;

		// スタンプの生成判定（一定間隔ごと）
		if (ball.position.x - ball.lastStampX >= stampInterval_) {
			CreateStamp(ball.position, ball.isUpper);
			ball.lastStampX = ball.position.x;
		}

		// スプライトに反映
		ball.sprite->SetPosition(ball.position);
		ball.sprite->SetRotation(ball.rotation);
		ball.sprite->Update();
	}
}

void BallWipeTransition::CreateStamp(const Vector2& position, bool isUpper)
{
	Stamp stamp;
	stamp.sprite = std::make_unique<Sprite>();
	stamp.sprite->Initialize(stampSpriteFilePath_);
	stamp.position = position;
	stamp.currentSize = 0.0f;
	stamp.sizeBeforeShrink = 0.0f;

	// サイズをランダムに変化
	float sizeVariation = (rand() % 20 - 10) / 100.0f;
	stamp.targetSize = 600.0f + 400.0f * (0.5f + sizeVariation);

	stamp.age = 0.0f;
	stamp.isUpper = isUpper;

	// カラフルな色を設定
	stamp.color = GetColorForStamp(stampColorIndex_);
	stampColorIndex_ = (stampColorIndex_ + 1) % COLOR_PALETTE.size();

	stamp.sprite->SetAnchorPoint({ 0.5f,0.5f });
	stamps_.push_back(std::move(stamp));
}

void BallWipeTransition::UpdateStamps(float deltaTime)
{
	for (auto& stamp : stamps_) {
		stamp.age += deltaTime;



		// Uncoveringフェーズならスタンプを縮小
		if (currentPhase_ == Phase::Uncovering) {
			float shrinkTime = currentTime_ - uncoverStartTime_;
			float shrinkProgress = shrinkTime / stampShrinkDuration_;
			shrinkProgress = (std::min)(1.0f, shrinkProgress);

			// 縮小アニメーション（イーズイン：加速しながら縮む）
			float shrinkEase = shrinkProgress * shrinkProgress;
			// 保存したサイズから0へ
			stamp.currentSize = stamp.sizeBeforeShrink * (1.0f - shrinkEase);

		} else {
			// 通常の成長アニメーション
			float growProgress = stamp.age / stampGrowDuration_;
			float eased;
			if (growProgress < 1.0f) {
				eased = 1.0f - (1.0f - growProgress) * (1.0f - growProgress);
			} else {
				float extraTime = growProgress - 1.0f;
				eased = 1.0f + extraTime * 0.8f;
				eased = (std::min)(3.5f, eased);
			}
			stamp.currentSize = stamp.targetSize * eased;
		}


		// スプライトに反映
		stamp.sprite->SetPosition(stamp.position);
		stamp.sprite->SetSize({ stamp.currentSize, stamp.currentSize });
		stamp.sprite->SetColor(stamp.color);
		stamp.sprite->Update();
	}
}

void BallWipeTransition::UpdateCover(float deltaTime)
{

}

Vector4 BallWipeTransition::GetColorForStamp(int index)
{
	return COLOR_PALETTE[index % COLOR_PALETTE.size()];
}

void BallWipeTransition::Draw()
{
	// スタンプの描画
	for (const auto& stamp : stamps_) {
		if (stamp.currentSize > 0.0f) {
			stamp.sprite->Draw();
		}
	}
	// ボール
	if (currentPhase_ == Phase::Rolling) {

		for (const auto& ball : balls_) {
			ball.sprite->Draw();
		}
	}


}

bool BallWipeTransition::IsHalfway()
{
	// 画面が完全に覆われたらロード開始
	return currentPhase_ >= Phase::Covering;
}

bool BallWipeTransition::IsComplete()
{
	return currentPhase_ == Phase::Complete;
}

float BallWipeTransition::GetProgress()
{
	return (std::min)(1.0f, currentTime_ / totalDuration_);
}


