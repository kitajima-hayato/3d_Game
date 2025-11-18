#include "GamePlayScene.h"
#include "Game/Application/Map.h"
#include "engine/bace/ImGuiManager.h"
#include "Game/Application/Player.h"
#include "Game/Collision/CollisionManager.h"
#include <Input.h>
GamePlayScene::GamePlayScene()
{
}

GamePlayScene::~GamePlayScene()
{
}

void GamePlayScene::Initialize(DirectXCommon* dxCommon)
{
	/// スプライトの初期化
	SpriteCommon::GetInstance()->Initialize(dxCommon);
	/// オーディオの初期化
	Audio::GetInstance()->Initialize();
	soundData = Audio::GetInstance()->LoadWave("resources/mokugyo.wav");
	xaudio2_ = Audio::GetInstance()->GetXAudio2();
	Audio::GetInstance()->SoundPlayWave(xaudio2_, soundData);

	// カメラクラスの生成
	camera = std::make_unique<Camera>();
	// カメラの初期設定
	camTargetPos_ = { 7.5f,4.0f,-20.0f };
	// どれだけ引くか
	const float pullBack = 30.0f;
	camStartPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z + pullBack };

	// 強めに引く
	camOvershootPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z - overShootAmt_ };
	
	// 目標位置
	cameraTransform.translate = camStartPos_;
	// カメラに反映
	camera->SetTranslate(cameraTransform.translate);
	Object3DCommon::GetInstance()->SetDefaultCamera(camera.get());

	// スタート演出の開始
	startPhase_ = StartCamPhase::DollyIn;
	startTimer_ = 0.0f;


	// マップ
	map = std::make_unique<Map>();
	map->Initialize();

	
	collision_ = std::make_unique<CollisionManager>();

	/// プレイヤーの初期化
	player = std::make_unique<Player>();
	player->Initialize(Vector3{2.0f,1.0f,0.0f});
	player->SetMap(map.get());
	//player->SetTransitionTiming(TransitionTiming::OnDeath);

	InitializeEnemy();

	//sceneTransition = std::make_unique<SceneTransition>();
	//sceneTransition->Initialize(10);

	titleLogoObject = std::make_unique<Object3D>();
	titleLogoObject->Initialize();
	titleLogoObject->SetModel("title.obj");
	titleLogoTransform = {
		{ 1.0f,1.0f,1.0f },
		{ 1.6f,3.2f,0.0f },
		{ 7.4f,-3.1f,50.0f }
	};

	titleLogoObject->SetTransform(titleLogoTransform);

	backGround = std::make_unique<BackGround>();
	backGround->Initialize();

	clearSprite = std::make_unique<Sprite>();
	clearSprite->Initialize("resources/clear.png");
	clearSprite->SetPosition({ 400.0f,135.0f });
	clearSprite->SetSize({ 500.0f,300.0f });
	clearSprite->SetColor({ 1.0f,1.0f,1.0f,0.8f });

	clearSprite2 = std::make_unique<Sprite>();
	clearSprite2->Initialize("resources/oneMore.png");
	clearSprite2->SetPosition({ 490.0f,385.0f });
	clearSprite2->SetSize({ 300.0f,150.0f });
	clearSprite2->SetColor({ 1.0f,1.0f,1.0f,0.8f });

	clearSprite3 = std::make_unique<Sprite>();
	clearSprite3->Initialize("resources/R.png");
	clearSprite3->SetPosition({ 800.0f,430.0f });
	clearSprite3->SetSize({ 70.0f,70.0f });
	clearSprite3->SetColor({ 1.0f,1.0f,1.0f,0.8f });

	clearBack = std::make_unique<Object3D>();
	clearBack->Initialize();
	clearBack->SetModel("whiteBack.obj");
	clearBack->SetScale({0.1f,2.3f,4.0f});
	clearBack->SetTranslate({ 7.3f,4.0f,-5.2f });
	clearBack->SetRotate({ 0.0f,1.55f,0.0f });

	ParticleManager::GetInstance()->CreateParticleGroup("Particle", "resources/monsterball.png");

	ParticleManager::GetInstance()->CreateParticleGroup("neo", "resources/uvChecker.png");
	// パーティクルエミッターの初期化
	clearParticle_ = make_unique<ParticleEmitter>();
	clearParticle_->SetTransform({ {0.0f,0.0f,0.0f}, { 0.0f,0.0f,0.0f }, { -5.0f,0.0f,20.0f } });
	clearParticle_->SetParticleName("Particle");

}


void GamePlayScene::Update()
{
	if(isClear_==false) {
		// クリア条件
		if (player->GetPosition().x >= 6.4f) {
			isClear_ = true;
		}
	} 

	// ゴールしているときRが押されたらフラグを戻してプレイヤーを初期位置へ
	if (isClear_) {
		if (Input::GetInstance()->PushKey(DIK_R)) {
			isClear_ = false;
			player->SetPosition({ 2.0f,1.0f,0.0f });
		}
	}
	
	clearParticle_->Update();

	titleLogoObject->Update();

	backGround->Update();

	const float dt = 1.0f / 60.0f;

	/// カメラの更新
	UpdateStartCamera(dt);

	camera->SetTranslate(cameraTransform.translate);
	camera->Update();

	//sceneTransition->Update();
	/// マップの更新
	map->Update();
	  
	//player->BeginFrameHitReset();

	/// マップとプレイヤーの判定のためマップチップデータをプレイヤーにも渡す
	//player->SetMapChipField(map.get());

	/// プレイヤーの更新
	player->Update();

	// スプライトの更新
	clearSprite->Update();
	clearSprite2->Update();
	clearSprite3->Update();
	clearBack->Update();

	


	for (auto& enemy : enemies) {
		enemy->Update();
	}
	/// 当たりは判定
	CheckCollision();

	/// imgui
	DrawImgui();
}

void GamePlayScene::Draw()
{
	
	backGround->Draw();

	//sceneTransition->Draw();
	/// マップの描画
	map->Draw();
	/// プレイヤーの描画
	player->Draw();
	/// タイトルロゴの描画
	//titleLogoObject->Draw();
	/// 敵の描画
	for (auto& enemy : enemies) {
		//enemy->Draw();
	}
	/// クリアスプライトの描画
	if (isClear_) {
		clearSprite->Draw();
		clearSprite2->Draw();
		clearSprite3->Draw();
		clearBack->Draw();
	}
	ParticleManager::GetInstance()->Draw();
	clearParticle_->Emit();
}

void GamePlayScene::InitializeEnemy()
{

	normalEnemy = EnemyFactory::CreateEnemy("NormalEnemy");
	normalEnemy->Initialize();
	normalEnemy->SetTranslate({ 4.0f,-3.0f,20.0f });
	enemies.push_back(std::move(normalEnemy));

	flyingEnemy = EnemyFactory::CreateEnemy("FlyingEnemy");
	flyingEnemy->Initialize();
	flyingEnemy->SetTranslate({ 1.0f,-5.0f,20.0f });
	enemies.push_back(std::move(flyingEnemy));




}

void GamePlayScene::CheckCollision()
{
	
	collision_->Clear();

	/// プレイヤー
	if (player) {
		//collision_->AddCollider(player.get());
	}

	/// エネミー全種
	for (auto& enemy : enemies) {
		if (!enemy)continue;
		
		if(enemy->IsAlive()) {
			collision_->AddCollider(enemy.get());
		}
	}

	collision_->CheckAllCollisions();

}

void GamePlayScene::UpdateStartCamera(float dt)
{
	switch (startPhase_) {
	case StartCamPhase::DollyIn: {
		startTimer_ += dt;
		float t = EaseOutCubic(startTimer_ / durDollyIn_); // 早→遅（スムーズな減速）
		cameraTransform.translate = Lerp(camStartPos_, camOvershootPos_, t);
		if (startTimer_ >= durDollyIn_) {
			startPhase_ = StartCamPhase::Settle;
			startTimer_ = 0.0f;
		}
	} break;

	case StartCamPhase::Settle: {
		startTimer_ += dt;
		float t = EaseOutBack(startTimer_ / durSettle_);   // “ボン”と戻る
		cameraTransform.translate = Lerp(camOvershootPos_, camTargetPos_, t);
		if (startTimer_ >= durSettle_) {
			startPhase_ = StartCamPhase::Shake;
			startTimer_ = 0.0f;
			cameraTransform.translate = camTargetPos_; // 目標にピタッ
		}
	} break;

	case StartCamPhase::Shake: {
		startTimer_ += dt;
		float u = std::clamp(1.0f - (startTimer_ / shakeTime_), 0.0f, 1.0f); // 減衰
		// シンプルな1軸シェイク（必要ならxy両方にしてもOK）
		float s = std::sin(startTimer_ * 60.0f); // フレーム相当で手早く
		Vector3 offset = { 0.0f, 0.0f, s * shakeAmp_ * u };
		cameraTransform.translate = camTargetPos_ + offset;
		if (startTimer_ >= shakeTime_) {
			startPhase_ = StartCamPhase::None;
			startTimer_ = 0.0f;
			cameraTransform.translate = camTargetPos_;
		}
	} break;

	case StartCamPhase::None:
	default:
		// 何もしない（通常進行）
		break;
	}
}



void GamePlayScene::Finalize()
{

	/// オーディオの終了処理
	Audio::GetInstance()->SoundUnload(&soundData);

	/// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();


}


void GamePlayScene::DrawImgui()
{
#ifdef _DEBUG
	ImGui::Begin("Camera Settings / GamePlayScene");
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.1f);
	ImGui::SeparatorText("Start Cam Params");
	ImGui::DragFloat("durDollyIn", &durDollyIn_, 0.01f, 0.05f, 3.0f);
	ImGui::DragFloat("durSettle", &durSettle_, 0.01f, 0.05f, 1.0f);
	ImGui::DragFloat("overshoot", &overShootAmt_, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("shakeTime", &shakeTime_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("shakeAmp", &shakeAmp_, 0.001f, 0.0f, 0.5f);
	if (ImGui::Button("Replay Start Cam")) {
		// 現在の目標位置を基準にリプレイ
		camTargetPos_ = cameraTransform.translate;
		const float pullBack = 12.0f;
		camStartPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z + pullBack };
		camOvershootPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z - overShootAmt_ };
		cameraTransform.translate = camStartPos_;
		startPhase_ = StartCamPhase::DollyIn;
		startTimer_ = 0.0f;
	}

	//// clear関連のimgui / スプライト２枚の調整とclearBackの調整
	//ImGui::SeparatorText("Clear Settings");

	//// ClearSprite
	//{
	//	Vector2 pos = clearSprite->GetPosition();
	//	if (ImGui::DragFloat2("ClearSprite Position", &pos.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite->SetPosition(pos);
	//	}
	//	Vector2 size = clearSprite->GetSize();
	//	if (ImGui::DragFloat2("ClearSprite Size", &size.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite->SetSize(size);
	//	}
	//}

	//// ClearSprite2
	//{
	//	Vector2 pos2 = clearSprite2->GetPosition();
	//	if (ImGui::DragFloat2("ClearSprite2 Position", &pos2.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite2->SetPosition(pos2);
	//	}
	//	Vector2 size2 = clearSprite2->GetSize();
	//	if (ImGui::DragFloat2("ClearSprite2 Size", &size2.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite2->SetSize(size2);
	//	}
	//}

	//// ClearSprite3
	//{
	//	Vector2 pos3 = clearSprite3->GetPosition();
	//	if (ImGui::DragFloat2("ClearSprite3 Position", &pos3.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite3->SetPosition(pos3);
	//	}
	//	Vector2 size3 = clearSprite3->GetSize();
	//	if (ImGui::DragFloat2("ClearSprite3 Size", &size3.x, 1.0f, 0.0f, 1920.0f)) {
	//		clearSprite3->SetSize(size3);
	//	}
	//}

	//// clearBack
	//{
	//	Vector3 clearBackPos = clearBack->GetTranslate();
	//	if (ImGui::DragFloat3("ClearBack Position", &clearBackPos.x, 0.1f, -100.0f, 100.0f)) {
	//		clearBack->SetTranslate(clearBackPos);
	//	}
	//	Vector3 clearBackScale = clearBack->GetScale();
	//	if (ImGui::DragFloat3("ClearBack Scale", &clearBackScale.x, 0.1f, 0.1f, 100.0f)) {
	//		clearBack->SetScale(clearBackScale);
	//	}
	//	Vector3 clearBackRotate = clearBack->GetRotate();
	//	if (ImGui::DragFloat3("ClearBack Rotate", &clearBackRotate.x, 0.1f, -180.0f, 180.0f)) {
	//		clearBack->SetRotate(clearBackRotate);
	//	}
	//	
	//}

	ImGui::End();

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
#endif
	
}