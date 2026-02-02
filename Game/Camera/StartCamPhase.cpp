#include "StartCamPhase.h"
#include <cassert>

void StartCamPhase::Bind(Camera* camera, Transform* cameraTransform)
{
	// カメラ情報のバインド
	camera_ = camera;
	cameraTransform_ = cameraTransform;
}

void StartCamPhase::Start()
{
	// 中身があるかどうか
	assert(camera_ != nullptr);

	// 固定地点の保存
	introFixedY_ = camTargetPos_.y;
	introFixedZ_ = camTargetPos_.z;

	// 開始地点保存
	startPos_ = { camTargetPos_.x,introFixedY_,introFixedZ_ };

	// カメラを初期位置へ
	cameraTransform_->translate = startPos_;
	cameraTransform_->rotate = { 0.0f,0.0f,0.0f };
	// カメラに反映
	camera_->SetTranslate(cameraTransform_->translate);
	camera_->SetRotate(cameraTransform_->rotate);

	phase_ = Phase::MoveToLeft;
	timer_ = 0.0f;



}

void StartCamPhase::Skip()
{
	// 即座にターゲットへ
	introFixedY_ = camTargetPos_.y;
	introFixedZ_ = camTargetPos_.z;

	startPos_ = { camTargetPos_.x,introFixedY_,introFixedZ_ };
	cameraTransform_->translate = startPos_;
	camera_->SetTranslate(cameraTransform_->translate);

	phase_ = Phase::None;
	timer_ = 0.0f;
}


void StartCamPhase::Update(float dt)
{
	if (phase_ == Phase::None) {
		return;
	}

	const Vector3 leftPos = { introLeftX_,introFixedY_,introFixedZ_ };
	const Vector3 rightPos = { introRightX_,introFixedY_,introFixedZ_ };
	const Vector3 startPos = { camTargetPos_.x,introFixedY_,introFixedZ_ };

	switch (phase_)
	{
	case StartCamPhase::Phase::None:
		break;
	case StartCamPhase::Phase::MoveToLeft:
	{
		timer_ += dt;
		const float t = EaseOutCubic(timer_ / introMoveDur_);
		cameraTransform_->translate = Lerp(startPos_, Vector3{ introLeftX_,introFixedY_,introFixedZ_ }, t);
		if (timer_ >= introMoveDur_) {
			phase_ = Phase::PanToRight;
			timer_ = 0.0f;
			cameraTransform_->translate = leftPos;
		}
	}
	break;
	case StartCamPhase::Phase::PanToRight:
	{
		timer_ += dt;
		const float t = EaseOutCubic(timer_ / introPanDur_);
		cameraTransform_->translate = Lerp(leftPos, rightPos, t);
		if (timer_ >= introPanDur_) {
			phase_ = Phase::Hold;
			timer_ = 0.0f;
			cameraTransform_->translate = rightPos;
		}
	}
	break;
	case StartCamPhase::Phase::Hold: 
	{
		timer_ += dt;
		cameraTransform_->translate = rightPos;

		if (timer_ >= introHoldDur_) {
			phase_ = Phase::ReturnToStart;
			timer_ = 0.0f;
		}
	}
	break;
	case StartCamPhase::Phase::ReturnToStart: {

		timer_ += dt;

		float u = (introReturnDur_ > 0.0f) ? (timer_ / introReturnDur_) : 1.0f;
		u = std::clamp(u, 0.0f, 1.0f);

		float t = EaseOutBack(u);
		t = std::clamp(t, 0.0f, 1.0f);
		cameraTransform_->translate = Lerp(rightPos, startPos, t);

		if (timer_ >= introReturnDur_) {
			phase_ = Phase::None;
			timer_ = 0.0f;
			cameraTransform_->translate = startPos;
		}
	}
											break;
	default:

		break;
	}
	camera_->SetTranslate(cameraTransform_->translate);
}


bool StartCamPhase::IsRunning() const
{
	return phase_ != Phase::None;
}

void StartCamPhase::DrawImgui()
{
}
