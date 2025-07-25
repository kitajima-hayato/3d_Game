#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "engine/3d/ModelManager.h"
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
	ParticleManager::GetInstance()->CreateParticleGroup("Particle", "resources/uvChecker.png");

	// パーティクルエミッターの初期化
	particleEmitter = make_unique<ParticleEmitter>();
	//particleEmitter->SetTransform({{0.0f,0.0f,0.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,50.0f }});

	particleEmitter->SetParticleName("Particle");

	object3D = make_unique<Object3D>();
	object3D->Initialize();
	object3D->SetModel("cubeR.obj");
	object3D->SetTranslate(Vector3(0.0f, 0.0f, 10.0f));
	object3D->SetScale(Vector3(0.2f, 0.2f, 0.2f));

	levelData = std::make_unique<LevelLoader>();
	levelData->Load("shadow");
	levelData->CreateObject();



	playerObject = std::make_unique<Object3D>();
	playerObject->Initialize();
	playerObject->SetModel("Player.obj");
	if (levelData->HasPlayerSpawn()) {
		const auto& playerSpawn = levelData->getPlayerSpawns()[0];

		playerObject->SetTranslate(playerSpawn.transform.translate);
		playerObject->SetRotate(playerSpawn.transform.rotate);
		
	}

	Rainbow = std::make_unique<Object3D>();
	Rainbow->Initialize();
	Rainbow->SetModel("RainbowPlane.obj");
	rainbowTransform = Rainbow->GetTransform();
	rainbowTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{3.0f,0.0f,15.0f},
	};
	Rainbow->SetTransform(rainbowTransform);





#pragma region 演出
	EffectManager::GetInstance()->CreateEffectGroup("Ring", "resources/gradationLine_flipped.png");
	effectEmitter = make_unique<EffectEmitter>();

	effectTransform = effectEmitter->GetTransform();
	effectTransform = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{-1.0f,0.0f,10.0f},
	};
	effectEmitter->SetTransform(effectTransform);
	effectEmitter->SetEffectName("Ring");

	EffectManager::GetInstance()->CreateEffectGroup("Cylinder", "resources/monsterball.png");
	cylinder = make_unique<EffectEmitter>();
	cylinderTransform = cylinder->GetTransform();

	cylinderTransform = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,10.0f},
	};
	cylinder->SetTransform(cylinderTransform);
	cylinder->SetEffectName("Cylinder");
#pragma endregion

}

void TitleScene::Update()
{
	//sprite_->Update();
	object3D->Update();

	particleEmitter->SetTransform({
	emitterScale,
	emitterRotate,
	emitterTranslate
		});
#ifdef _DEBUG
	DrawImgui();
#endif


	//particleEmitter->Update();

	effectEmitter->EmitCylinder();
	cylinder->EmitRing();

	levelData->Update();
	playerObject->Update();

	//  Rainbow回転（ここを追加）
	rainbowTransform.rotate.y += 0.02f; // 回転速度は調整可能
	Rainbow->SetTransform(rainbowTransform);

	Rainbow->Update();


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
	object3D->Draw();
	levelData->Draw();
	playerObject->Draw();
	Rainbow->Draw();
	// パーティクルの描画
	ParticleManager::GetInstance()->Draw();
	// エフェクトの描画

	//EffectManager::GetInstance()->DrawRing();
	//EffectManager::GetInstance()->DrawCylinder();

	//EffectManager::GetInstance()->DrawCylinder();

	
}

void TitleScene::Finalize()
{


	// オーディオの終了処理
	//Audio::GetInstance()->SoundUnload(&soundData);
	ParticleManager::GetInstance()->DeleteParticleGroup("Particle");
	EffectManager::GetInstance()->DeleteEffectGroup("Cylinder");
	EffectManager::GetInstance()->DeleteEffectGroup("Ring");

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
	sprite_->Initialize("resources/gradationLine_flipped.png");
	sprite_->SetPosition({ 0.0f,0.0f });
	sprite_->SetRotation(0.0f);
}

void TitleScene::DrawImgui() {
#ifdef _DEBUG
	if (levelData->HasPlayerSpawn()) {
		const auto& playerSpawn = levelData->getPlayerSpawns()[0];

		// プレイヤーの位置と回転を Object3D にセット
		playerObject->SetTranslate(playerSpawn.transform.translate);
		playerObject->SetRotate(playerSpawn.transform.rotate);

		// ImGui ウィンドウで表示
		ImGui::Begin("PlayerSpawn Info");

		const Vector3& pos = playerSpawn.transform.translate;
		const Vector3& rot = playerSpawn.transform.rotate;

		ImGui::Text("Translate: X = %.2f, Y = %.2f, Z = %.2f", pos.x, pos.y, pos.z);
		ImGui::Text("Rotate:    X = %.2f, Y = %.2f, Z = %.2f", rot.x, rot.y, rot.z);

		ImGui::End();
	}




	//ImGui::Begin("Particle");

	//ImGui::Text("Transform");
	//ImGui::DragFloat3("Scalea", &emitterScale.x, 0.1f);
	//ImGui::DragFloat3("Rotatea", &emitterRotate.x, 0.1f);
	//ImGui::DragFloat3("Translatea", &emitterTranslate.x, 1.0f); // ここでtest.xを操作可能に

	//ImGui::End();

	ImGui::Begin("effect");

	ImGui::Text("Transform");
	ImGui::DragFloat3("Scale", &effectTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &effectTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &effectTransform.translate.x, 1.0f);
	effectEmitter->SetTransform(effectTransform);
	
	ImGui::Text("asdf");
	ImGui::DragFloat3("CylinderScale", &cylinderTransform.scale.x, 0.1f);
	ImGui::DragFloat3("CylinderRotate", &cylinderTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("CylinderTranslate", &cylinderTransform.translate.x, 1.0f);
	cylinder->SetTransform(cylinderTransform);
	ImGui::End(); 
#endif // _DEBUG
}

