#include "GamePlayScene.h"
#include "Game/Application/Map/Map.h"
#include "Game/Application/Player/Player.h"
#include "Game/Collision/CollisionManager.h"
#include "Game/Particle/ParticleManager.h"
#include "InsideScene/Framework.h"
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
	SpriteCommon::GetInstance()->Initialize(dxCommon);
	/// オーディオの初期化
	Audio::GetInstance()->Initialize();
	soundData = Audio::GetInstance()->LoadWave("resources/mokugyo.wav");
	xaudio2_ = Audio::GetInstance()->GetXAudio2();
	Audio::GetInstance()->SoundPlayWave(xaudio2_, soundData);

	// カメラクラスの生成
	camera = Framework::GetMainCamera();



	// 固定YZ
	introFixedY_ = camTargetPos_.y;
	introFixedZ_ = camTargetPos_.z;
	// 演出開始時のカメラの位置
	cameraTransform.translate = camTargetPos_;
	// カメラに反映
	camera->SetTranslate(cameraTransform.translate);
	// スタート演出の開始
	startPhase_ = StartCamPhase::MoveToLeft;
	startTimer_ = 0.0f;
	stageStartEventFlag_ = true;


	// マップ
	map = std::make_unique<Map>();
	map->Initialize("1-1");


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

	SpritesInitialize();

	stageStartEventFlag_ = true;
	player->SetControlEnabled(false);

	isPlayerControlLocked_ = true;

}


void GamePlayScene::Update()
{
	camera->Update();


	titleLogoObject->Update();

	backGround->Update();


	// スタート演出中はカメラを更新
	if (stageStartEventFlag_) {
		UpdateStartCamera(dt);
	}

	bool isEnemyHitNow = player->GetHitEnemy();

	// プレイヤーが敵に当たったらカメラをシェイクする
	if (player->GetHitEnemy() && !wasEnemyHit_) {
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
	if (enemyHitShakeActive_) {
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
	if (isPlayerControlLocked_) {
		player->SetControlEnabled(false);
	} else {
		player->SetControlEnabled(true);
	}


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
	/// スプライトの更新
	SpritesUpdate();

	/// プレイヤーがゴールに触れていたらシーン遷移
	bool isGoal = player->GetIsGoal();
	if (isGoal) {
		sceneManager->ChangeScene("GAMEPLAY");
	}

	/// ImGuiの描画
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

	SpritesDraw();

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
	// 左右の端の座標 / YZは固定
	Vector3 leftPos = { introLeftX_,introFixedY_,introFixedZ_ };
	Vector3 rightPos = { introRightX_,introFixedY_,introFixedZ_ };
	// 開始座標
	Vector3 startPos = { camTargetPos_.x,introFixedY_,introFixedZ_ };

	// StartCamPhaseの各フェーズごとの処理
	switch (startPhase_) {

	case StartCamPhase::MoveToLeft:
	{
		// @todo: 理解説明
		// ゲーム内時間を進める
		startTimer_ += dt;
		// イージング適用
		float t = EaseOutCubic(startTimer_ / introMoveDur_);
		// 左端へ移動
		cameraTransform.translate = Lerp(startPos, leftPos, t);

		// フェーズ移行判定
		if (startTimer_ >= introMoveDur_) {
			// PanToRightへ移行
			startPhase_ = StartCamPhase::PanToRight;
			// タイマーリセット
			startTimer_ = 0.0f;
			// 位置補正
			cameraTransform.translate = leftPos;
		}
	}
	break;

	case StartCamPhase::PanToRight:
	{
		// ゲーム内時間を進める
		startTimer_ += dt;
		// イージング適用
		float t = EaseOutCubic(startTimer_ / introPanDur_);
		// 右端へパン
		cameraTransform.translate = Lerp(leftPos, rightPos, t);

		// フェーズ移行判定 / 右端に到達したら
		if (startTimer_ >= introPanDur_) {
			// Holdへ移行
			startPhase_ = StartCamPhase::Hold;
			// タイマーリセット
			startTimer_ = 0.0f;
			// 位置補正
			cameraTransform.translate = rightPos;
		}
	}
	break;

	case StartCamPhase::Hold:
	{
		// ゲーム内時間を進める
		startTimer_ += dt;
		// そのまま右端で停止
		cameraTransform.translate = rightPos;
		// フェーズ移行判定 / 停止時間経過したら
		if (startTimer_ >= introHoldDur_) {
			// ReturnToStartへ移行
			startPhase_ = StartCamPhase::ReturnToStart;
			// タイマーリセット
			startTimer_ = 0.0f;
		}
	}
	break;

	case StartCamPhase::ReturnToStart:
	{
		// ゲーム内時間を進める
		startTimer_ += dt;

		// 0～1に正規化（duration=0対策込み）
		float u = (introReturnDur_ > 0.0f) ? (startTimer_ / introReturnDur_) : 1.0f;
		u = std::clamp(u, 0.0f, 1.0f);

		// イージング適用（Backは1を超え得るので必要ならclamp）
		float t = EaseOutBack(u);
		t = std::clamp(t, 0.0f, 1.0f);

		// 開始地点へ戻る
		cameraTransform.translate = Lerp(rightPos, startPos, t);

		// フェーズ移行判定 / 開始地点に戻ったら
		if (startTimer_ >= introReturnDur_) {
			// 演出終了
			startPhase_ = StartCamPhase::None;
			// タイマーリセット
			startTimer_ = 0.0f;
			// 位置補正（最終フレームは必ず目標に固定）
			cameraTransform.translate = startPos;
		}
	} break;


	case StartCamPhase::None:
		// 演出終了
		stageStartEventFlag_ = false;
		isPlayerControlLocked_ = false;
		player->SetControlEnabled(true);

		break;

	default:
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

void GamePlayScene::SpritesInitialize()
{
	// 自機がダメージを受けたら画面シェイク中に出すスプライト
	enemyHitSprite_ = std::make_unique<Sprite>();
	enemyHitSprite_->Initialize("resources/HitDamage.png");
	enemyHitSprite_->SetPosition({ 0.0f,0.0f });
	enemyHitSprite_->SetSize({ 1280.0f,720.0f });

	enemyHitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	// コントロールUI
	controlUI_D = std::make_unique<Sprite>();
	controlUI_D->Initialize("resources/KyeUI/D.png");
	controlUI_D->SetPosition({ 200.0f,70.0f });
	controlUI_D->SetSize({ 50.0f,50.0f });

	controlUI_A = std::make_unique<Sprite>();
	controlUI_A->Initialize("resources/KyeUI/A.png");
	controlUI_A->SetPosition({ 100.0f,70.0f });
	controlUI_A->SetSize({ 50.0f,50.0f });

	controlUI_S = std::make_unique<Sprite>();
	controlUI_S->Initialize("resources/KyeUI/S.png");
	controlUI_S->SetPosition({ 150.0f,70.0f });
	controlUI_S->SetSize({ 50.0f,50.0f });

	controlUI_W = std::make_unique<Sprite>();
	controlUI_W->Initialize("resources/KyeUI/W.png");
	controlUI_W->SetPosition({ 150.0f,20.0f });
	controlUI_W->SetSize({ 50.0f,50.0f });

	controlUI_DashUI = std::make_unique<Sprite>();
	controlUI_DashUI->Initialize("resources/KyeUI/DashUI.png");
	controlUI_DashUI->SetPosition({ 700.0f,115.0f });
	controlUI_DashUI->SetSize({ 250.0f,100.0f });

	controlUI_move = std::make_unique<Sprite>();
	controlUI_move->Initialize("resources/KyeUI/moveUI.png");
	controlUI_move->SetPosition({ 430.0f,115.0f });
	controlUI_move->SetSize({ 250.0f,100.0f });
}

void GamePlayScene::SpritesUpdate()
{
	enemyHitSprite_->Update();

	// UI
	controlUI_D->Update();
	controlUI_A->Update();
	controlUI_S->Update();
	controlUI_W->Update();
	controlUI_DashUI->Update();
	controlUI_move->Update();

	// 押されているキーだけ色を濃くする
	if (Input::GetInstance()->PushKey(DIK_D)) {
		controlUI_D->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		// 少しだけ右にずらす
		controlUI_D->SetPosition({ 205.0f,70.0f });
	} else {
		controlUI_D->SetColor({ 1.0f,1.0f,1.0f,0.5f });
		controlUI_D->SetPosition({ 200.0f,70.0f });
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		controlUI_A->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		// 少しだけ左にずらす
		controlUI_A->SetPosition({ 95.0f,70.0f });
	} else {
		controlUI_A->SetColor({ 1.0f,1.0f,1.0f,0.5f });
		controlUI_A->SetPosition({ 100.0f,70.0f });
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		controlUI_S->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		controlUI_S->SetPosition({ 150.0f,75.0f });
	} else {
		controlUI_S->SetColor({ 1.0f,1.0f,1.0f,0.5f });
		controlUI_S->SetPosition({ 150.0f,70.0f });
	}
	if (Input::GetInstance()->PushKey(DIK_W)) {
		controlUI_W->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		controlUI_W->SetPosition({ 150.0f,15.0f });
	} else {
		controlUI_W->SetColor({ 1.0f,1.0f,1.0f,0.5f });
		controlUI_W->SetPosition({ 150.0f,20.0f });
	}
	// どこかしらの移動キーが押されていたらDashUIとMoveUIを強調
	if (Input::GetInstance()->PushKey(DIK_W) ||
		Input::GetInstance()->PushKey(DIK_A) ||
		Input::GetInstance()->PushKey(DIK_S) ||
		Input::GetInstance()->PushKey(DIK_D)) {
		UiActive_ = true;


	} else {
		controlUI_DashUI->SetColor({ 1.0f,1.0f,1.0f,0.0f });
		controlUI_move->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	}

	if (UiActive_)
	{
		uiTimer++;
	}

	if (uiTimer > 60) {
		controlUI_move->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	if (uiTimer > 240)
	{
		controlUI_DashUI->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	if (uiTimer >= 300)
	{
		UiActive_ = false;
		uiTimer = 0;
	}
}

void GamePlayScene::SpritesDraw()
{
	if (enemyHitShakeActive_ || enemyHitSprite_->GetColor().w > 0.0f) {
		enemyHitSprite_->Draw();
	}
	if (!isPlayerControlLocked_) {

		controlUI_D->Draw();
		controlUI_A->Draw();
		controlUI_S->Draw();
		controlUI_W->Draw();
	}

	//controlUI_DashUI->Draw();
	//controlUI_move->Draw();
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
	//==============================
	// Start Camera Intro Tuning UI
	//==============================

	// カメラの配置 / 回転修正
	cameraTransform.translate = camera->GetTranslate();
	ImGui::DragFloat3("Camera Position", &cameraTransform.translate.x, 0.1f, -10000.0f, 10000.0f);
	
	cameraTransform.rotate = camera->GetRotate();
	ImGui::DragFloat3("Camera Rotation", &cameraTransform.rotate.x, 0.1f, -180.0f, 180.0f);
	


	if (ImGui::CollapsingHeader("Start Camera Intro", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("StageStartEventFlag: %s", stageStartEventFlag_ ? "true" : "false");
		ImGui::Text("StartPhase: %d", static_cast<int>(startPhase_));
		ImGui::Text("StartTimer: %.3f", startTimer_);

		ImGui::SeparatorText("Positions");
		ImGui::DragFloat("Intro Left X", &introLeftX_, 0.1f, -10000.0f, 10000.0f);
		ImGui::DragFloat("Intro Right X", &introRightX_, 0.1f, -10000.0f, 10000.0f);

		ImGui::DragFloat("Intro Fixed Y", &introFixedY_, 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Intro Fixed Z", &introFixedZ_, 0.1f, -1000.0f, 1000.0f);

		ImGui::SeparatorText("Durations (sec)");
		ImGui::DragFloat("Move To Left Dur", &introMoveDur_, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Pan To Right Dur", &introPanDur_, 0.01f, 0.0f, 20.0f);
		ImGui::DragFloat("Hold Dur", &introHoldDur_, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Return Dur", &introReturnDur_, 0.01f, 0.0f, 10.0f);

		// 事故防止：Right < Left になったら入れ替え
		if (introRightX_ < introLeftX_)
		{
			ImGui::TextColored(ImVec4(1, 0.6f, 0.2f, 1), "Warning: RightX < LeftX. Auto swap available.");
			if (ImGui::Button("Swap Left/Right"))
			{
				std::swap(introLeftX_, introRightX_);
			}
		}

		ImGui::SeparatorText("Controls");

		// その場で演出をやり直す
		if (ImGui::Button("Restart Intro"))
		{
			// 演出の「戻り先」を camTargetPos_ 基準にする場合
			// introFixedY_/Z はUIでいじれるのでそのまま
			startPhase_ = StartCamPhase::MoveToLeft;
			startTimer_ = 0.0f;
			stageStartEventFlag_ = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Skip Intro"))
		{
			startPhase_ = StartCamPhase::None;
			startTimer_ = 0.0f;
			stageStartEventFlag_ = false;
		}

		// camTargetPos_ を調整したい場合（必要なら）
		ImGui::SeparatorText("Target (Return) Position");
		ImGui::DragFloat3("Cam Target Pos", &camTargetPos_.x, 0.05f, -10000.0f, 10000.0f);
	}

	ImGui::SeparatorText("ControlUI Positions");
	Vector2 dPos = controlUI_D->GetPosition();
	Vector2 aPos = controlUI_A->GetPosition();
	Vector2 sPos = controlUI_S->GetPosition();
	Vector2 wPos = controlUI_W->GetPosition();

	Vector2 dSize = controlUI_D->GetSize();
	Vector2 aSize = controlUI_A->GetSize();
	Vector2 sSize = controlUI_S->GetSize();
	Vector2 wSize = controlUI_W->GetSize();

	ImGui::DragFloat2("D Position", &dPos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("D Size", &dSize.x, 1.0f, 0.0f, 500.0f);
	ImGui::DragFloat2("A Position", &aPos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("A Size", &aSize.x, 1.0f, 0.0f, 500.0f);
	ImGui::DragFloat2("S Position", &sPos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("S Size", &sSize.x, 1.0f, 0.0f, 500.0f);
	ImGui::DragFloat2("W Position", &wPos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("W Size", &wSize.x, 1.0f, 0.0f, 500.0f);
	controlUI_D->SetPosition(dPos);
	controlUI_D->SetSize(dSize);
	controlUI_A->SetPosition(aPos);
	controlUI_A->SetSize(aSize);
	controlUI_S->SetPosition(sPos);
	controlUI_S->SetSize(sSize);
	controlUI_W->SetPosition(wPos);
	controlUI_W->SetSize(wSize);

	controlUI_DashUI->SetPosition(controlUI_DashUI->GetPosition());
	Vector2 dashPos = controlUI_DashUI->GetPosition();
	Vector2 dashSize = controlUI_DashUI->GetSize();
	ImGui::DragFloat2("DashUI Position", &dashPos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("DashUI Size", &dashSize.x, 1.0f, 0.0f, 500.0f);
	controlUI_DashUI->SetPosition(dashPos);
	controlUI_DashUI->SetSize(dashSize);
	controlUI_move->SetPosition(controlUI_move->GetPosition());
	Vector2 movePos = controlUI_move->GetPosition();
	Vector2 moveSize = controlUI_move->GetSize();
	ImGui::DragFloat2("MoveUI Position", &movePos.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("MoveUI Size", &moveSize.x, 1.0f, 0.0f, 500.0f);

	controlUI_move->SetPosition(movePos);
	controlUI_move->SetSize(moveSize);




	if (ImGui::Button("Vertical Camera")) {
		cameraTransform.translate = { 8.0f,20.0f,0.0f };
		cameraTransform.rotate = { 1.6f,0.0f,0.0f };
	}
	ImGui::End();

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
#endif

}