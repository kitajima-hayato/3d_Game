#include "PauseSystem.h"
#include "Input.h"



void PauseSystem::Initialize()
{
	pauseUI_ = std::make_unique<PauseUI>();
	pauseUI_->Initialize();
}

bool PauseSystem::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		isPause_ = !isPause_;
	}
	if (isPause_) {
		// ポーズ画面
		pauseUI_->Update();
		// 続けるが選ばれて決定したらポーズ解除
		if (pauseUI_->PauseReleaseRequested()) {
			isPause_ = false;
		}
		// ポーズ中にもimguiを表示する
		DrawImgui();
		// ゲームの更新を行わない
		return true;
	}
	
	return isPause_;
}

void PauseSystem::Draw()
{
	if (isPause_) {
		// ポーズ画面
		pauseUI_->Draw();
	}
}

void PauseSystem::DrawImgui()
{
	if (isPause_) {
		pauseUI_->DrawImGui();
	}
}
