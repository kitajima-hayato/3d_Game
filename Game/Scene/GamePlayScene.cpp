#include "GamePlayScene.h"
#include "Game/Application/Map.h"
#include "engine/bace/ImGuiManager.h"
#include "Game/Camera/camera.h"
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


}


void GamePlayScene::Update()
{

	map->Update();
}

void GamePlayScene::Draw()
{
	///Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->DrawSettingCommon();

	// マップ
	map->Draw();
}

void GamePlayScene::Finalize()
{

	/// オーディオの終了処理
	Audio::GetInstance()->SoundUnload(&soundData);
	
	/// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();


}
