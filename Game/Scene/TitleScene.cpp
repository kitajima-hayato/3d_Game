#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "engine/3d/ModelManager.h"
#include "Game/Camera/camera.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/ParticleEmitter.h"
#include "engine/InsideScene/Framework.h"
TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(DirectXCommon* dxCommon)
{
	// スプライトの初期化
	SpriteCommon::GetInstance().Initialize(dxCommon);


	// マルチスレッドでの読み込み
	LoadAudio();
	LoadSprite();

	// パーティクルグループを作成
	ParticleManager::GetInstance()->CreateParticleGroup("Particle", "resources/monsterball.png");
	particleEmitter = make_unique<ParticleEmitter>();
	particleEmitter->SetParticleName("Particle");

	ParticleManager::GetInstance()->CreateParticleGroup("neo", "resources/back1.png");
	particleEmitter2 = make_unique<ParticleEmitter>();
	particleEmitter2->SetTransform({ { 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f },{ 5.0f,0.0f,20.0f } });
	particleEmitter2->SetParticleName("neo");

	object3D = make_unique<Object3D>();
	object3D->Initialize();

	object3D->SetModel("plane.obj");
	object3D->SetTranslate(Vector3(-4.0f, 0.0f, 10.0f));

	object3D->SetScale(Vector3(0.2f, 0.2f, 0.2f));
	speed = object3D->GetTranslate();



	playerObject = std::make_unique<Object3D>();
	playerObject->Initialize();
	playerObject->SetModel("Player.obj");
	playerTransform = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,-7.0f,20.0f }
	};
	playerObject->SetTransform(playerTransform);


	


	//sceneTransition = std::make_unique<SceneTransition>();
	//sceneTransition->Initialize();

	background = std::make_unique<BackGround>();
	background->Initialize();

	// カメラの取得と設定
	camera = Framework::GetMainCamera();
	camera->SetTranslate({7.5f,-4.0f,0.0f});
	camera->SetRotate({ 0.0f, 0.0f, 0.0f });

	// タイトルスプライトの初期化
	titleSprite = std::make_unique<Sprite>();
	titleSprite->Initialize("resources/Title/Title.png");
	titleSprite->SetPosition({ 0.0f,0.0f });
	titleSprite->SetSize({ 1280.0f,720.0f });

	// Press Start スプライトの初期化
	pressStartSprite = std::make_unique<Sprite>();
	pressStartSprite->Initialize("resources/Title/PushEnter.png");
	pressStartSprite->SetPosition({ -20.0f,100.0f });
	pressStartSprite->SetSize({ 1280.0f,720.0f });

}

void TitleScene::Update()
{
	//sprite_->Update();
	camera->Update();

	background->Update();

	

	//sceneTransition->Update();

#ifdef USE_IMGUI
	DrawImgui();
#endif


	
	// エミッタの位置を更新（プレイヤーに追従させるなど）
	Transform transform;
	transform.translate = { 1.0f, -5.0f, 15.0f };
	particleEmitter->SetTransform(transform);
	//particleEmitter->Update();
	//particleEmitter2->Update();


	titleSprite->Update();
	pressStartSprite->Update();
	// 点滅させる

	static int blinkTimer;
	blinkTimer++;
	if (blinkTimer > blinkDuration) {
		pressStartSprite->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	if (blinkTimer > blinkInterval) {
		pressStartSprite->SetColor({ 1.0f,1.0f,1.0f,0.0f });
		blinkTimer = 0;
	}


	


	// プレイヤーを回転
	playerTransform.rotate.z -= 0.05f; // 回転速度は調整可能
	playerObject->SetTransform(playerTransform);
	playerObject->Update();



	// ENTERキーが押されたら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	else if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("STAGESELECT");
	}

}

void TitleScene::Draw()
{

#pragma region 3Dオブジェクトの描画準備

	//sprite_->Draw();
#pragma endregion

#pragma region 3Dオブジェクトの描画

	background->Draw();

	//object3D->Draw();
	//levelData->Draw();
	playerObject->Draw();
	//Rainbow->Draw();

	

	// パーティクルの描画
	ParticleManager::GetInstance()->Draw();
	//particleEmitter->Emit();
	//particleEmitter2->Emit();
	// エフェクトの描画


	//EffectManager::GetInstance()->DrawRing();
	//EffectManager::GetInstance()->DrawCylinder();
	


	//sceneTransition->Draw();


#pragma endregion

	titleSprite->Draw();
	pressStartSprite->Draw();

}

void TitleScene::Finalize()
{


	// オーディオの終了処理
	//Audio::GetInstance()->SoundUnload(&soundData);
	/*ParticleManager::GetInstance()->DeleteParticleGroup("Particle");
	EffectManager::GetInstance()->DeleteEffectGroup("Cylinder");
	EffectManager::GetInstance()->DeleteEffectGroup("Ring");*/

	// スプライトの終了処理
	SpriteCommon::GetInstance().DeleteInstance();
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
	sprite_->Initialize("resources/gradationLine.png");
	sprite_->SetPosition({ 0.0f,0.0f });
	sprite_->SetRotation(0.0f);
}

void TitleScene::DrawImgui() {
#ifdef USE_IMGUI

	ImGui::Begin("playerObject");

	Transform pTransform = playerObject->GetTransform();
	ImGui::DragFloat3("Scale", &pTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &pTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &pTransform.translate.x, 0.1f);

	playerObject->SetTransform(pTransform);

	ImGui::End();

	


#endif // _DEBUG

}

