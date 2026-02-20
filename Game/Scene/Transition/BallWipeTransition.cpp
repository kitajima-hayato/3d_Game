#include "BallWipeTransition.h"

BallWipeTransition::BallWipeTransition(bool isStartTransition)
	:isStartMode_(isStartTransition)
	, totalDuration_(0.0f)
	, currentTime_(0.0f)
	, wipeProgress_(0.0f)
{
	// モードに応じてタイムラインを設定
	if (isStartMode_) {
		// 豪華版のタイムライン
		totalDuration_ = 2.0f;
		ballAppearTime_ = 0.5f;
		rollStartTime_ = 0.5f;
		rollEndTime_ = 1.0f;
		wipeCompleteTime_ = 1.0f;
	} else {
		// シンプル版のタイムライン
		totalDuration_ = 0.3f;
		ballAppearTime_ = 0.05f;
		rollStartTime_ = 0.05f;
		rollEndTime_ = 0.22f;
		wipeCompleteTime_ = 0.3f;
	}
}

BallWipeTransition::~BallWipeTransition()
{
}

void BallWipeTransition::Initialize()
{
	currentTime_ = 0.0f;
	wipeProgress_ = 0.0f;

	// ボールの初期化(２つ)
	balls_.clear();
	balls_.resize(2);

	// 上側のボール
	balls_[0].sprite = std::make_unique<Sprite>();
	// 使用するスプライトの指定
	balls_[0].sprite->Initialize(ballSpriteFilePath_);
	balls_[0].radius = 32.0f;// @todo 適切な大きさに
	balls_[0].isUpper = true;
	balls_[0].position = { -balls_[0].radius,SCREEN_HEIGHT * 0.25f };
	balls_[0].rotation = 0.0f;

	// 下側のボール
	balls_[1].sprite = std::make_unique<Sprite>();
	// 使用するスプライトの指定
	balls_[1].sprite->Initialize(ballSpriteFilePath_);
	balls_[1].radius = 32.0f;// @todo 適切な大きさに
	balls_[1].isUpper = false;
	balls_[1].position = { -balls_[1].radius,SCREEN_HEIGHT * 0.75f };
	balls_[1].rotation = 0.0f;

	// 帯（道）の初期化
	trails_.clear();
	trails_.resize(2);

	float trailHeight = isStartMode_ ? 80.0f : 40.0f; // 豪華版は帯を少し太くする

	// 上側の帯
	trails_[0].sprite = std::make_unique<Sprite>();
	trails_[0].sprite->Initialize(trailSpriteFilePath_);
	trails_[0].isUpper = true;
	trails_[0].position = { 0.0f,balls_[0].position.y - trailHeight / 2.0f };
	trails_[0].size = { 0.0f,trailHeight };

	// 下側の帯
	trails_[1].sprite = std::make_unique<Sprite>();
	trails_[1].sprite->Initialize(trailSpriteFilePath_);
	trails_[1].isUpper = false;
	trails_[1].position = { 0.0f,balls_[1].position.y - trailHeight / 2.0f };
	trails_[1].size = { 0.0f,trailHeight };

}

void BallWipeTransition::Start()
{
	Initialize();
}

void BallWipeTransition::Update(float deltaTime)
{
	// 遷移時間の更新
	currentTime_ += deltaTime;

	// タイムラインに応じた更新
	if (currentTime_ >= rollStartTime_ &&
		currentTime_ <= rollEndTime_) {
		UpdateBalls(deltaTime);
		UpdateTrail(deltaTime);
	}
	if (currentTime_ >= rollEndTime_) {
		UpdateWipe(deltaTime);
	}
}

void BallWipeTransition::UpdateBalls(float deltaTime)
{
	// 転がりフェーズの進行度
	float rollDuration = rollEndTime_ - rollStartTime_;
	float rollTime = currentTime_ - rollStartTime_;
	float rollProgress = rollTime / rollDuration;

	if (rollProgress > 1.0f)rollProgress = 1.0f;

	// ボールを両面端まで移動
	float targetX = SCREEN_WIDTH + 50.0f;

	for (auto& ball : balls_) {
		// 位置の更新
		float newX = -ball.radius + (targetX + ball.radius) * rollProgress;
		float distance = newX - ball.position.x;
		ball.position.x = newX;

		// 回転の更新 / 移動距離で連動
		ball.rotation += distance / ball.radius;

		// スプライトに反映
		ball.sprite->SetPosition(ball.position);
		ball.sprite->SetRotation(ball.rotation);
		ball.sprite->Update();
	}


}

void BallWipeTransition::UpdateTrail(float deltaTime)
{
	// ボールの位置に応じて帯を延ばす
	for (size_t i = 0; i < trails_.size(); ++i) {
		auto& trail = trails_[i];
		auto& ball = balls_[i];

		// 帯の幅をボールの位置まで伸ばす
		trail.size.x = ball.position.x + ball.radius;

		// スプライトに反映
		trail.sprite->SetPosition(trail.position);
		trail.sprite->SetSize(trail.size);
		trail.sprite->Update();
	}
}

void BallWipeTransition::UpdateWipe(float deltaTime)
{
	// ワイプの進行度を計算
	float wipeDuration = wipeCompleteTime_ - rollEndTime_;
	float wipeTime = currentTime_ - rollEndTime_;
	wipeProgress_ = wipeTime / wipeDuration;

	if (wipeProgress_ > 1.0f) wipeProgress_ = 1.0f;
}

void BallWipeTransition::Draw()
{
	// ボールと帯を描画
	for (const auto& trail : trails_) {
		if (trail.size.x > 0.0f) {
			trail.sprite->Draw();
		}
	}
	for (const auto& ball : balls_) {
		if (currentTime_ >= ballAppearTime_) {
			ball.sprite->Draw();
		}
	}
}

bool BallWipeTransition::IsHalfway()
{
	// 帯が画面中央まで到達したらロード開始
	return wipeProgress_ >= 0.5f;
}

bool BallWipeTransition::IsComplete()
{
	return currentTime_ >= totalDuration_;
}

float BallWipeTransition::GetProgress()
{
	return currentTime_ / totalDuration_;
}


