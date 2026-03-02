#include "SceneManager.h"
#include "Game/Scene/Transition/BallWipeTransition.h"

using namespace Engine;

static SceneManager* instance = nullptr;


SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new SceneManager();
	}
	return instance;
}

void SceneManager::DestroyInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

void SceneManager::Initialize()
{
	// 遷移マネージャーの初期化
	transitionManager_ = std::make_unique<TransitionManager>();
	transitionManager_->Initialize();

}

void SceneManager::Update(DirectXCommon* dxCommon)
{
	// 遷移マネージャーの更新
	if (transitionManager_) {
		transitionManager_->Update(kFixedDeltaTime_);

		// ロード開始タイミングでシーン生成
		if (transitionManager_->CanStartLoading() && isTransitionRequested_) {
			if (!nextScene_) {
				assert(sceneFactory_);
				nextScene_ = sceneFactory_->CreateScene(pendingSceneName_);

				// シーン初期化
				if (scene_) {
					scene_->Finalize();
					scene_.reset();
				}

				// シーン初期化
				if (nextScene_) {
					nextScene_->SetSceneManager(this);
					nextScene_->Initialize(dxCommon);
				}
				// ロードの完了通知
				transitionManager_->OnLoadingComplete();
			}
		}

		// ロード完了後、シーン切り替え実行
		if (transitionManager_->GetPhase() == TransitionPhase::FadeIn &&
			nextScene_ && isTransitionRequested_) {


			// シーンの切り替え
			scene_ = std::move(nextScene_);

			// リクエストをクリア
			isTransitionRequested_ = false;
			pendingSceneName_.clear();
		}
	}

	// 遷移中でない場合のみ通常シーン切り替え処理
	if (!transitionManager_->IsTransitioning() && !isTransitionRequested_) {
		// 次のシーンが予約されている場合（遷移無しの従来処理）
		if (nextScene_) {
			ExecuteSceneChange(dxCommon);
		}
	}

	// 実行中シーンを更新
	if (scene_) {
		scene_->Update();
	}

}

void SceneManager::ExecuteSceneChange(Engine::DirectXCommon* dxCommon)
{
	// 今のシーンの終了処理
	if (scene_) {
		scene_->Finalize();
		scene_.reset();
	}
	// 次のシーンの初期化
	scene_ = std::move(nextScene_);
	// シーンマネージャーの設定
	scene_->SetSceneManager(this);
	scene_->Initialize(dxCommon);
}


void SceneManager::Draw()
{
	// 実行中シーンが存在する場合のみ描画
	if (scene_) {
		scene_->Draw();
	}

	// 遷移演出の描画
	if (transitionManager_) {
		transitionManager_->Draw();
	}
}

void SceneManager::Finalize()
{
	// 遷移マネージャーの終了
	if (transitionManager_)
	{
		transitionManager_.reset();
	}

	// 最後のシーンの終了処理
	if (scene_)
	{
		scene_->Finalize();
		scene_.reset();
	}
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(!nextScene_);
	assert(sceneFactory_);

	// 次のシーン生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::ChangeSceneWithTransition(const std::string& sceneName, TransitionType transitionType)
{
	// 遷移中または既にリクエスト済みなら無視
	if (isTransitionRequested_ || transitionManager_->IsTransitioning()) {
		return;
	}

	// 次のシーン名を保存
	pendingSceneName_ = sceneName;
	isTransitionRequested_ = true;

	// 遷移タイプに応じた演出をセット
	if (transitionType == TransitionType::Start) {
		// Start版（少し豪華の方）
		auto startTransition = std::make_unique<BallWipeTransition>(true);
		startTransition->Initialize();
		transitionManager_->SetTransitionEffect(std::move(startTransition));
	} else {
		// Normal版（通常の方）
		auto normalTransition = std::make_unique<BallWipeTransition>(false);
		normalTransition->Initialize();
		transitionManager_->SetTransitionEffect(std::move(normalTransition));
	}
	// 遷移の開始
	transitionManager_->StartTransition(transitionType);
}





