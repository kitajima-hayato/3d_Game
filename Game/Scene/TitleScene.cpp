#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "Game/Camera/camera.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/ModelParticleManager.h"
#include "Game/Particle/ParticleSystem.h"
#include "engine/InsideScene/Framework.h"
#include "Game/Particle/ParticlePresets.h"
using Engine::DirectXCommon;
TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(DirectXCommon* dxCommon)
{
	// マルチスレッドでの読み込み
	LoadAudio();
	LoadSprite();

	// パーティクルグループを作成
	particleEmitter = ParticlePresets::CreateSmoke({ 1.0f,-7.0f,20.0f });
	particleEmitter->Pause();

	
	particleEmitter2 = ParticlePresets::CreateSparks({ 1.0f,-7.0f,20.0f });
	particleEmitter2->Pause();


	presetEffect = ParticlePresets::CreateSummonCircle({5.0f,-7.0f,20.0f});
	//presetEffect->Play();




	playerObject = std::make_unique<Object3D>();
	playerObject->Initialize();
	playerObject->SetModel("GamePlay/Player");
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
	
	camera->SetTranslate(initCameraTransform.translate);
	camera->SetRotate(initCameraTransform.rotate);

	// タイトルスプライトの初期化
	titleSprite = std::make_unique<Sprite>();
	titleSprite->Initialize("resources/Scenes/Title/UI/Texture/Title.png");
	titleSprite->SetPosition({ 0.0f,0.0f });
	titleSprite->SetSize({ 1280.0f,720.0f });

	// Press Start スプライトの初期化
	pressStartSprite = std::make_unique<Sprite>();
	pressStartSprite->Initialize("resources/Scenes/Title/UI/Texture/PushSpace.png");
	pressStartSprite->SetPosition({ -20.0f,100.0f });
	pressStartSprite->SetSize({ 1280.0f,720.0f });


	basePlayerY_ = playerTransform.translate.y;

	// 常時煙（ループ）
	titleSmoke_ = ParticlePresets::CreateSmoke(playerTransform.translate);
	titleSmoke_->Pause();
	titleSmoke_->SetLoop(true);
	titleSmoke_->SetEmissionRate(15.0f);
	// 砂煙っぽい色（Player.cppのdashSmokeEffect_と同系）
	titleSmoke_->SetColor(Vector4(0.7f, 0.6f, 0.4f, 1.0f));
	titleSmoke_->Play(); // 重要：Playはここで1回だけ（毎フレームしない）

	// ジャンプ開始スパーク（単発）
	titleJumpSpark_ = ParticlePresets::CreateSparks(playerTransform.translate);
	titleJumpSpark_->Pause();
	titleJumpSpark_->SetLoop(false);
	titleJumpSpark_->SetEmissionRate(50.0f); // 派手目

	// 着地スパーク（単発）
	titleLandSpark_ = ParticlePresets::CreateSparks(playerTransform.translate);
	titleLandSpark_->Pause();
	titleLandSpark_->SetLoop(false);
	titleLandSpark_->SetEmissionRate(30.0f);
	titleLandSpark_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
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
	particleEmitter->Update();
	particleEmitter2->Update();
	presetEffect->Update();

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
		SceneManager::GetInstance()->ChangeSceneWithTransition("GAMEPLAY",TransitionType::Start);
	}
	else if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeSceneWithTransition("STAGESELECT",TransitionType::Start);
	}

	if (Input::GetInstance()->TriggerKey(DIK_0))
	{
		particleEmitter->Stop();
		particleEmitter2->Stop();
		presetEffect->Stop();
	} else if (Input::GetInstance()->TriggerKey(DIK_9))
	{
		//particleEmitter->Play();
		//particleEmitter2->Play();
		//presetEffect->Play();
	}
	// (1) ジャンプ開始判定（一定間隔）
	jumpIntervalTimer_++;
	if (!isJumping_ && jumpIntervalTimer_ >= jumpIntervalFrame_) {
		isJumping_ = true;
		jumpFrame_ = 0;
		jumpIntervalTimer_ = 0;

		// ジャンプ開始スパーク：足元
		Vector3 footPos = playerTransform.translate;
		footPos.y -= footOffsetY_;
		if (titleJumpSpark_) {
			titleJumpSpark_->SetTranslate(footPos);
			titleJumpSpark_->Play();
		}
	}

	// (2) ジャンプ中の上下（sinで上がって下がる）
	if (isJumping_) {
		jumpFrame_++;

		float t = static_cast<float>(jumpFrame_) / static_cast<float>(jumpDurationFrame_);
		if (t > 1.0f) t = 1.0f;

		// 0 -> PI のsin：最初と最後が0、中央が最大
		const float pi = 3.14159265f;
		float yOffset = std::sin(t * pi) * jumpHeight_;

		playerTransform.translate.y = basePlayerY_ + yOffset;

		// 終了（着地）
		if (jumpFrame_ >= jumpDurationFrame_) {
			isJumping_ = false;
			jumpFrame_ = 0;
			playerTransform.translate.y = basePlayerY_;

			// 着地スパーク：足元
			Vector3 footPos = playerTransform.translate;
			footPos.y -= footOffsetY_;
			if (titleLandSpark_) {
				titleLandSpark_->SetTranslate(footPos);
				titleLandSpark_->Play();
			}
		}
	}

	// (3) パーティクル位置追従（足元）
	Vector3 footPos = playerTransform.translate;
	footPos.y -= footOffsetY_;

	if (titleSmoke_) {
		titleSmoke_->SetTranslate(footPos);
		titleSmoke_->Update();
	}
	if (titleJumpSpark_) {
		titleJumpSpark_->Update();
	}
	if (titleLandSpark_) {
		titleLandSpark_->Update();
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
	ModelParticleManager::GetInstance().Draw();

	//sceneTransition->Draw();


#pragma endregion

	titleSprite->Draw();
	pressStartSprite->Draw();

}

void TitleScene::Finalize()
{


	// オーディオの終了処理
	//Audio::GetInstance()->SoundUnload(&soundData);
	
	
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

