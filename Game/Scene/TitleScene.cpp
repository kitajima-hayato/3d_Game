#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "Object3DCommon.h"
TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(DirectXCommon* dxCommon)
{



	// スプライトの初期化
	SpriteCommon::GetInstance()->Initialize(dxCommon);

	// マルチスレッドでの読み込み
	std::thread th1(&TitleScene::LoadAudio, this);
	std::thread th2(&TitleScene::LoadSprite, this);

	th1.join();
	th2.join();


	// パーティクルグループを作成
	ParticleManager::GetInstance()->CreateParticleGroup("Primitive", "resources/circle2.png");

	// パーティクルエミッターの初期化
	particleEmitter = make_unique<ParticleEmitter>();
	particleEmitter->SetTransform({ {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f} });
	particleEmitter->SetParticleName("Primitive");

	object3D = make_unique<Object3D>();
	object3D->Initialize(Object3DCommon::GetInstance());
	object3D->CreateRingMesh(32, 1.0f, 0.2f);
	object3D->SetTranslate({ 0.0f, 0.0f, 0.0f });
	object3D->SetScale({ 1.0f, 1.0f, 1.0f });
	// テクスチャのロード
	TextureManager::GetInstance()->LoadTexture("resources/gradationLine.png");
	// SRVハンドル取得
	auto handle = TextureManager::GetInstance()->GetSrvHandleGPU("resources/gradationLine.png");
	// Object3Dにセット
	object3D->SetTextureHandle(handle);

}

void TitleScene::Update()
{
	sprite_->Update();
	particleEmitter->Update();
	//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		//ParticleManager::GetInstance()->EmitRing(
		//	{ 0, 0, 0 },         // 位置
		//	1.0f, 0.2f,        // 外半径・内半径
		//	"resources/gradationLine.png" // テクスチャ
		//);
	//}
	
	
	object3D->Update();
	// ENTERキーが押されたら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	
}

void TitleScene::Draw()
{
	//Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->DrawSettingCommon();

	//sprite_->Draw();
	if (object3D) {
		object3D->Draw();
	}
	// パーティクルの描画
	ParticleManager::GetInstance()->Draw();
}

void TitleScene::Finalize()
{


	// オーディオの終了処理
	//Audio::GetInstance()->SoundUnload(&soundData);
	ParticleManager::GetInstance()->DeleteParticleGroup("Primitive");

	// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();
}

void TitleScene::LoadAudio()
{
	// オーディオの初期化
	//Audio::GetInstance()->Initialize();
	//soundData = Audio::GetInstance()->LoadWave("resources/fanfare.wav");
	//xaudio2_ = Audio::GetInstance()->GetXAudio2();
	//Audio::GetInstance()->SoundPlayWave(xaudio2_, soundData);

}

void TitleScene::LoadSprite()
{


	sprite_ = make_unique<Sprite>();
	sprite_->Initialize("resources/monsterball.png");
	sprite_->SetPosition({ 0.0f,0.0f });
	sprite_->SetRotation(0.0f);
}


