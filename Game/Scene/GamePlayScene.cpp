#include "GamePlayScene.h"
#include "Game/Application/Map.h"
#include "engine/bace/ImGuiManager.h"
#include "Game/Camera/camera.h"
#include "Game/Application/Player.h"
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


	// マップ
	map = std::make_unique<Map>();
	map->Initialize("TestStage.csv");

	enemy = std::make_unique<Object3D>();
	enemy->Initialize();
	enemy->SetModel("Tentativeenemy.obj");
	enemy->SetTransform(enemyTransform);


	/// プレイヤーの初期化
	player = std::make_unique<Player>();
	player->Initialize();

	InitializeEnemy();

}


void GamePlayScene::Update()
{
	/// マップの更新
	map->Update();
	/// プレイヤーの更新
	player->Update();
	/// 敵を左に
	enemyTransform.translate.x -= 0.01f;
	enemy->SetTransform(enemyTransform);

	enemy->Update();
	normalEnemy->Update();
	flyingEnemy->Update();
}

void GamePlayScene::Draw()
{

	/// マップの描画
	map->Draw();
	/// プレイヤーの描画
	player->Draw();

	/// 敵の描画
	enemy->Draw();
	normalEnemy->Draw();
	flyingEnemy->Draw();
}

void GamePlayScene::InitializeEnemy()
{
	
	normalEnemy = EnemyFactory::CreateEnemy("NormalEnemy");
	normalEnemy->Initialize();

	flyingEnemy = EnemyFactory::CreateEnemy("FlyingEnemy");
	flyingEnemy->Initialize();

	
}



void GamePlayScene::Finalize()
{

	/// オーディオの終了処理
	Audio::GetInstance()->SoundUnload(&soundData);

	/// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();


}


