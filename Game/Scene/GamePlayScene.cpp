#include "GamePlayScene.h"
#include "Game/Application/Map.h"
#include "engine/bace/ImGuiManager.h"
#include "Game/Application/Player.h"
#include "Game/Collision/CollisionManager.h"
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

	camera = std::make_unique<Camera>();
	cameraTransform.translate = { 7.5f,-4.0f,0.0f };
	camera->SetTranslate(cameraTransform.translate);
	Object3DCommon::GetInstance()->SetDefaultCamera(camera.get());


	// マップ
	map = std::make_unique<Map>();
	map->Initialize("TestStage.csv");

	
	collision_ = std::make_unique<CollisionManager>();

	/// プレイヤーの初期化
	player = std::make_unique<Player>();
	player->Initialize();

	InitializeEnemy();

}


void GamePlayScene::Update()
{
	camera->Update();
	/// マップの更新
	map->Update();
	  
	player->BeginFrameHitReset();

	/// マップとプレイヤーの判定のためマップチップデータをプレイヤーにも渡す
	player->SetMapChipField(map.get());

	/// プレイヤーの更新
	player->Update();

	


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

	/// マップの描画
	map->Draw();
	/// プレイヤーの描画
	player->Draw();

	/// 敵の描画
	for (auto& enemy : enemies) {
		enemy->Draw();
	}
}

void GamePlayScene::InitializeEnemy()
{

	normalEnemy = EnemyFactory::CreateEnemy("NormalEnemy");
	normalEnemy->Initialize();
	enemies.push_back(std::move(normalEnemy));

	flyingEnemy = EnemyFactory::CreateEnemy("FlyingEnemy");
	flyingEnemy->Initialize();
	enemies.push_back(std::move(flyingEnemy));




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
		
		if(enemy->IsAlive()) {
			collision_->AddCollider(enemy.get());
		}
	}

	collision_->CheckAllCollisions();

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
	ImGui::Separator();
	ImGui::Begin("Camera Settings / GamePlayeScene");
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.1f);
	ImGui::End();
	camera->SetTranslate(cameraTransform.translate);

	camera->SetRotate(cameraTransform.rotate);
#endif // _DEBUG

	
}