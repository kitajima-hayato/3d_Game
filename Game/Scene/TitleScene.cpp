#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "engine/3d/ModelManager.h"
#include "Game/Camera/camera.h"
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
	ParticleManager::GetInstance()->CreateParticleGroup("Particle", "resources/monsterball.png");

	ParticleManager::GetInstance()->CreateParticleGroup("neo", "resources/uvChecker.png");
	// パーティクルエミッターの初期化
	particleEmitter = make_unique<ParticleEmitter>();
	particleEmitter->SetTransform({ {0.0f,0.0f,0.0f}, { 0.0f,0.0f,0.0f }, { -5.0f,0.0f,20.0f } });
	particleEmitter->SetParticleName("Particle");

	particleEmitter2 = make_unique<ParticleEmitter>();
	particleEmitter2->SetTransform({ { 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f },{ 5.0f,0.0f,20.0f } });
	particleEmitter2->SetParticleName("neo");


	levelData = std::make_unique<LevelLoader>();
	levelData->Load("4");
	levelData->CreateObject();



	playerObject = std::make_unique<Object3D>();
	playerObject->Initialize();
	playerObject->SetModel("Player.obj");


	if (levelData->HasPlayerSpawn()) {
		const auto& playerSpawn = levelData->GetPlayerSpawns()[0];

		playerObject->SetTranslate(playerSpawn.transform.translate);
		playerObject->SetRotate(playerSpawn.transform.rotate);

	}

	/*for (auto& enemyData : levelData->GetEnemySpawns()) {
		auto enemy = std::make_unique<LevelEnemy>();
		Transform enemyTransform = enemyData.transform;
		enemy->Initialize();
		enemy->SetModelName(enemyData.fileName);
		enemy->SetTransform(enemyTransform);
		enemies.push_back(std::move(enemy));



	}*/
	
	for (auto& enemy : levelData->GetEnemySpawns()) {
		auto enemyobject = std::make_unique<Object3D>();
		enemyobject->Initialize();
		enemyobject->SetModel(enemy.fileName);
		enemyobject->SetTranslate(enemy.transform.translate);
		enemies.push_back(std::move(enemyobject));

	}
	



#pragma region 演出
	EffectManager::GetInstance()->CreateEffectGroup("Ring", "resources/monsterball.png");
	effectEmitter = make_unique<EffectEmitter>();

	effectTransform = effectEmitter->GetTransform();
	effectTransform = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{-1.0f,0.0f,10.0f},
	};
	effectEmitter->SetTransform(effectTransform);
	effectEmitter->SetEffectName("Ring");

	EffectManager::GetInstance()->CreateEffectGroup("Cylinder", "resources/gradationLine.png");
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


	particleEmitter->SetTransform({
	emitterScale,
	emitterRotate,
	emitterTranslate
		});
#ifdef _DEBUG
	DrawImgui();
#endif


	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 
		effectEmitter->EmitCylinder();
		cylinder->EmitRing();
	}


	particleEmitter->Update();
	particleEmitter2->Update();



	levelData->Update();
	playerObject->Update();

	for (auto& enemy : enemies) {
		enemy->Update();
	}



	// ENTERキーが押されたら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::Draw()
{

#pragma region 3Dオブジェクトの描画準備

	//sprite_->Draw();
#pragma endregion

#pragma region 3Dオブジェクトの描画

	
	levelData->Draw();
	playerObject->Draw();
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	

	// パーティクルの描画
	//ParticleManager::GetInstance()->Draw();
	//particleEmitter->Emit();
	//particleEmitter2->Emit();
	// エフェクトの描画

	//EffectManager::GetInstance()->DrawRing();
	//EffectManager::GetInstance()->DrawCylinder();

#pragma endregion

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
	sprite_->Initialize("resources/gradationLine.png");
	sprite_->SetPosition({ 0.0f,0.0f });
	sprite_->SetRotation(0.0f);
}

void TitleScene::DrawImgui() {
#ifdef _DEBUG


	//if (levelData->HasPlayerSpawn()) {
	//	const auto& playerSpawn = levelData->getPlayerSpawns()[0];

	//	// プレイヤーの位置と回転を Object3D にセット
	//	playerObject->SetTranslate(playerSpawn.transform.translate);
	//	playerObject->SetRotate(playerSpawn.transform.rotate);

	//	// ImGui ウィンドウで表示
	//	ImGui::Begin("PlayerSpawn Info");

	//	const Vector3& pos = playerSpawn.transform.translate;
	//	const Vector3& rot = playerSpawn.transform.rotate;

	//	ImGui::Text("Translate: X = %.2f, Y = %.2f, Z = %.2f", pos.x, pos.y, pos.z);
	//	ImGui::Text("Rotate:    X = %.2f, Y = %.2f, Z = %.2f", rot.x, rot.y, rot.z);

	//	ImGui::End();
	//}




	//ImGui::Begin("Particle");

	//ImGui::Text("Transform");
	//ImGui::DragFloat3("Scalea", &emitterScale.x, 0.1f);
	//ImGui::DragFloat3("Rotatea", &emitterRotate.x, 0.1f);
	//ImGui::DragFloat3("Translatea", &emitterTranslate.x, 1.0f); // ここでtest.xを操作可能に

	//ImGui::End();

	/*ImGui::Begin("effect");

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
	ImGui::End();*/
	ImGui::Begin("Enemies Info");

	for (size_t i = 0; i < enemies.size(); ++i) {
		Transform transform = enemies[i]->GetTransform();

		ImGui::PushID(static_cast<int>(i)); // 重複防止
		ImGui::Text("Enemy %zu", i);
		if (ImGui::DragFloat3("Position", &transform.translate.x, 0.1f)) {
			enemies[i]->SetTransform(transform); // 変更を反映
		}
		ImGui::PopID();
	}

	ImGui::End();

#endif  _DEBUG

}

