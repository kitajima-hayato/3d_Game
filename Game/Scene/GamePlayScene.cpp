#include "GamePlayScene.h"
#include "Game/Application/Map/Map.h"
#include "Game/Application/Player/Player.h"
#include "Game/Collision/CollisionManager.h"
#include "Game/Particle/ParticleManager.h"
#include "engine/InsideScene/Framework.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
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
	SpriteCommon::GetInstance().Initialize(dxCommon);
	/// オーディオの初期化
	Audio::GetInstance().Initialize();
	soundData = Audio::GetInstance().LoadWave("resources/mokugyo.wav");
	xaudio2_ = Audio::GetInstance().GetXAudio2();
	Audio::GetInstance().SoundPlayWave(xaudio2_, soundData);

	// カメラクラスの生成
	camera = Framework::GetMainCamera();


	// マップ
	map = std::make_unique<Map>();
	map->Initialize("1-1");


	collision_ = std::make_unique<CollisionManager>();

	/// プレイヤーの初期化
	player = std::make_unique<Player>();
	player->Initialize(Vector3{ 2.0f,2.0f,0.0f });
	player->SetMap(map.get());

	InitializeEnemy();

	//sceneTransition = std::make_unique<SceneTransition>();
	//sceneTransition->Initialize(10);

	backGround = std::make_unique<BackGround>();
	backGround->Initialize();

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();

	SpritesInitialize();

	stageStartEventFlag_ = true;
	player->SetControlEnabled(false);

	isPlayerControlLocked_ = true;

	// 自機がダメージを受けたら画面シェイク中に出すスプライト
	enemyHitSprite_ = std::make_unique<Sprite>();
	enemyHitSprite_->Initialize("resources/HitDamage.png");
	enemyHitSprite_->SetPosition({ 0.0f,0.0f });
	enemyHitSprite_->SetSize({ 1280.0f,720.0f });

	enemyHitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
	


	pauseSystem_ = std::make_unique<PauseSystem>();
	pauseSystem_->Initialize();

	gamePlayHUD_ = std::make_unique<GamePlayHUD>();
	gamePlayHUD_->Initialize();

	startCam_ = std::make_unique<StartCamPhase>();
	startCam_->Bind(camera, &cameraTransform);

	startCam_->Start();

}


void GamePlayScene::Update()
{

	if (pauseSystem_->Update()) {
		return;
	}

	// カメラの更新
	camera->Update();


	backGround->Update();
	enemyHitSprite_->Update();
	startCam_->Update(dt);
	stageStartEventFlag_ = startCam_->IsRunning();
	isPlayerControlLocked_ = stageStartEventFlag_;
	player->SetControlEnabled(!isPlayerControlLocked_);
	


	

	bool isEnemyHitNow = player->GetHitEnemy();

	// プレイヤーが敵に当たったらカメラをシェイクする
	if (player->GetHitEnemy() && !wasEnemyHit_) {
		enemyHitShakeActive_ = true;
		//enemyHitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemyHitTimer_ = 0.0f;
		// カメラの元の位置を保存
		enemyHitBasePos = camera->GetTranslate();

	}
	// 敵を点滅させる
	//Vector4 currentColor = enemyHitSprite_->GetColor();
	//if (currentColor.w > 0.0f) {
	//	// 減算スピードは 0.01f ～ 0.05f くらいで調整
	//	float newAlpha = (std::max)(0.0f, currentColor.w - 0.02f);
	//	enemyHitSprite_->SetColor({ currentColor.x, currentColor.y, currentColor.z, newAlpha });
	//}
	wasEnemyHit_ = isEnemyHitNow;

	// カメラシェイクの更新
	if (enemyHitShakeActive_) {
		// 
		EnemyHitShake(dt);
	}

	//sceneTransition->Update();

	// マップの更新
	map->Update();

	// エネミーレイヤーが変更されたらエネミーを再生成
	if (map->ConsumeEnemyLayerDirtyFlag()) {
		GenerateEnemy();
	}


	// カメラのフォロー範囲
	cameraController_->SetFollowRange(8.0f, 92.0f);
	// 開始演出中はプレイヤーへの入力を受け付けない
	if (isPlayerControlLocked_) {
		player->SetControlEnabled(false);
	} else {
		player->SetControlEnabled(true);
	}

	// プレイヤーの更新
	player->Update();

	// フォローカメラ
	if (!startCam_->IsRunning())
	{
		cameraTransform.translate = camera->GetTranslate(); 

		cameraController_->SetFollowRange(8.0f, 92.0f);
		cameraController_->SetCameraPosition(cameraTransform.translate);
		cameraController_->SetTargetPosition(player->GetTranslate());
		cameraController_->Update(dt);

		cameraTransform.translate = cameraController_->GetCameraPosition();
		camera->SetTranslate(cameraTransform.translate);
	}

	// 敵の更新
	for (auto& enemy : enemies) {
		enemy->Update();
	}

	// 当たりは判定
	CheckCollision();
	// スプライトの更新
	gamePlayHUD_->Update();

	// プレイヤーがゴールに触れていたらシーン遷移
	bool isGoal = player->GetIsGoal();
	if (isGoal) {
		sceneManager->ChangeScene("GAMEPLAY");
	}


	// ImGuiの描画
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

	/// 敵の描画
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	ParticleManager::GetInstance()->Draw();

	///////////////////
	// スプライトの描画 //
	///////////////////

	// スプライト描画処理
	SpritesDraw();

	gamePlayHUD_->Draw(pauseSystem_->GetPause(), !isPlayerControlLocked_);

	pauseSystem_->Draw();
}

void GamePlayScene::InitializeEnemy()
{

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
	// 衝突判定実行
	collision_->CheckAllCollisions();

}

void GamePlayScene::UpdateStartCamera(float dt)
{
	
	
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

void GamePlayScene::SpritesInitialize()
{
	

	
}

void GamePlayScene::SpritesUpdate()
{
	

	
	
	
}

void GamePlayScene::SpritesDraw()
{




	if (enemyHitShakeActive_ || enemyHitSprite_->GetColor().w > 0.0f) {
		// ダメージフィードバック
		/*if (damageFeedback) {
			enemyHitSprite_->Draw();
		}*/
	}
	

}



void GamePlayScene::Finalize()
{
	map->Finalize();

	/// オーディオの終了処理
	Audio::GetInstance().SoundUnload(&soundData);

	/// スプライトの終了処理
	SpriteCommon::GetInstance().DeleteInstance();

}


void GamePlayScene::DrawImgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("Camera Settings / GamePlayScene");
	//==============================
	// Start Camera Intro Tuning UI
	//==============================

	// カメラの配置 / 回転修正
	cameraTransform.translate = camera->GetTranslate();
	ImGui::DragFloat3("Camera Position", &cameraTransform.translate.x, 0.1f, -10000.0f, 10000.0f);

	cameraTransform.rotate = camera->GetRotate();
	ImGui::DragFloat3("Camera Rotation", &cameraTransform.rotate.x, 0.1f, -180.0f, 180.0f);



	if (ImGui::Button("Vertical Camera")) {
		cameraTransform.translate = { 8.0f,20.0f,0.0f };
		cameraTransform.rotate = { 1.6f,0.0f,0.0f };
	}
	ImGui::End();

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
#endif

}