#include "GamePlayScene.h"
#include "Game/Application/Map/Map.h"
#include "Game/Application/Player/Player.h"
#include "Game/Collision/CollisionManager.h"
#include "Game/Particle/ParticleManager.h"
#include "InsideScene/Framework.h"
#ifdef USE_IMGUI
#include "engine/bace/ImGuiManager.h"
#endif
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
	camera = Framework::GetMainCamera();
	// カメラの初期設定


	camStartPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z + pullBack };

	// 強めに引く
	camOvershootPos_ = { camTargetPos_.x, camTargetPos_.y, camTargetPos_.z - overShootAmt_ };

	// 目標位置
	cameraTransform.translate = camStartPos_;
	// カメラに反映
	camera->SetTranslate(cameraTransform.translate);

	// スタート演出の開始
	startPhase_ = StartCamPhase::DollyIn;
	startTimer_ = 0.0f;


	// マップ
	map = std::make_unique<Map>();
	map->Initialize("test");


	collision_ = std::make_unique<CollisionManager>();

	/// プレイヤーの初期化
	player = std::make_unique<Player>();
	player->Initialize(Vector3{ 2.0f,2.0f,0.0f });
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

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();

	// 自機がダメージを受けたら画面シェイク中に出すスプライト
	enemyHitSprite_ = std::make_unique<Sprite>();
	enemyHitSprite_->Initialize("resources/HitDamage.png");
	enemyHitSprite_->SetPosition({ 0.0f,0.0f });
	enemyHitSprite_->SetSize({ 1280.0f,720.0f });
	
	enemyHitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	/*ParticleManager::GetInstance()->CreateParticleGroup("test", "resources/HitDamage.png");
	testParticle_ = std::make_unique<ParticleEmitter>();
	testParticle_->SetParticleName("test");
	testParticle_->SetTranslate({ 5.0f,-5.0f,0.0f });
	testParticle_->SetScale({ 0.3f,0.3f,0.3f });*/
	
}


void GamePlayScene::Update()
{
	camera->Update();
	enemyHitSprite_->Update();

	titleLogoObject->Update();

	backGround->Update();


	// スタート演出中はカメラを更新
	if (stageStartEventFlag_) {
		UpdateStartCamera(dt);
	}

	bool isEnemyHitNow = player->GetHitEnemy();

	// プレイヤーが敵に当たったらカメラをシェイクする
	if (player->GetHitEnemy()&& !wasEnemyHit_) {
		enemyHitShakeActive_ = true;
		enemyHitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemyHitTimer_ = 0.0f;
		// カメラの元の位置を保存
		enemyHitBasePos = camera->GetTranslate();
	
	}
	Vector4 currentColor = enemyHitSprite_->GetColor();
	if (currentColor.w > 0.0f) {
		// 減算スピードは 0.01f ～ 0.05f くらいで調整してください
		float newAlpha = (std::max)(0.0f, currentColor.w - 0.02f);
		enemyHitSprite_->SetColor({ currentColor.x, currentColor.y, currentColor.z, newAlpha });
	}
	wasEnemyHit_ = isEnemyHitNow;

	// カメラシェイクの更新
	if(enemyHitShakeActive_){
		// 
		EnemyHitShake(dt);
	}

	//sceneTransition->Update();
	
	/// マップの更新
	map->Update();

	// エネミーレイヤーが変更されたらエネミーを再生成
	if (map->ConsumeEnemyLayerDirtyFlag()) {
		GenerateEnemy();
	}


	/// マップとプレイヤーの判定のためマップチップデータをプレイヤーにも渡す

	cameraController_->SetFollowRange(8.0f, 92.0f);
	/// プレイヤーの更新
	player->Update();
	if (stageStartEventFlag_ == false)
	{
		cameraController_->SetCameraPosition(cameraTransform.translate);
		Vector3 playerPosition = player->GetTranslate();
		cameraController_->SetTargetPosition(playerPosition);
		cameraController_->Update(dt);
		cameraTransform.translate = cameraController_->GetCameraPosition();
		camera->SetTranslate(cameraTransform.translate);
	}
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
	///////////////////
	//  モデルの描画   //
	///////////////////
	
	backGround->Draw();

	

	//sceneTransition->Draw();
	/// マップの描画
	map->Draw();
	
	
	/// プレイヤーの描画
	player->Draw();


	/// タイトルロゴの描画
	//titleLogoObject->Draw();

	//testParticle_->Emit();

	/// 敵の描画
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	ParticleManager::GetInstance()->Draw();

	///////////////////
	// スプライトの描画 //
	///////////////////
	if (enemyHitShakeActive_ || enemyHitSprite_->GetColor().w > 0.0f) {
		enemyHitSprite_->Draw();
	}
}

void GamePlayScene::InitializeEnemy()
{

	normalEnemy = EnemyFactory::CreateEnemy("NormalEnemy");
	normalEnemy->Initialize();
	normalEnemy->SetTranslate({ 3.0f,1.5f,0.0f });
	enemies.push_back(std::move(normalEnemy));

	flyingEnemy = EnemyFactory::CreateEnemy("FlyingEnemy");
	flyingEnemy->Initialize();
	flyingEnemy->SetTranslate({ 6.0f,8.5f,0.0f });
	enemies.push_back(std::move(flyingEnemy));



	// csvを読み込み敵の配置情報を取得
	GenerateEnemy();

}

void GamePlayScene::GenerateEnemy()
{
	// エネミーを一旦クリア
	enemies.clear();

	const EnemyLayerData& enemyLayerData = map->GetEnemyLayerData();
	const auto& enemyData = enemyLayerData.enemyData;

	const uint32_t mapHeight = static_cast<uint32_t>(enemyData.size());
	for (uint32_t y = 0; y < mapHeight; y++) {
		const uint32_t mapWidth = static_cast<uint32_t>(enemyData[y].size());
		for (uint32_t x = 0; x < mapWidth; x++) {
			EnemyType type = enemyData[y][x];
			// 敵の種類がNoneならスキップ
			if (EnemyType::None == type) {
				continue;
			}

			// 敵の生成
			// EnemyTypeから敵の種類を特定して生成
			std::string enemyId;
			switch (type) {
			case EnemyType::NormalEnemy:
				enemyId = "NormalEnemy";
				break;
			case EnemyType::FlyingEnemy:
				enemyId = "FlyingEnemy";
				break;
			default:
				// 未知のタイプ
				continue;
			}

			// ファクトリーでエネミーを生成
			auto enemy = EnemyFactory::CreateEnemy(enemyId);
			// 生成失敗チェック
			if (!enemy) continue;
			// エネミーの初期化
			enemy->Initialize();
			// マップ上の位置にセット
			Vector3 enemyPos = map->GetMapChipPositionByIndex(x, y);
			// オフセット
			enemyPos.x += enemySpawnOffset_;
			enemyPos.y -= enemySpawnOffset_;
			enemy->SetTranslate(enemyPos);
			// エネミーリストに追加
			enemies.push_back(std::move(enemy));
		}
	}
}

void GamePlayScene::CheckCollision()
{

	collision_->Clear();

	/// プレイヤー
	if (player) {
		collision_->AddCollider(player.get());
	}

	/// エネミー全種
	for (auto& enemy : enemies) {
		if (!enemy)continue;

		if (enemy->IsAlive()) {
			collision_->AddCollider(enemy.get());
		}
	}

	collision_->CheckAllCollisions();

}

void GamePlayScene::UpdateStartCamera(float dt)
{
	switch (startPhase_) {
	case StartCamPhase::DollyIn: {
		// 早→遅（スムーズな減速）
		startTimer_ += dt;
		float t = EaseOutCubic(startTimer_ / durDollyIn_);
		cameraTransform.translate = Lerp(camStartPos_, camOvershootPos_, t);
		if (startTimer_ >= durDollyIn_) {
			startPhase_ = StartCamPhase::Settle;
			startTimer_ = 0.0f;
		}
	} break;

	case StartCamPhase::Settle: {
		// “ボン”と戻る
		startTimer_ += dt;
		float t = EaseOutBack(startTimer_ / durSettle_);
		cameraTransform.translate = Lerp(camOvershootPos_, camTargetPos_, t);
		if (startTimer_ >= durSettle_) {
			startPhase_ = StartCamPhase::Shake;
			startTimer_ = 0.0f;
			// 目標にピタッ
			cameraTransform.translate = camTargetPos_;
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
		// スタート演出終了
		stageStartEventFlag_ = false;
		break;
	default:
		// 何もしない（通常進行）
		break;
	}
	camera->SetTranslate(cameraTransform.translate);
}

void GamePlayScene::EnemyHitShake(float dt)
{
	// スタート演出中は無効
	if (stageStartEventFlag_) {
		return;

	}
	enemyHitTimer_ += dt;
	
	// 減衰
	float u = std::clamp(1.0f - (enemyHitTimer_ / enemyHitShakeTime_), 0.0f, 1.0f);

	// ２軸(x,y)のシェイク
	float sx = std::sin(enemyHitTimer_ * 80.0f);
	float sy = std::sin(enemyHitTimer_ * 100.0f);

	Vector3 offset = { sx * shakeAmp_ * u, sy * shakeAmp_ * u, 0.0f };

	cameraTransform.translate += offset;

	// シェイク終了
	if (enemyHitTimer_ >= enemyHitShakeTime_) {
		enemyHitTimer_ = 0.0f;
		enemyHitShakeActive_ = false;

		// 最終的には元の位置に戻す
		camera->SetTranslate(enemyHitBasePos);
		cameraTransform.translate = enemyHitBasePos;

	}


}



void GamePlayScene::Finalize()
{
	map->Finalize();

	/// オーディオの終了処理
	Audio::GetInstance()->SoundUnload(&soundData);

	/// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();


}


void GamePlayScene::DrawImgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("Camera Settings / GamePlayScene");
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.1f);
	
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
	if (ImGui::Button("Vertical Camera")) {
		cameraTransform.translate = { 8.0f,20.0f,0.0f };
		cameraTransform.rotate = { 1.6f,0.0f,0.0f };
	}
	ImGui::End();

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
#endif

}